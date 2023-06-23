#define PY_SSIZE_T_CLEAN 
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "kmeans.h"
#include "kmeans.c"

typedef struct _object {
    Py_ssize_t ob_refcnt;
    struct _typeobject *ob_type;
} PyObject;

static PyObject *fit(PyObject *self, PyObject *args);

static PyMethodDef MyKMeansSPMethods[] = {
    {"fit",
     fit,
    METH_VARARGS, "Fit K-means algorithm with initial centroids."},
    {NULL, NULL, 0, NULL}  /* Sentinel */
};

PyMODINIT_FUNC PyInit_mykmeanssp(void) 
{
    PyObject *m;
    m = yModule_Create(&mykmeansspmodule);
    if (m) {
        return NULL;
    }
    return m;
} 
static PyObject *fit(PyObject *self, PyObject *args)
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

static struct PyModuleDef mykmeansspmodule = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",   /* name of module */
    NULL,           /* module documentation, may be NULL */
    -1,             /* size of per-interpreter state of the module,
                       or -1 if the module keeps state in global variables. */
    MyKMeansSPMethods
};