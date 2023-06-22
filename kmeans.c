#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#define MAX_ITER 1000
#define EPSILON 0.001

typedef struct {
    double *coordinates;
} point;

typedef struct {
    point *points;
    point centroid;
    int size;
} cluster;

point* readPointsFromFile(int* numPoints, int* dimensions);

void initializeClusters(point* points, int K, cluster* clusters, int d);

void findNearestCluster(int* res, point x, cluster* clusters, int numClusters, int d);

double euclideanDistance(double* p, double* q, int d);

void freePoints(point* points, int numPoints);

void freeClusters(cluster* clusters, int K);

void printInvalidInputError(const char* message);

void printPoint(point point, int d);

void updateCentroids(cluster* clusters, int K, int d, int* isSame);

void oneIter(point* points, int N, cluster* clusters, int K, int d, int* isSame);

int isNumber(const char* str);

int main(int argc, char **argv){
    int K, d, N, iter, isCentroidsSame, i;
    point* points;
    cluster* clusters;
    
    if ( argc < 3 ){
        iter = 200;
    }
    else {
        if (!isNumber(argv[2])) {
            printf("Invalid maximum iteration!\n");
            return 1;
        }
        iter = atoi(argv[2]);
    }

    points = readPointsFromFile(&N, &d);
    if (!isNumber(argv[1])) {
        freePoints(points, N);
        printf("Invalid number of clusters!\n");
        return 1;
    }
    else{
        K = atoi(argv[1]);
    }

    if (K <= 1 || K >= N) {
        freePoints(points, N);
        printf("Invalid number of clusters!\n");
        return 1;
    }
    if (iter < 1 || iter >= MAX_ITER) {
        freePoints(points, N);
        printf("Invalid maximum iteration!");
        return 1;
    }
    
    clusters = (cluster*)malloc(K * sizeof(cluster));
    if (clusters == NULL) {
        freePoints(points, N);
        printf("An Error Has Occurred\n");
        return 1;
    }
    
    initializeClusters(points, K, clusters, d);
    
    /* Assign every xi to the closest cluster k */
    isCentroidsSame = 0;
    while (0 < iter && !isCentroidsSame){
        iter--;
        oneIter(points, N, clusters, K, d, &isCentroidsSame);
    }
    for (i = 0 ; i < K; i++){
        printPoint(clusters[i].centroid,d);
        printf("\n");
    }
    for (i = 0 ; i < K; i++){
        free(clusters[i].centroid.coordinates);
    }
    free(clusters);
    freePoints(points, N);
    return 0;
}

point* readPointsFromFile(int* numPoints, int* dimensions) {
    char line[256];
    int count, pointIndex, coordinateIndex;
    char* token;
    point* points;
    *numPoints = 0;
    *dimensions = 0;
    while (fgets(line, sizeof(line), stdin)) {
        (*numPoints)++;
        count = 0;
        token = strtok(line, ",");
        while (token != NULL) {
            count++;
            token = strtok(NULL, ",");
        }
        if (*dimensions == 0) {
            *dimensions = count;
        } else if (*dimensions != count) {
            printInvalidInputError("An Error Has Occurred\n");
            return NULL;
        }
    }
    
    points = (point*)malloc(*numPoints * sizeof(point));
    if (points == NULL) {
        printInvalidInputError("An Error Has Occurred\n");
        return NULL;
    }

    rewind(stdin);

    pointIndex = 0;
    while (fgets(line, sizeof(line), stdin)) {
        points[pointIndex].coordinates = (double*)malloc(*dimensions * sizeof(double));
        if (points[pointIndex].coordinates == NULL) {
            freePoints(points, pointIndex);
            printInvalidInputError("An Error Has Occurred\n");
            return NULL;
        }

        coordinateIndex = 0;
        token = strtok(line, ",");
        while (token != NULL) {
            points[pointIndex].coordinates[coordinateIndex] = atof(token);
            coordinateIndex++;
            token = strtok(NULL, ",");
        }
        ++pointIndex;
    }
    return points;
}

void freeClusters(cluster* clusters, int K){
    int i;
    if (clusters == NULL) {
        return;
    }
    for (i = 0; i < K; i++) {
        free(clusters[i].points);
    }

    free(clusters);
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

void initializeClusters(point* points, int K, cluster* clusters, int d){
    int i,j;
    for (i = 0; i < K; i++) {
        clusters[i].centroid.coordinates = (double*)malloc(d * sizeof(double));
        if (clusters[i].centroid.coordinates == NULL) {
            printInvalidInputError("An Error Has Occurred");
            return;
        }
        for (j = 0 ; j < d ; j++){
            clusters[i].centroid.coordinates[j] = points[i].coordinates[j];
        }
        clusters[i].size = 0;
        }
}

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

void updateCentroids(cluster* clusters, int K, int d, int* isSame) {
    int i,j,p;
    double distance, sum;
    point* prevCentroids;
    prevCentroids = (point*)malloc(K * sizeof(point));
    if (prevCentroids == NULL) {
        printf("An Error Has Occurred\n");
    }
    
    for (i = 0; i < K; i++) {
        prevCentroids[i].coordinates = (double*)malloc(d * sizeof(double));
        if (prevCentroids[i].coordinates == NULL) {
            printf("An Error Has Occurred\n");
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
        if (distance >= EPSILON) {
            (*isSame) = 0;
            break;
        }
    }

    for (i = 0; i < K; i++) {
        free(prevCentroids[i].coordinates);
    }
    free(prevCentroids);
}

void oneIter(point* points, int N, cluster* clusters, int K, int d, int* isSame){
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

    updateCentroids(clusters, K, d,  &(*isSame));

    for (i = 0; i < K; i++) {
        free(clusters[i].points);
        clusters[i].size = 0;
    }
}
int isNumber(const char* str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }

    while (*str != '\0') {
        if (!isdigit(*str)) {
            return 0;
        }
        ++str;
    }

    return 1;
}
