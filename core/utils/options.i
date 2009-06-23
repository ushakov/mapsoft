%typemap(in) Options {
  if(!PyMapping_Check($input)) {
    PyErr_SetString(PyExc_ValueError,"Expected a mapping");
    return NULL;
  }
  PyObject *items = PyDict_Items($input);
  for (int i = 0; i < PyList_Size(items); ++i) {
    PyObject *it = PyList_GetItem(items, i);
    PyObject *key = PyTuple_GetItem(it, 0);
    PyObject *val = PyTuple_GetItem(it, 1);
    PyObject *strkey = PyObject_Str(key);
    PyObject *strval = PyObject_Str(val);
    $1.put(PyString_AsString(strkey), PyString_AsString(strval));
    Py_DECREF(strkey);
    Py_DECREF(strval);
  }
  Py_DECREF(items);
}

%typemap(in) const Options& (Options t) {
  if(!PyMapping_Check($input)) {
    PyErr_SetString(PyExc_ValueError,"Expected a mapping");
    return NULL;
  }
  $1 = &t;
  PyObject *items = PyDict_Items($input);
  for (int i = 0; i < PyList_Size(items); ++i) {
    PyObject *it = PyList_GetItem(items, i);
    PyObject *key = PyTuple_GetItem(it, 0);
    PyObject *val = PyTuple_GetItem(it, 1);
    PyObject *strkey = PyObject_Str(key);
    PyObject *strval = PyObject_Str(val);
    $1->put(PyString_AsString(strkey), PyString_AsString(strval));
    Py_DECREF(strkey);
    Py_DECREF(strval);
  }
  Py_DECREF(items);
}

%typemap(typecheck) Options {
  $1 = PyMapping_Check($input) ? 1 : 0;
}

%typemap(typecheck) const Options& {
  $1 = PyMapping_Check($input) ? 1 : 0;
}

%typemap(out) Options {
  PyObject *dict = PyDict_New();
  for (Options::const_iterator it = $1.begin(); it != $1.end(); ++it) {
    PyObject *val = PyString_FromString(it->second.c_str());
    PyDict_SetItemString(dict, it->first.c_str(), val);
    Py_DECREF(val);
  }
  $result = dict;
}
