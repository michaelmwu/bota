#define KEY_FUNC(_attr, _func) \
 if( !str_cmp(attr,_attr) ) return _func;
 
#define KEY_STRING(_attr, _var) \
 if( !str_cmp(attr,_attr) ) \
 { if(_var) return PyString_FromString(_var); \
   else {Py_INCREF(Py_None); return Py_None;} }

#define KEY_INT(_attr, _var) \
 if( !str_cmp(attr,_attr) ) return PyInt_FromLong(_var);

