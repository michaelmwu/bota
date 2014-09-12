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

void setup_pyclass_mob(MOB_INDEX_DATA *pMob);
void setup_pyclass_obj(OBJ_INDEX_DATA *pObj);

void init_MUD_mod();

/* --------------------------------------------------------------- */

void init_python()
{

    Py_Initialize();

    PyRun_SimpleString("import sys");

    PyRun_SimpleString("sys.path += ['../scripts/']");
    log_string(LOG_GAME,"print 'Python core loaded.'");

    init_MUD_mod();


    PyRun_SimpleString("import mud");
//    PyRun_SimpleString("import standard");

}

void terminate_python()
{
    Py_Finalize();
}

void do_python(CHAR_DATA *ch, char *argument)
{
    PyObject *pch, *module, *dict;

    pch = Character_FromChar(ch);

    if((module = PyImport_AddModule("__main__")) == NULL)
        return;
    if((dict = PyModule_GetDict(module)) == NULL)
        return;

    PyDict_SetItemString(dict,"ch", pch);

    PyRun_SimpleString(argument);

    PyDict_DelItemString(dict,"ch");

    Py_DECREF(pch);

    return;
}


void do_runscript(CHAR_DATA *ch, char *argument)
{

}

void do_pycall(CHAR_DATA *ch, char *argument)
{

}

void do_scriptsetup(CHAR_DATA *ch, char *argument)
{
    int vnum;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);
    vnum = atoi(arg2);

    if(!str_cmp(arg1, "mob"))
    {
        MOB_INDEX_DATA *pMob;
        if((pMob = get_mob_index(vnum)) == NULL)
        {
            send_to_char("VNUM not found.\n\r",ch);
            return;
        }
        if(!str_cmp(arg3, "none"))
        {
            Py_XDECREF(pMob->pyclass);
            pMob->pyclass = NULL;
            return;
        }
        setup_pyclass_mob(pMob);
        return;
    }

    else if(!str_cmp(arg1, "obj"))
    {
        OBJ_INDEX_DATA *pObj;
        if((pObj = get_obj_index(vnum)) == NULL)
        {
            send_to_char("VNUM not found.\n\r",ch);
            return;
        }
        if(!str_cmp(arg3, "none"))
        {
            Py_XDECREF(pObj->pyclass);
            pObj->pyclass = NULL;
            return;
        }

        setup_pyclass_obj(pObj);
        return;
    }
    else
    {
        send_to_char("scriptsetup <mob/object> <vnum>\r\n", ch);
    }

}


/* ----- Merge C/Python data ----- */

void setup_pyclass_mob(MOB_INDEX_DATA *pMob)
{
    PyObject *class;

    if(pMob->pyclass_loc[0] == '\0')
        return;    

    if((class = find_python_class(pMob->pyclass_loc, TRUE)) == NULL)
    {
        return;
    }

    if(pMob->pyclass)
    {
        Py_DECREF(pMob->pyclass);
    }

    pMob->pyclass = class;
}

void setup_pyclass_obj(OBJ_INDEX_DATA *pObj)
{
    PyObject *class;

    if(pObj->pyclass_loc[0] == '\0')
        return;    

    if((class = find_python_class(pObj->pyclass_loc, TRUE)) == NULL)
    {
        return;
    }

    if(pObj->pyclass)
    {
        Py_DECREF(pObj->pyclass);
    }

    pObj->pyclass = class;
}

PyObject *find_python_class(char *classloc, bool reload)
{
    PyObject *module, *dict, *class;
    char modulename[MAX_STRING_LENGTH];
    char classname[MAX_STRING_LENGTH];

    separate_char_end(classloc,':', modulename, classname);

    if(modulename[0] == '\0' || classname[0] == '\0')
    {
        bug_f("find_python_class: Bad classloc: %s %s",modulename,classname);
        return NULL;
    }

    dict = PyImport_GetModuleDict();
    if(PyDict_GetItemString(dict,modulename))
    {
        PyObject *temp;
        temp = PyImport_AddModule(modulename);
        Py_INCREF(temp);
        if(reload)
        {
            module = PyImport_ReloadModule(temp);
            Py_DECREF(temp);
        }
        else
        {
            module = temp;
        }
    }
    else
    {
        module = PyImport_ImportModule(modulename);
    }

    if(!module)
    {
        log_string(LOG_ERR,"find_python_class: load %s failed", classloc);
        PyErr_Print();
        return NULL;
    }

    dict = PyModule_GetDict(module);
    if((class = PyDict_GetItemString(dict,classname)) == NULL)
    {
        log_string(LOG_ERR,"find_python_class: %s: class not found", classname);
        Py_DECREF(module);

        return NULL;
    }

    Py_DECREF(module);

    if(!PyType_Check(class))
    {
        log_string(LOG_ERR,"find_python_class: %s: not a Type subclass", classname);
        return NULL;
    }    

    Py_INCREF(class);
    return class;
}

/* ----- Script calling stuff ----- */

void script_call_noarg(PyObject *target, char *name)
{
    PyObject *result, *pname, *method;

    if(!target) return;

/*
    Py_INCREF(target);
    result = PyObject_CallMethod(target, name, NULL);
    Py_DECREF(target);*/

    pname = PyString_FromString(name);

    method = PyObject_GetAttr(target, pname);
    Py_XDECREF(pname);

    if(!method)
    {
        PyErr_Clear();
        return;
    }

    if(!PyCallable_Check(method))
    {
        /*PyErr_SetString(PyExc_ValueError, "");*/
        bug_f("script_call: not callable: %s", name);
        return;
    }

    result = PyObject_CallFunction(method, NULL);

    if(result)
        Py_DECREF(result);
    else
    {
        PyErr_Print();
    }

    return;
}

PyObject *script_call(PyObject *target, char *name, char *fmt, ...)
{
    PyObject *result, *args, *tupargs, *param, *method, *pname;
    va_list ap;
    bool keepreturn = FALSE;

    if(!target) return NULL;

    pname = PyString_FromString(name);

    method = PyObject_GetAttr(target, pname);
    Py_XDECREF(pname);

    if(!method)
    {
        PyErr_Clear();
        return NULL;
    }
    if(!PyCallable_Check(method))
    {
        /*PyErr_SetString(PyExc_ValueError, "");*/
        bug_f("script_call: not callable: %s", name);
        return NULL;
    }

    args = PyList_New(0);

    va_start(ap, fmt);
    while(*fmt)
    {
        param = NULL;

        switch(*fmt++)
        {
            case 'C':
              param = Character_FromChar(va_arg(ap,CHAR_DATA*));
              break;
            case 'O':
              param = Object_FromObj(va_arg(ap,OBJ_DATA *));
              break;
            case 'R':
              param = Room_FromRoom(va_arg(ap,ROOM_INDEX_DATA *));
              break;
            case 'S':
              param = PyString_FromString(va_arg(ap,char*));
              break;
            case '$':
              keepreturn = TRUE;
              break;
        }
        if(param)
        {
            PyList_Append(args, param);
            Py_DECREF(param);
        }

    }

    va_end(ap);

    tupargs = PyList_AsTuple(args);

    result = PyObject_CallObject(method, tupargs);

    Py_XDECREF(tupargs);
    Py_XDECREF(args);
    Py_XDECREF(method);

    if(result)
    {
        if(keepreturn)
            return result;
        else
            Py_DECREF(result);
    }
    else
    {
        PyErr_Print();
    }

    return NULL;
}

PyObject *get_Py_None()
{
    Py_INCREF(Py_None);
    return Py_None;
}

void setup_pyclass_scripts()
{
    int vnum, nMatch;
    OBJ_INDEX_DATA *pObjIndex;
    MOB_INDEX_DATA *pMobIndex;

    for ( vnum = 0, nMatch = 0; nMatch < top_obj_index; vnum++ )
    {
        if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
        {
            nMatch++;
            setup_pyclass_obj(pObjIndex);
        }
    }

    for ( vnum = 0, nMatch = 0; nMatch < top_mob_index; vnum++ )
    {
        if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
        {
            nMatch++;
            setup_pyclass_mob(pMobIndex);
        }
    }



}

PyObject *load_module(char *moduleloc, bool reload)
{
    PyObject *module, *dict;
    dict = PyImport_GetModuleDict();
    if(PyDict_GetItemString(dict,moduleloc))
    {	
        PyObject *temp;
        temp = PyImport_AddModule(moduleloc);
        Py_INCREF(temp);
        if(reload)
	{
            module = PyImport_ReloadModule(temp);
            Py_DECREF(temp);
        }
        else
            module = temp;
    }
    else
    {
            module = PyImport_ImportModule(moduleloc);
    }
    if(!module)
    {
        log_f("load_module: load %s failed", moduleloc);
        PyErr_Print();
        return NULL;
    }
    return module;
}

