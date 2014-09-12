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


extern PyTypeObject CharacterType;

void init_MUD_mod();
PyObject *mud_log(PyObject *self, PyObject *args);
PyObject *mud_toRoman(PyObject *self, PyObject *args);
PyObject *mud_ticks(PyObject *self, PyObject *args);
PyObject *mud_spawnMob(PyObject *self, PyObject *args);
PyObject *mud_spawnObj(PyObject *self, PyObject *args);

/* standard python library */
static PyMethodDef MUDMethods[] =
{
    {"log", mud_log, METH_VARARGS, "Logs a message."},
    {"toRoman", mud_toRoman, METH_VARARGS, "Converts to Roman numerals."},
    {"ticks", mud_ticks, METH_VARARGS, "Returns current tick."},
    {"spawnMob", mud_spawnMob, METH_VARARGS, "Spawns a new mob."},
    {"spawnObj", mud_spawnObj, METH_VARARGS, "Spawns a new object."},
    {NULL, NULL, 0, NULL}
};

void init_MUD_mod()
{
    PyObject *module, *dict;

    CharacterType.ob_type = &PyType_Type;
    ObjectType.ob_type = &PyType_Type;
    RoomType.ob_type = &PyType_Type;

    log_string(LOG_GAME,"init_MUD_mod() called");

    if(PyType_Ready(&CharacterType) < 0)
        return;
    if(PyType_Ready(&ObjectType) < 0)
        return;
    if(PyType_Ready(&RoomType) < 0)
        return;


    module = Py_InitModule3("mud", MUDMethods, "Core library for BOTA");
    dict = PyModule_GetDict(module);

    Py_INCREF(&CharacterType);
    if(PyDict_SetItemString(dict, "CharacterType",
       (PyObject *) &CharacterType) < 0)
        log_string(LOG_ERR,"CharacterType dict insertion failed.");

    Py_INCREF(&ObjectType);
    if(PyDict_SetItemString(dict, "ObjectType",
       (PyObject *) &ObjectType) < 0)
        log_string(LOG_ERR,"ObjectType dict insertion failed.");

    Py_INCREF(&RoomType);
    if(PyDict_SetItemString(dict, "RoomType",
       (PyObject *) &RoomType) < 0)
        log_string(LOG_ERR,"RoomType dict insertion failed.");
}

PyObject *mud_log(PyObject *self, PyObject *args)
{
    PyObject *message, *arg;

    if(!PyArg_ParseTuple(args, "O", &arg))
        return NULL;

    message = PyObject_Repr(arg);
    if(!message)
        return NULL;

    log_f("[python] %s", PyString_AsString(message));

    Py_INCREF(Py_None);
    return Py_None;
}

PyObject *mud_toRoman(PyObject *self, PyObject *args)
{
    long num;

    if(!PyArg_ParseTuple(args, "l", &num))
        return NULL;

    return PyString_FromString(long2roman(num));
}


PyObject *mud_spawnMob(PyObject *self, PyObject *args)
{
    int vnum;
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;

    if(!PyArg_ParseTuple(args, "i", &vnum))
        return NULL;

    if ( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
    {
        PyErr_SetString(PyExc_ValueError, "Invalid VNUM to spawn");
        return NULL;
    }

    mob = create_mobile(pMobIndex);

    return Character_FromChar(mob);
}

PyObject *mud_spawnObj(PyObject *self, PyObject *args)
{
    int vnum;
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;

    if(!PyArg_ParseTuple(args, "i", &vnum))
        return NULL;

    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
        PyErr_SetString(PyExc_ValueError, "Invalid VNUM to spawn");
        return NULL;
    }

    obj = create_object(pObjIndex, 0);

    return Object_FromObj(obj);
}

PyObject *mud_ticks(PyObject *self, PyObject *args)
{
    return PyLong_FromLong( (long)current_pulse / PULSE_TICK );
}

PyObject *mud_findCharWorld(PyObject *self, PyObject *args)
{
    CHAR_DATA *ch;
    char *name;
    
    if(!PyArg_ParseTuple(args, "S", &name))
        return NULL;    

    for(;;)
    {
    }

    Character_FromChar( ch );

    return NULL;
}

PyObject *mud_schedule(PyObject *self, PyObject *args)
{
    PyObject *target, *func;
    long time;

    if(!PyArg_ParseTuple(args, "OOl", &target, &func, &time))
        return NULL;

    return NULL;
}

PyObject *world_gecho(PyObject *self, PyObject *args)
{

    Py_INCREF(Py_None);
    return Py_None;
}

/*

static PyObject *mud_act(PyObject *self, PyObject *message)
{
    char *arg_string;
    char buf[MAX_INPUT_LENGTH * 2];
    int len = 0;

    if(!PyArg_ParseTuple(message, "s#", &arg_string, &len))
        return NULL;

    if(len >= MAX_INPUT_LENGTH - 2)
    {
        PyErr_SetString(PyExc_ValueError, "String too long");
        return NULL;
    }

    strcpy(buf,arg_string);

    Py_INCREF(Py_None);
    return Py_None;
}

*/
