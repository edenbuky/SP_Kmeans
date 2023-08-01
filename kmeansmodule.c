#define PY_SSIZE_T_CLEAN 
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "kmeans.c"

static PyObject *fit(PyObject *self, PyObject *args);


point* convertPyListToPoints(PyObject *pyList, int n, int d) {
    point* points = (point*)malloc(n * sizeof(point));

    /*Iterate over the outer list*/
    for (int i = 0; i < n; i++) {
        PyObject *innerList = PyList_GetItem(pyList, i);

        /*Allocate memory for the coordinates of the point*/
        points[i].coordinates = (double*)malloc(d * sizeof(double));

        /*Iterate over the inner list*/
        for (int j = 0; j < d; j++) {
            PyObject *coordObj = PyList_GetItem(innerList, j);
            points[i].coordinates[j] = PyFloat_AsDouble(coordObj);
        }
    }

    return points;
}

static PyObject* transferCentersToPyList(cluster* clusters, int k, int d)
{
	int i,j;
	double rounded;
    PyObject* pyFloat;
    PyObject* pyPoints; 
	

	pyPoints = PyList_New(k);
		for (i = 0; i < k; i++)
	{
		PyObject* point = PyList_New(d);
		for (j = 0; j < d; j++)
		{	
		    rounded = round(clusters[i].centroid.coordinates[j] * 10000.0) / 10000.0; 
			pyFloat = Py_BuildValue("f", rounded); 
        	PyList_SetItem(point, j, pyFloat);
		}
		PyList_SetItem(pyPoints, i, point);

	}
    return pyPoints;
}

PyObject* kmeans_c(PyObject *PyCentroids, PyObject *PyPoints, int K, int iter, int d, double eps){
    int N, isCentroidsSame, i,j;
    cluster* clusters;
    point* points;
    point* centroids;
    N = PyObject_Length(PyPoints);
    PyObject* pyCentersList;


    clusters = (cluster*)malloc(K * sizeof(cluster));
    if (clusters == NULL) {
        printInvalidInputError("An Error Has Occurred\n");
    }
    /*Converting a Python lists to an array of points in C*/
    centroids = convertPyListToPoints(PyCentroids, K, d);
    
    points = convertPyListToPoints(PyPoints, N, d);

    /*initialize Clusters*/
    for (i = 0; i < K; i++) {
        clusters[i].centroid.coordinates = (double*)malloc(d * sizeof(double));
        if (clusters[i].centroid.coordinates == NULL) {
            printInvalidInputError("An Error Has Occurred");
        }
        for (j = 0 ; j < d ; j++){
            clusters[i].centroid.coordinates[j] = centroids[i].coordinates[j];
        }
        clusters[i].size = 0;
        }

    /* Assign every xi to the closest cluster k */
    isCentroidsSame = 0;
    while (0 < iter && !isCentroidsSame){
        iter--;
        oneIter(points, N, clusters, K, d, &isCentroidsSame, eps);
    }
    /*Transfers cluster centers to a Python list*/
    /*point* cent = clusters->centroid;*/
    pyCentersList = transferCentersToPyList(clusters, K, d);

    
    for (i = 0 ; i < K; i++){
        free(clusters[i].centroid.coordinates);
    }
    free(clusters);
    freePoints(points, N);
    freePoints(centroids,K);
    return pyCentersList;
}



static PyObject* fit(PyObject *self, PyObject *args)
{
    PyObject *centroids, *points;
    int k, iter, d;
    double eps;
    /* This parses the Python arguments into a double (d)  variable named z and int (i) variable named n*/
    if(!PyArg_ParseTuple(args, "OOiiid",&centroids, &points, &k, &iter, &d, &eps)) {
        
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */

    }

/* This builds the answer ("d" = Convert a C double to a Python floating point number) back into a python object */
    return Py_BuildValue("O", kmeans_c(centroids, points,k, iter, d, eps)); /*  Py_BuildValue(...) returns a PyObject*  */
}

static PyMethodDef MyKMeansSPMethods[] = {
    {"fit",
     (PyCFunction) fit,
    METH_VARARGS,
    PyDoc_STR("Fit K-means algorithm with initial centroids.")},
    {NULL, NULL, 0, NULL}  /* Sentinel */
};

static struct PyModuleDef mykmeansspmodule = {
        PyModuleDef_HEAD_INIT,
        "mykmeanssp",
        NULL,
        -1,
        MyKMeansSPMethods
};

PyMODINIT_FUNC PyInit_mykmeanssp(void) 
{
    PyObject *m;
    m = PyModule_Create(&mykmeansspmodule);
    if (!m) {
        return NULL;
    }
    return m;
} 

