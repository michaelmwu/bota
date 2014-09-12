
typedef struct {
    PyObject_HEAD
    CHAR_DATA *ch;
} Character;

typedef struct {
    PyObject_HEAD
    OBJ_DATA *obj;
} Object;

typedef struct {
    PyObject_HEAD
    ROOM_INDEX_DATA *room;
} Room;


PyObject *Character_New(PyTypeObject *type, PyObject *args, PyObject *kwds);
PyObject *Object_New(PyTypeObject *type, PyObject *args, PyObject *kwds);
PyObject *Room_New(PyTypeObject *type, PyObject *args, PyObject *kwds);

PyObject *Character_FromChar(CHAR_DATA *ch);
PyObject *Room_FromRoom(ROOM_INDEX_DATA *room);
PyObject *Object_FromObj(OBJ_DATA *obj);

#define KEY_FUNC(_attr, _func) \
 if( !str_cmp(attr,_attr) ) return _func;
 
#define KEY_STRING(_attr, _var) \
 if( !str_cmp(attr,_attr) ) \
 { if(_var) return PyString_FromString(_var); \
   else {Py_INCREF(Py_None); return Py_None;} }

#define KEY_ROOM(_attr, _var) \
 if( !str_cmp(attr,_attr) ) \
 { if(_var) return Room_FromRoom(_var); \
   else {Py_INCREF(Py_None); return Py_None;} }


#define KEY_ISTRING(_attr, _var) \
 if( !str_cmp(attr,_attr) ) \
 { if(_var) return PyString_FromString(_var); \
   else {Py_INCREF(Py_None); return Py_None;} }


#define KEY_INT(_attr, _var) \
 if( !str_cmp(attr,_attr) ) return PyInt_FromLong(_var);

#define KEY_BOOL(_attr, _var) \
 if( !str_cmp(attr,_attr) ) return PyInt_FromLong(_var ? 1 : 0);


extern PyTypeObject CharacterType;
extern PyTypeObject ObjectType;
extern PyTypeObject RoomType;

#define Character_Exact(v)	((v)->ob_type == &CharacterType)
#define Character_Check(v)	(PyObject_TypeCheck((v),&CharacterType))
#define Character_ch(v)		( ((Character*)(v))->ch )

#define Object_Exact(v)	((v)->ob_type == &ObjectType)
#define Object_Check(v)	(PyObject_TypeCheck((v),&ObjectType))
#define Object_obj(v)	( ((Object*)(v))->obj )

#define Room_Exact(v) ((v)->ob_type == &RoomType)
#define Room_Check(v) (PyObject_TypeCheck((v),&RoomType))
#define Room_room(v) ( ((Room*)(v))->room )

PyObject *find_python_class(char *classname, bool reload);

PyObject *Character_send(PyObject *self, PyObject *message);

#define PYNONE_RETURN {Py_INCREF(Py_None); return Py_None;}

#define GENERIC_GET(fname, var, type, dtype, vn) \
static PyObject * type ## _ ## fname ## _get(PyObject *self, void *_m) \
{ dtype *vn;type ## _Require(self); vn = type ## _ ## vn(self); \
return (var);}

#define GENERIC_GET_STRING(fname, var, type, dtype, vn) \
static PyObject * type ## _ ## fname ## _get(PyObject *self, void *_m) \
{ dtype *vn;type ## _Require(self); vn = type ## _ ## vn(self); \
if(var) return PyString_FromString(var); else PYNONE_RETURN;}

#define GENERIC_GET_DSTRING(fname, var, type, dtype, vn) \
static PyObject * type ## _ ## fname ## _get(PyObject *self, void *_m) \
{ dtype *vn; char *t; type ## _Require(self); vn = type ## _ ## vn(self); \
t = var; if(t) return PyString_FromString(t); else PYNONE_RETURN;}

#define GENERIC_GET_INT(fname, var, type, dtype, vn) \
static PyObject * type ## _ ## fname ## _get(PyObject *self, void *_m) \
{ dtype *vn;type ## _Require(self); vn = type ## _ ## vn(self); \
return PyInt_FromLong(var);}

#define GENERIC_GET_BOOL(fname, var, type, dtype, vn) \
static PyObject * type ## _ ## fname ## _get(PyObject *self, void *_m) \
{ dtype *vn;type ## _Require(self); vn = type ## _ ## vn(self); \
return PyInt_FromLong(var ? 1: 0);}


#define OBJECT_GET(fname, var) \
  GENERIC_GET(fname,var,Object,OBJ_DATA,obj)
#define OBJECT_GET_STRING(fname, var) \
  GENERIC_GET_STRING(fname,var,Object,OBJ_DATA,obj)
#define OBJECT_GET_DSTRING(fname, var) \
  GENERIC_GET_DSTRING(fname,var,Object,OBJ_DATA,obj)
#define OBJECT_GET_INT(fname, var) \
  GENERIC_GET_INT(fname,var,Object,OBJ_DATA,obj)
#define OBJECT_GET_BOOL(fname, var) \
  GENERIC_GET_BOOL(fname,var,Object,OBJ_DATA,obj)

#define CHARACTER_GET(fname, var) \
  GENERIC_GET(fname,var,Character,CHAR_DATA,ch)
#define CHARACTER_GET_STRING(fname, var) \
  GENERIC_GET_STRING(fname,var,Character,CHAR_DATA,ch)
#define CHARACTER_GET_INT(fname, var) \
  GENERIC_GET_INT(fname,var,Character,CHAR_DATA,ch)
#define CHARACTER_GET_BOOL(fname, var) \
  GENERIC_GET_BOOL(fname,var,Character,CHAR_DATA,ch)


#define ROOM_GET(fname, var) \
  GENERIC_GET(fname,var,Room,ROOM_INDEX_DATA,room)
#define ROOM_GET_STRING(fname, var) \
  GENERIC_GET_STRING(fname,var,Room,ROOM_INDEX_DATA,room)
#define ROOM_GET_INT(fname, var) \
  GENERIC_GET_INT(fname,var,Room,ROOM_INDEX_DATA,room)
#define ROOM_GET_BOOL(fname, var) \
  GENERIC_GET_BOOL(fname,var,Room,ROOM_INDEX_DATA,room)



