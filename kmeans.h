
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