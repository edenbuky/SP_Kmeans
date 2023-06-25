#define MAX_ITER 1000
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
