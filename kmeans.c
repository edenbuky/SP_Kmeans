#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    double *coordinates;
} point;

typedef struct {
    point *points;
    point centroid;
    int size;
} cluster;

void findNearestCluster(int* res, point x, cluster* clusters, int numClusters, int d);

double euclideanDistance(double* p, double* q, int d);

void freePoints(point* points, int numPoints);

void printInvalidInputError(const char* message);

void printPoint(point point, int d);

void updateCentroids(cluster* clusters, int K, int d, int* isSame, double epsilon);

void oneIter(point* points, int N, cluster* clusters, int K, int d, int* isSame, double eps);

double euclideanDistance(double* p, double* q, int d) {
    double diff, sum = 0.0;
    int i;
    for (i = 0; i < d; i++) {
        diff = p[i] - q[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

void findNearestCluster(int* res, point x, cluster* clusters, int K, int d) {
    double minDistance, distance;
    int i, result;
    result = 0;
    minDistance = euclideanDistance(x.coordinates,clusters[0].centroid.coordinates, d);
    for (i = 1; i < K; i++) {
        distance = euclideanDistance(x.coordinates,clusters[i].centroid.coordinates, d);
        if (distance < minDistance) {
            minDistance = distance;
            result = i;
        }
    }
    (*res) = result;
    
}

void printInvalidInputError(const char* message) {
    printf("%s\n", message);
    exit(1);
}

void printPoint(point point, int d) {
    int i;
    for (i = 0; i < d; i++) {
        printf("%.4f", point.coordinates[i]);
        if(i < d-1){
            printf(",");
        }
    }
}

void updateCentroids(cluster* clusters, int K, int d, int* isSame, double epsilon) {
    int i,j,p;
    double distance, sum;
    point* prevCentroids;
    prevCentroids = (point*)malloc(K * sizeof(point));
    if (prevCentroids == NULL) {
        printInvalidInputError("An Error Has Occurred\n");
    }
    
    for (i = 0; i < K; i++) {
        prevCentroids[i].coordinates = (double*)malloc(d * sizeof(double));
        if (prevCentroids[i].coordinates == NULL) {
            printInvalidInputError("An Error Has Occurred\n");
        }
        for (j = 0; j < d; j++) {
            prevCentroids[i].coordinates[j] = clusters[i].centroid.coordinates[j];
        }
    }

    
    (*isSame) = 1;

    for (i = 0; i < K; i++) {
        
        for (j = 0; j < d; j++) {
            sum = 0.0;
            for (p = 0; p < clusters[i].size; p++){
                sum += clusters[i].points[p].coordinates[j];
            }
            clusters[i].centroid.coordinates[j] = sum / clusters[i].size;
        }
    }
    
    for (i = 0; i < K; i++) {
        distance = euclideanDistance(clusters[i].centroid.coordinates, prevCentroids[i].coordinates, d);
        if (distance >= epsilon) {
            (*isSame) = 0;
            break;
        }
    }

    for (i = 0; i < K; i++) {
        free(prevCentroids[i].coordinates);
    }
    free(prevCentroids);
}

void oneIter(point* points, int N, cluster* clusters, int K, int d, int* isSame, double eps){
    int i , indx, newSize;
    point* newPoints;
    indx = 0;
    for(i = 0 ; i < N ; i++){
        findNearestCluster(&indx ,points[i], clusters, K, d);
        if( clusters[indx].size == 0){
            clusters[indx].points = (point*)malloc(sizeof(point));
            if (clusters[indx].points == NULL) {
                printInvalidInputError("An Error Has Occurred\n");
            }
            clusters[indx].points[0] = points[i];
            clusters[indx].size = 1;
        }
        else{
            newSize = clusters[indx].size + 1;
            newPoints = (point*)realloc(clusters[indx].points, newSize * sizeof(point));
            if (newPoints == NULL) {
                printInvalidInputError("An Error Has Occurred\n");
            }
            /* Update the cluster's points array */
            clusters[indx].points = newPoints;
            clusters[indx].points[newSize - 1] = points[i];
            clusters[indx].size = newSize;
        }
    }

    updateCentroids(clusters, K, d,  &(*isSame), eps);

    for (i = 0; i < K; i++) {
        free(clusters[i].points);
        clusters[i].size = 0;
    }
}
void freePoints(point* points, int numPoints) {
    int i;
    if (points == NULL) {
        return;
    }
    for (i = 0; i < numPoints; i++) {
        if (points[i].coordinates != NULL){
            free(points[i].coordinates);
        }
    }

    free(points);
}

void printClusters(cluster* clusters, int n, int d) {
    int i;
    int j;
    for (i = 0; i < n; ++i) {
        printf("Cluster %d:\n", i);
        printf("Centroid: (");
        for (j = 0; j < d; ++j) {
            printf("%f", clusters[i].centroid.coordinates[j]);
            if (j != d - 1) printf(", ");
        }
        printf(")\n\n");
        }
}
void printPoints(point* points, int n, int d) {
    int i;
    int j;
    for (i = 0; i < n; ++i) {
        printf("Point %d: (", i);
        for (j = 0; j < d; ++j) {
            printf("%f", points[i].coordinates[j]);
            if (j != d - 1) printf(", ");
        }
        printf(")\n");
        }
}

