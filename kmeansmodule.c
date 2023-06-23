#define PY_SSIZE_T_CLEAN 
#include <Python.h>

static PyObject* mykmeansssp(PyObject *self, PyObject *args)
{
    int k , iter;
    /* This parses the Python arguments into a double (d)  variable named z and int (i) variable named n*/
    if(!PyArg_ParseTuple(args, "i|i", &k, &iter)) {
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */
    }

/* This builds the answer ("d" = Convert a C double to a Python floating point number) back into a python object */
    return Py_BuildValue("d", kmeans_c(z, n)); /*  Py_BuildValue(...) returns a PyObject*  */
}
