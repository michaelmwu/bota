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

//staticforward PyTypeObject Object;


static int Object_init(PyObject *self, PyObject *args, 
                              PyObject *kwds);


static PyObject *Object_repr(PyObject *self);


PyObject *Object_New(PyTypeObject *type, PyObject *args, PyObject *kwds);
static PyObject *_Object_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
static void Object_dealloc(PyObject *object);

PyObject *object_pyobject_value(OBJ_DATA *obj, char *attr);


//static PyObject *Object_getattro(PyObject *obj, PyObject *name);

#define Object_Require(v) \
if(!Object_Check(v)) \
{log_string(LOG_ERR,"Object_Require: bad type");\
PyErr_SetString(PyExc_TypeError, "Must be an Object"); return NULL;}


OBJECT_GET_STRING(name, obj->name);
OBJECT_GET_STRING(description, obj->description);
OBJECT_GET_STRING(short_desc, obj->short_descr);
OBJECT_GET_DSTRING(type, (item_type_name( obj )) );
OBJECT_GET(location, (obj->carried_by ? Character_FromChar(obj->carried_by) : 
                      Room_FromRoom(obj->in_room)) );

OBJECT_GET_INT(level, obj->level);
OBJECT_GET_INT(cost, obj->cost);

OBJECT_GET_BOOL(isWorn, obj->wear_loc != -1);

#define GETTER(name) {#name, Object_ ## name ## _get}

static PyGetSetDef Object_getsets[] = {
  GETTER(name),
  GETTER(short_desc),
  GETTER(description),
  GETTER(type),
  GETTER(location),
  GETTER(level),
  GETTER(cost),
  GETTER(isWorn),
  {0}
};

#undef GETTER

static PyMethodDef Object_methods[] = 
{
  {NULL,NULL,0,NULL}
};


PyTypeObject ObjectType = {
    PyObject_HEAD_INIT(NULL)
    0,
    "mud.Object",
    sizeof(Object),
    0,
    Object_dealloc,
    0, /*tp_print*/
    0, /*tp_getattr*/
    0, /*tp_setattr*/
    0, /*tp_compare*/
    Object_repr, /*tp_repr*/
    0,
    0,
    0, /*tp_as_mapping*/
    0, 0, 0,
    PyObject_GenericGetAttr, /*tp_getattro*/
    PyObject_GenericSetAttr, /*tp_setattro*/
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "This type represents an Object in the MUD.",
    0, 0, 0, 0,
    0, /*tp_iter*/
    0,
    Object_methods, /*tp_methods*/
    0, /*tp_members*/
    Object_getsets, /*tp_getset*/
    0, /*tp_base*/
    0, 0, 0, 0,
    Object_init, /*tp_init*/
    PyType_GenericAlloc, /*tp_alloc*/
    _Object_new,
    _PyObject_Del, /*tp_free*/
};

static PyObject *_Object_new(PyTypeObject *type, PyObject *args,
                                   PyObject *kwds)
{
    return type->tp_alloc(type, 0);
    //return (PyObject *)PyObject_New(Object, type);
}

PyObject *Object_New(PyTypeObject *type, PyObject *args,
                                   PyObject *kwds)
{
    PyObject *obj = _Object_new(type,args,kwds);
/*    type->tp_init(obj, args, kwds);*/
    return obj;
}

/*
static PyObject *Object_getattro(PyObject *obj, PyObject *name)
{
    OBJ_DATA *object;
    PyObject *result;

    Object_Require(obj);
    object = Object_obj(obj);

    if(!Object_Exact(obj))
    {
        log_string(LOG_GAME,"Object_getattro doing generic lookup for %s",
            PyString_AsString(name) );

        result = PyObject_GenericGetAttr(obj, name);
        if(result) return result;
    }

    if((result = object_pyobject_value(object,PyString_AsString(name)))
        != NULL)
        return result;

    return Py_FindMethod(Object_methods, obj, PyString_AsString(name));
}*/


static int Object_init(PyObject *self, PyObject *args, 
                              PyObject *kwds)
{
/*    log_string(LOG_GAME,"Object_init called");*/

    if(PyType_Type.tp_init(self, args, kwds) < 0)
        return -1;

    return 0;
}



PyObject *Object_FromObj(OBJ_DATA *object)
{
    PyTypeObject *class = NULL;
    Object *o_obj;

    if(!object)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    if(object->pyobject != NULL)
    {
        Py_INCREF((PyObject *)object->pyobject);
        return (PyObject *) object->pyobject;
    }

    if(object->pIndexData->pyclass)
    {
        class = (PyTypeObject*) object->pIndexData->pyclass;
        Py_INCREF(class);
    }
    else class = (PyTypeObject*) find_python_class("standard:Object", FALSE);

    if(class)
    {
        PyObject *emptyargs;

        o_obj = (Object*) class->tp_alloc(class, 0);

        Py_DECREF(class);

        if(!o_obj)
        {
            PyErr_Print();
            return NULL;
        }

        o_obj->obj = object;

        Py_INCREF(Py_None);
        emptyargs = Py_BuildValue("()");
        o_obj->ob_type->tp_init((PyObject*)o_obj, emptyargs, Py_None);
        Py_DECREF(emptyargs);
        Py_DECREF(Py_None);

        return (PyObject*)o_obj;
    }

    bug_f("Object_FromObj: No suitable class found.");

    o_obj = PyObject_New(Object, &ObjectType);

    o_obj->obj = object;

    return (PyObject *) o_obj;
}

static void Object_dealloc(PyObject *self)
{
    OBJ_DATA *obj = Object_obj(self);

/*    log_string(LOG_GAME,"Object_dealloc called");*/

    if(obj)
        obj->pyobject = NULL;

    self->ob_type->tp_free(self);

}

static PyObject *Object_repr(PyObject *self)
{
    OBJ_DATA *obj;
    char buf[MSL * 2];

    Object_Require(self);
    obj = Object_obj(self);

    if(!obj)
    {
        log_string(LOG_ERR,"Repr called for non-existant obj");
        PyErr_SetString(PyExc_ValueError, "Null obj");
        return NULL;
    }

    sprintf(buf, "<Object \"%s\">",obj->name);

    return PyString_FromString( buf );
}

PyObject *object_pyobject_value(OBJ_DATA *obj, char *attr)
{
  switch(attr[0])
  {
    case 'd':
      KEY_STRING("description", obj->description);
      break;
    case 'n':
      KEY_STRING("name", obj->name);
      break;
    case 's':
      KEY_STRING("short_desc", obj->short_descr);
      break;
  }

  return NULL;

}

void show_obj_description(OBJ_DATA *obj, CHAR_DATA *ch)
{
    PyObject *res, *pch;

    if(!obj->pyobject)
        return;

    pch = Character_FromChar(ch);

    Py_INCREF(obj->pyobject);
    res = PyObject_CallMethod(obj->pyobject, "getDesc", "N", pch);

    if(res)
    {

        if(PyString_Check(res))
        {
            PyObject *res2;

            res2 = Py_BuildValue("(O)",res);

            Character_send(pch, res2);

            Py_XDECREF(res2);
        }
        Py_DECREF(res);
    }
    else
    {
        PyErr_Print();
    }


    Py_XDECREF(pch);

    return;
}

