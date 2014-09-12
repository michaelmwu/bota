#if defined(macintosh)
#include <types.h> 
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"

#include "Python.h"
#include "python_local.h"
#include "structmember.h"

PyObject *mud_new_room(PyObject *self, PyObject *args);


static int Room_init(PyObject *self, PyObject *args, 
                              PyObject *kwds);


static PyObject *Room_repr(PyObject *self);


PyObject *Room_New(PyTypeObject *type, PyObject *args, PyObject *kwds);
static PyObject *_Room_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
static void Room_dealloc(PyObject *object);

PyObject *room_pyobject_value(ROOM_INDEX_DATA *room, char *attr);


//static PyObject *Room_getattro(PyObject *obj, PyObject *name);

#define Room_Require(v) \
if(!Room_Check(v)) \
{log_string(LOG_ERR,"Room_Require: bad type");\
PyErr_SetString(PyExc_TypeError, "Must be a Room"); return NULL;}

ROOM_GET_STRING(name, room->name);
ROOM_GET_STRING(description, room->description);
ROOM_GET_INT(vnum, room->vnum);

#define GETTER(name) {#name, Room_ ## name ## _get}

static PyGetSetDef Room_getsets[] = {
  GETTER(name),
  GETTER(description),
  GETTER(vnum),
  {0}
};

#undef GETTER


static PyMethodDef Room_methods[] = 
{
  {NULL,NULL,0,NULL}
};

PyTypeObject RoomType = {
    PyObject_HEAD_INIT(NULL)
    0,
    "mud.Room",
    sizeof(Room),
    0,
    Room_dealloc,
    0, /*tp_print*/
    0, /*tp_getattr*/
    0, /*tp_setattr*/
    0, /*tp_compare*/
    Room_repr, /*tp_repr*/
    0,
    0,
    0, /*tp_as_mapping*/
    0, 0, 0,
    PyObject_GenericGetAttr, /*tp_getattro*/
    0, /*tp_setattro*/
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "This type represents a Room in the MUD.",
    0, 0, 0, 0,
    0, /*tp_iter*/
    0,
    Room_methods, /*tp_methods*/
    0, /*tp_members*/
    Room_getsets,
    0, /*tp_base*/
    0, 0, 0, 0,
    Room_init, /*tp_init*/
    PyType_GenericAlloc, /*tp_alloc*/
    _Room_new,
    _PyObject_Del, /*tp_free*/
};

static PyObject *_Room_new(PyTypeObject *type, PyObject *args,
                                   PyObject *kwds)
{
    return type->tp_alloc(type, 0);
/*    return (PyObject *)PyObject_New(Room, type);*/
}

PyObject *Room_New(PyTypeObject *type, PyObject *args,
                                   PyObject *kwds)
{
    PyObject *obj = _Room_new(type,args,kwds);
/*    type->tp_init(obj, args, kwds);*/
    return obj;
}

#if 0
static PyObject *Room_getattro(PyObject *obj, PyObject *name)
{
    ROOM_INDEX_DATA *room;
    PyObject *result;

    Room_Require(obj);
    room = Room_room(obj);

    if(!Room_Exact(obj))
    {
        result = PyObject_GenericGetAttr(obj, name);
        if(result) return result;
    }

    if((result = room_pyobject_value(room,PyString_AsString(name)))
        != NULL)
        return result;

    return Py_FindMethod(Room_methods, obj, PyString_AsString(name));

}
#endif

static int Room_init(PyObject *self, PyObject *args, 
                              PyObject *kwds)
{
//    log_string(LOG_GAME,"Room_init called");

    if(PyType_Type.tp_init(self, args, kwds) < 0)
        return -1;

    return 0;
}


PyObject *Room_FromRoom(ROOM_INDEX_DATA *room)
{
    PyTypeObject *class = NULL;
    Room *room_obj;

    if(!room)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    if(room->pyobject != NULL)
    {
        Py_INCREF((PyObject *)room->pyobject);
        return (PyObject *) room->pyobject;
    }

    if(room->pyclass)
        class = (PyTypeObject *) room->pyclass;
    else class = (PyTypeObject*) find_python_class("standard:Room", FALSE);

    if(class)
    { /* ooh, use the subclass */
        PyObject *emptyargs;

        room_obj = (Room*) class->tp_alloc(class, 0);

        if(!room_obj)
        {
            PyErr_Print();
            return NULL;
        }

        room_obj->room = room;

        Py_INCREF(Py_None);
        emptyargs = Py_BuildValue("()");
        room_obj->ob_type->tp_init((PyObject*)room_obj, emptyargs, Py_None);
        Py_DECREF(emptyargs);
        Py_DECREF(Py_None);

        return (PyObject*)room_obj;
    }


    room_obj = PyObject_New(Room, &RoomType);

    room_obj->room = room;

    return (PyObject *) room_obj;
}

static void Room_dealloc(PyObject *self)
{
    ROOM_INDEX_DATA *room = Room_room(self);

    if(room)
        room->pyobject = NULL;

    self->ob_type->tp_free(self);
}


static PyObject *Room_repr(PyObject *self)
{
    ROOM_INDEX_DATA *room;
    char buf[MSL];

    Room_Require(self);
    room = Room_room(self);

    if(!room)
    {
        log_string(LOG_ERR,"Repr called for non-existant room");
        PyErr_SetString(PyExc_ValueError, "Null room");
        return NULL;
    }

    sprintf(buf, "<Room \"%s\">",room->name);

    return PyString_FromString
        ( room->name );
}

 
PyObject *room_pyobject_value(ROOM_INDEX_DATA *room, char *attr)
{
  switch(attr[0])
  {
    case 'd':
      KEY_STRING("description", room->description);
      break;
    case 'h':
      KEY_INT   ("heal_rate", room->heal_rate);
      break;
    case 'm':
      KEY_INT   ("mana_rate", room->mana_rate);
      break;
    case 'n':
      KEY_STRING("name", room->name);
      break;
    case 'v':
      KEY_INT   ("vnum", room->vnum);
      break;
  }

  return NULL;
}


