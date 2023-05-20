#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

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

point* readPointsFromFile(const char* filename, int* numPoints, int* dimensions);

void initializeClusters(point* points, int K, cluster* clusters, int isFirst);

void findNearestCluster(int* res, point x, cluster* clusters, int numClusters, int d);

double euclideanDistance(double* p, double* q, int d);

void freePoints(point* points, int numPoints);

void freeClusters(cluster* clusters, int K);

void printPoints(point* points, int num, int d);

void printClusters(cluster* clusters, int num, int d);

void printInvalidInputError(const char* message);

int hasOnlyUnity(int* array, int size);

void updateArrayToUnity(int* array, int size);

void printPoint(point point, int d);

void updateCentroids(cluster* clusters, int K, int d, int* isSame);

int main(int argc, char **argv){
    int K, d, N, indx, iter, isCentroidsSame, i, newSize;
    point* points;
    cluster* clusters;
    point* newPoints;
    
    if ( argc == 3 ){
        iter = 200;
        indx = 2;
    }
    else {
        iter = atoi(argv[2]);
        indx = 3;
    }
    printf( "the argc = %d the file: %s\n",argc, argv[indx]);
    points = readPointsFromFile(argv[indx], &N, &d);
    
    
    K = atoi(argv[1]);
    if (K <= 1 || K >= N) {
        printf("Invalid number of clusters!\n");
        return 1;
    }
    if (iter < 1 || iter > MAX_ITER) {
        printf("Invalid maximum iteration!\n");
        return 1;
    }
    
    clusters = (cluster*)malloc(K * sizeof(cluster));
    if (clusters == NULL) {
        printf("An Error Has Occurred\n");
        return 1;
    }
    
    initializeClusters(points, K, clusters, 1);

    /* Assign every xi to the closest cluster k */
    isCentroidsSame = 0;
    indx = 0;
    while (0 < iter && !isCentroidsSame){
        iter--;
        for(i = K ; i < N ; i++){
            findNearestCluster(&indx ,points[i], clusters, K, d);
            newSize = clusters[indx].size + 1;
            newPoints = (point*)realloc(clusters[indx].points, newSize * sizeof(point));
            if (newPoints == NULL) {
                printf("An Error Has Occurred - mem\n");
                return 1;
            }
            /* Update the cluster's points array */
            clusters[indx].points = newPoints;
            clusters[indx].points[newSize - 1] = points[i];
            clusters[indx].size = newSize;
        }
        
        updateCentroids(clusters, K, d, &isCentroidsSame);
        if (!isCentroidsSame){
            initializeClusters(points, K, clusters, 0);
        }
    }
    if (iter == 0){
        printf("An Error Has Occurred - iter\n");
    }else{
        for (i = 0 ; i < K; i++){
            printPoint(clusters[i].centroid,d);
        }
    }
    freePoints(points, N);
    freeClusters(clusters, K);
    return 0;
}

point* readPointsFromFile(const char* filename, int* numPoints, int* dimensions) {
    FILE* file;
    char line[256];
    int count, pointIndex, coordinateIndex;
    char* token;
    point* points;

    file = fopen(filename, "r");
    if (file == NULL) {
        printInvalidInputError("An Error Has Occurred -file\n");
        return NULL;
    }
    
    *numPoints = 0;
    *dimensions = 0;
    while (fgets(line, sizeof(line), file)) {
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
            fclose(file);
            printInvalidInputError("An Error Has Occurred -bad d\n");
            return NULL;
        }
    }
    
    points = (point*)malloc(*numPoints * sizeof(point));
    if (points == NULL) {
        fclose(file);
        printInvalidInputError("An Error Has Occurred - mem\n");
        return NULL;
    }

    rewind(file);

    pointIndex = 0;
    while (fgets(line, sizeof(line), file)) {
        points[pointIndex].coordinates = (double*)malloc(*dimensions * sizeof(double));
        if (points[pointIndex].coordinates == NULL) {
            fclose(file);
            freePoints(points, pointIndex);
            printInvalidInputError("An Error Has Occurred - mem\n");
            return NULL;
        }

        coordinateIndex = 0;
        token = strtok(line, ",");
        while (token != NULL) {
            points[pointIndex].coordinates[coordinateIndex] = atof(token);
            coordinateIndex++;
            token = strtok(NULL, ",");
        }

        pointIndex++;
    }
    fclose(file);
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

void initializeClusters(point* points, int K, cluster* clusters, int isFirst) {
    int i;
    for (i = 0; i < K; i++) {
        if (!isFirst){
            free(clusters[i].points);
        }
        clusters[i].points = (point*)malloc(clusters[i].size * sizeof(point));
        if (clusters[i].points == NULL) {
            printInvalidInputError("An Error Has Occurred - mem");
            return;
        }
        clusters[i].points[0] = points[i];
        clusters[i].size = 1;
        if(isFirst){
            clusters[i].centroid = points[i];
            
        }
        
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
        printf("%.4f ", point.coordinates[i]);
        if(i < d-1){
            printf(",");
        }
    }
    printf("\n");
}

void updateCentroids(cluster* clusters, int K, int d, int* isSame) {
    int i,j,p;
    double distance, sum;
    point* prevCentroids = (point*)malloc(K * sizeof(point));
    if (prevCentroids == NULL) {
        printf("An Error Has Occurred - mem\n");
        
    }

    
    for (i = 0; i < K; i++) {
        prevCentroids[i].coordinates = (double*)malloc(d * sizeof(double));
        if (prevCentroids[i].coordinates == NULL) {
            printf("An Error Has Occurred - mem\n");
            
        }

        
        for (j = 0; j < d; j++) {
            prevCentroids[i].coordinates[j] = clusters[i].centroid.coordinates[j];
        }
    }

    
    *isSame = 1; 

    for (i = 0; i < K; i++) {
        
        for (j = 0; j < d; j++) {
            sum = 0.0;
            for (p = 0; p < clusters[i].size; p++) {
                sum += clusters[i].points[p].coordinates[j];
            }
            clusters[i].centroid.coordinates[j] = sum / clusters[i].size;
        }

        
        distance = euclideanDistance(clusters[i].centroid.coordinates, prevCentroids[i].coordinates, d);
        if (distance >= EPSILON) {
            *isSame = 0; 
            break;       
        }
    }

    for (i = 0; i < K; i++) {
        free(prevCentroids[i].coordinates);
    }
    free(prevCentroids);
}
