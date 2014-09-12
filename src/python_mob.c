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

DO_FUN(	do_say		);

//staticforward PyTypeObject Character;
PyObject *mud_new_character(PyObject *self, PyObject *args);

static int Character_init(PyObject *self, PyObject *args, 
                              PyObject *kwds);


static PyObject *Character_repr(PyObject *self);

PyObject *Character_send(PyObject *self, PyObject *message);
static PyObject *Character_say(PyObject *self, PyObject *message);
static PyObject *Character_do(PyObject *self, PyObject *args);

PyObject *Character_New(PyTypeObject *type, PyObject *args, PyObject *kwds);
static PyObject *_Character_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
static void Character_dealloc(PyObject *object);

static PyObject *Character_canSee(PyObject *self, PyObject *args);
static PyObject *Character_setPath(PyObject *self, PyObject *args);
static PyObject *Character_findPath(PyObject *self, PyObject *args);
static PyObject *Character_setTeam(PyObject *self, PyObject *args);
static PyObject *Character_moveTo(PyObject *self, PyObject *args);

static char *find_path(ROOM_INDEX_DATA *start, PyObject *target);

static PyObject *Character_pers(PyObject *self, PyObject *args);

PyObject *get_items(CHAR_DATA *ch);

//static PyObject *Character_getattro(PyObject *obj, PyObject *name);

#define Character_Require(v) \
{if(!Character_Check(v)) \
{log_string(LOG_ERR,"Character_Require: bad type");\
PyErr_SetString(PyExc_TypeError, "Must be a Character"); return NULL;} \
else if(!Character_ch(v)) {PyErr_SetString(PyExc_ValueError, \
"Attempt to access deleted Character"); return NULL;} }


static PyMethodDef Character_methods[] = 
{
  {"do",	Character_do, METH_VARARGS,
		"Executes a command."},
  {"send",	Character_send, METH_VARARGS,
		"Sends a message."},
  {"say",	Character_say, METH_VARARGS,
		"Says something to the room."},
  {"canSee",	Character_canSee, METH_VARARGS,
		"Returns visibility of another character or object."},
  {"pers",	Character_pers, METH_VARARGS,
		"Describes another character's name."},
  {"setTeam",	Character_setTeam, METH_VARARGS,
		"Sets the arena team."},
  {"setPath",	Character_setPath, METH_VARARGS,
		"Sets a path for the character to follow."},
  {"findPath",	Character_findPath, METH_VARARGS,
		"Finds a path to a room, character, or object."},
  {"moveTo",	Character_moveTo, METH_VARARGS,
		"Set location to given object or room vnum."},
  {NULL,NULL,0,NULL}
};

CHARACTER_GET_STRING(name,ch->name);
CHARACTER_GET_STRING(short_desc,ch->short_descr);
CHARACTER_GET(in_room, Room_FromRoom(ch->in_room));
CHARACTER_GET(items, get_items(ch));
CHARACTER_GET_BOOL(isNPC, IS_NPC(ch));
CHARACTER_GET_BOOL(isPet, IS_SET(ch->act, ACT_PET));
CHARACTER_GET_INT(id, ch->id);
CHARACTER_GET_INT(vnum, IS_NPC(ch) ? ch->pIndexData->vnum : 0);
CHARACTER_GET_STRING(path,((ch->path[0] != '\0') ? ch->path : NULL));
CHARACTER_GET_INT(hp, ch->hit);
CHARACTER_GET_INT(max_hp, ch->max_hit);


#define GETTER(name) {#name, Character_ ## name ## _get}

static PyGetSetDef Character_getsets[] = {
  GETTER(name),
  GETTER(short_desc),
  GETTER(in_room),
  GETTER(id),
  GETTER(vnum),
  GETTER(isNPC),
  GETTER(isPet),
  GETTER(path),
  GETTER(hp),
  GETTER(max_hp),
  GETTER(items),

  /* aliases */
  {"location", Character_in_room_get},
  {0}
};
  
#undef GETTER

PyTypeObject CharacterType = {
    PyObject_HEAD_INIT(NULL)
    0,
    "mud.Character",
    sizeof(Character),
    0,
    Character_dealloc,
    0, /*tp_print*/
    0, /*tp_getattr*/
    0, /*tp_setattr*/
    0, /*tp_compare*/
    Character_repr, /*tp_repr*/
    0,
    0,
    0, /*tp_as_mapping*/
    0, 0, 0,
    PyObject_GenericGetAttr, /*tp_getattro*/
    PyObject_GenericSetAttr, /*tp_setattro*/
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "This type represents a Character in the MUD.",
    0, 0, 0, 0,
    0, /*tp_iter*/
    0,
    Character_methods, /*tp_methods*/
    0, /*tp_members*/
    Character_getsets,
    0, /*tp_base*/
    0, 0, 0, 0,
    Character_init, /*tp_init*/
    PyType_GenericAlloc, /*tp_alloc*/
    _Character_new,
    _PyObject_Del, /*tp_free*/
};

static PyObject *_Character_new(PyTypeObject *type, PyObject *args,
                                   PyObject *kwds)
{
//    log_string(LOG_GAME,"_Character_New called");

return    type->tp_alloc(type, 0);
//    return (PyObject *)PyObject_New(Character, type);
}

PyObject *Character_New(PyTypeObject *type, PyObject *args,
                                   PyObject *kwds)
{
    PyObject *obj = _Character_new(type,args,kwds);

/*    type->tp_init(obj, args, kwds);*/
    return obj;
}

#if 0
static PyObject *Character_getattro(PyObject *obj, PyObject *name)
{
    CHAR_DATA *ch;
    PyObject *result;

    Character_Require(obj);
    ch = Character_ch(obj);

    if(!Character_Exact(obj))
    {
/*
        log_f("Character_getattro doing generic lookup for %s",
            PyString_AsString(name) );
*/
        result = PyObject_GenericGetAttr(obj, name);
        if(result) return result;
    }

    if((result = character_pyobject_value(ch,PyString_AsString(name)))
        != NULL)
        return result;

    return Py_FindMethod(Character_methods, obj, PyString_AsString(name));
}

#endif

static int Character_init(PyObject *self, PyObject *args, 
                              PyObject *kwds)
{
//    log_string(LOG_GAME,"Character_init called");

    if(PyType_Type.tp_init(self, args, kwds) < 0)
        return -1;

    return 0;
}


PyObject *mud_new_character(PyObject *self, PyObject *args)
{
    Character *ch_obj;

    if (!PyArg_ParseTuple(args, ":new_character"))
        return NULL;

    /* log_string(LOG_GAME,"creating an new Character"); */

    ch_obj = PyObject_New(Character, &CharacterType);

    return (PyObject *) ch_obj;
}

PyObject *Character_send(PyObject *self, PyObject *message)
{
    CHAR_DATA *ch;
    char *arg_string;
    sh_int no_newline = 0;
    int len = 0;
    char buf[MAX_STRING_LENGTH * 3];

    Character_Require(self);
    ch = Character_ch(self);

    if(!PyArg_ParseTuple(message, "s#|h", &arg_string, &len, &no_newline))
        return NULL;

    if(len >= MAX_STRING_LENGTH - 4)
    {
        PyErr_SetString(PyExc_ValueError, "String too long");
        return NULL;
    }

    sprintf(buf, "%s%s\n", arg_string, no_newline ? "\n" : "");


    str_replace_c(buf, "\n\r", "\a");
    str_replace_c(buf, "\n", "\a");
    str_replace_c(buf, "\a", "\n\r");

    if(strlen(buf) >= MAX_STRING_LENGTH - 4)
    {
        PyErr_SetString(PyExc_ValueError, "String too long");
        return NULL;
    }

    send_to_char(buf,ch);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Character_say(PyObject *self, PyObject *message)
{
    CHAR_DATA *ch;
    char *arg_string;
    char buf[MAX_INPUT_LENGTH * 2];
    int len = 0;

    Character_Require(self);
    ch = Character_ch(self);

    if(!PyArg_ParseTuple(message, "s#", &arg_string, &len))
        return NULL;

    if(len >= MAX_INPUT_LENGTH - 2)
    {
        PyErr_SetString(PyExc_ValueError, "String too long");
        return NULL;
    }

    strcpy(buf,arg_string);
    do_say(ch, buf);

    Py_INCREF(Py_None);
    return Py_None;
}

static char *find_path(ROOM_INDEX_DATA *start, PyObject *target)
{

    if(Character_Check(target))
    {
        CHAR_DATA *victim;

        Character_Require(target);
        victim = Character_ch(target);

        if(!victim->in_room)
        {
            return NULL;
        }

        return str_dup(pathfind(start, victim->in_room));
    }
    else if(Object_Check(target))
    {
        OBJ_DATA *obj;
        ROOM_INDEX_DATA *room;

        obj = Object_obj(target);
        /* Fixme: check for object! */

        if(obj->carried_by)
            room = obj->carried_by->in_room;
        else if(obj->in_room)
            room = obj->in_room;
        else
        {
            OBJ_DATA *cont;
            for(cont = obj; cont != NULL; cont = cont->in_obj)
            {
                if(obj->carried_by)
                {
                    room = obj->carried_by->in_room;
                    break;
                }
                else if(obj->in_room)
                {
                    room = obj->in_room;
                    break;
                }
            }
        }

        if(!room)
        {
            return NULL;
        }

        return str_dup(pathfind(start, obj->in_room));

    }
    else if(Room_Check(target))
    {
        ROOM_INDEX_DATA *room;

        room = Room_room(target);

        return str_dup(pathfind(start, room));
    }
    else if(PyInt_Check(target))
    {
        int vnum;
        ROOM_INDEX_DATA *room;

        vnum = (int) PyInt_AsLong(target);

        room = get_room_index(vnum);

        if(!room)
        {
            return NULL;
        }

        return str_dup(pathfind(start, room));
    }

    return NULL;
}

static PyObject *Character_findPath(PyObject *self, PyObject *args)
{
    PyObject *arg, *s;
    CHAR_DATA *ch;
    char *path;

    Character_Require(self);
    ch = Character_ch(self);

    if(!PyArg_ParseTuple(args, "O", &arg))
        return NULL;

    if((path = find_path(ch->in_room, arg)) == NULL)
        return NULL;

    s = PyString_FromString(path);
    free_string(path);

    return s;
}

static PyObject *Character_setPath(PyObject *self, PyObject *args)
{
    CHAR_DATA *ch;
    PyObject *arg;

    Character_Require(self);
    ch = Character_ch(self);

    if(!PyArg_ParseTuple(args, "O", &arg))
        return NULL;

    if(PyString_Check(arg))
    {
        free_string(ch->path);
        ch->path_pos = NULL;
        ch->path = str_dup(PyString_AsString(arg));

        Py_INCREF(Py_None);
        return Py_None;
    }
    else
    {
        char *path;

        if((path = find_path(ch->in_room, arg)) == NULL)
            return NULL;

        free_string(ch->path);
        ch->path_pos = NULL;
        ch->path = path;

        Py_INCREF(Py_None);
        return Py_None;
    }

    /* fixme */
    PyErr_SetString(PyExc_TypeError,"Invalid argument for setPath");
    return NULL;
}


PyObject *Character_FromChar(CHAR_DATA *ch)
{
    PyTypeObject *class = NULL;
    Character *ch_obj;

    if(!ch)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }


    if(ch->pyobject != NULL) /* Already created one */
    {
        Py_INCREF((PyObject *)ch->pyobject);
        return (PyObject *) ch->pyobject;
    }

    if(IS_NPC(ch) && ch->pIndexData->pyclass)
    {
        class = (PyTypeObject*) ch->pIndexData->pyclass;
        Py_INCREF(class);
    }
    else
    {
        class = (PyTypeObject*) find_python_class("standard:Character", FALSE);
    }

    if(class)
    { /* ooh, use the subclass */
        PyObject *emptyargs;

        ch_obj = (Character *) class->tp_alloc(class, 0);
//        ch_obj = (Character *) ((PyTypeObject*)class)->tp_alloc((PyTypeObject*)class, 0);
//        ch_obj = PyObject_New(Character,
//                     (PyTypeObject*)class);

        Py_DECREF(class);

        if(!ch_obj)
        {
            bug_f("Error allocating character!");
            PyErr_Print();
            return NULL;
        }

        ch_obj->ch = ch;

        Py_INCREF(Py_None);
        emptyargs = Py_BuildValue("()");
        ch_obj->ob_type->tp_init(((PyObject*)ch_obj), emptyargs, Py_None);
        Py_DECREF(emptyargs);
        Py_DECREF(Py_None);

        return (PyObject *)ch_obj;
    }

    bug_f("Character_FromChar: No appropriate class found");

    ch_obj = PyObject_New(Character, &CharacterType);

    ch_obj->ch = ch;

    return (PyObject *) ch_obj;
}

static void Character_dealloc(PyObject *self)
{
    CHAR_DATA *ch = Character_ch(self);

/*    log_string(LOG_GAME,"character dealloc"); */

    if(ch)
        ch->pyobject = NULL;

/*    PyObject_Del(self);*/

/*    PyType_Type.tp_dealloc(self);*/

    self->ob_type->tp_free(self);
}


static PyObject *Character_repr(PyObject *self)
{
    CHAR_DATA *ch;
    char buf[MSL * 2];

    Character_Require(self);
    ch = Character_ch(self);

    if(!ch)
    {
        log_string(LOG_ERR,"Repr called for non-existant ch");
        PyErr_SetString(PyExc_ValueError, "Null ch");
        return NULL;
    }

    sprintf(buf, "<Character \"%s\">",ch->name);

    return PyString_FromString( buf );
}


static PyObject *Character_do(PyObject *self, PyObject *args)
{
    CHAR_DATA *ch;
    int len;
    char *arg_string;
    char buf[MAX_INPUT_LENGTH];

    Character_Require(self);
    ch = Character_ch(self);

    if(!PyArg_ParseTuple(args, "s#", &arg_string, &len))
        return NULL;

    if(len >= MAX_INPUT_LENGTH - 2)
    {
        PyErr_SetString(PyExc_ValueError, "String too long");
        return NULL;
    }

    strcpy(buf,arg_string);

    interpret(ch,buf);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Character_canSee(PyObject *self, PyObject *args)
{
    CHAR_DATA *ch;
    PyObject *arg;

    Character_Require(self);
    ch = Character_ch(self);

    if(!PyArg_ParseTuple(args, "O", &arg))
        return NULL;

    if(Character_Check(arg))
    {
        CHAR_DATA *victim;

        Character_Require(arg);
        victim = Character_ch(arg);

        return PyInt_FromLong( can_see(ch, victim) ? 1 : 0 );
    }
    else if(Object_Check(arg))
    {
        OBJ_DATA *obj;

        obj = Object_obj(arg);
        /* Fixme: check for object! */

        return PyInt_FromLong( can_see_obj(ch, obj) ? 1 : 0 );
    }
    else if(Room_Check(arg))
    {
        ROOM_INDEX_DATA *room;

        room = Room_room(arg);

        return PyInt_FromLong( can_see_room(ch, room) ? 1 : 0 );
    }

    /* fixme */
    PyErr_SetString(PyExc_TypeError,"Invalid argument for canSee");
    return NULL;
}



static PyObject *Character_pers(PyObject *self, PyObject *args)
{
    CHAR_DATA *ch;
    PyObject *arg;

    Character_Require(self);
    ch = Character_ch(self);

    if(!PyArg_ParseTuple(args, "O", &arg))
        return NULL;

    if(Character_Check(arg))
    {
        CHAR_DATA *victim;

        Character_Require(arg);
        victim = Character_ch(arg);

        return PyString_FromString(PERS(victim,ch));
    }
    else if(Object_Check(arg))
    {
        OBJ_DATA *obj;

        obj = Object_obj(arg);
        /* Fixme: check for object! */

        return NULL;
    }

    /* fixme */
    PyErr_SetString(PyExc_TypeError,"Invalid argument for pers");
    return NULL;
}

PyObject *get_items(CHAR_DATA *ch)
{
    OBJ_DATA *obj;
    PyObject *object, *list;

    list = PyList_New(0);
    if(!list) return NULL;

    log_f("New list created");

    for(obj = ch->carrying; obj != NULL; obj = obj->next_content)
    {
        int res;
        object = Object_FromObj(obj);
        if(object == NULL) return NULL;

        res = PyList_Append(list, object);
        if(res != 0) return NULL;

        Py_DECREF(object);
    }

    log_f("Returning list");

    return list;
}

static PyObject *Character_setTeam(PyObject *self, PyObject *args)
{
    CHAR_DATA *ch;
    int arg;

    Character_Require(self);
    ch = Character_ch(self);

    if(!PyArg_ParseTuple(args, "i", &arg))
        return NULL;

    if(arg < 0 || arg >= MAX_ARENA_TEAMS)
    {
        PyErr_SetString(PyExc_TypeError,"Invalid team id for setTeam");
        return NULL;
    }

    ch->in_arena = TRUE;
    ch->arena_team = arg;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Character_moveTo(PyObject *self, PyObject *args)
{
    CHAR_DATA *ch;
    PyObject *arg;

    Character_Require(self);
    ch = Character_ch(self);

    if(!PyArg_ParseTuple(args, "O", &arg))
        return NULL;

    if(Room_Check(arg))
    {
        ROOM_INDEX_DATA *room;

        room = Room_room(arg);

        char_to_room(ch, room);
    }
    else if(PyInt_Check(arg))
    {
        int vnum;
        ROOM_INDEX_DATA *room;

        vnum = (int) PyInt_AsLong(arg);
        room = get_room_index(vnum);

        if(!room)
        {
            return NULL;
        }

        char_to_room(ch, room);
    }


    Py_INCREF(Py_None);
    return Py_None;
}


