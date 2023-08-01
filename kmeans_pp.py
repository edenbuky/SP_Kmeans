import numpy as np
import sys
import pandas as pd
import mykmeanssp as kp

iter = 300
np.random.seed(0)



def make_probs(dists):
    sum_D = sum(dists)
    probs = [dists[i]/sum_D for i in range(len(dists))]
    return probs


def kmeans_pp_iter(arr, centroids, indecies, it, dists=None):
    if it == 0:
        choose_idx = np.random.choice(indecies)
    else:
        probs = make_probs(dists)
        choose_idx = np.random.choice(indecies, p=probs)

    centroids.append(choose_idx)
    point = arr[choose_idx, :]

    def distance(p2, p1=point):
        s = 0
        for i in range(len(p1)):
            tmp = p1[i] - p2[i]
            s += (tmp ** 2)
        return s ** 0.5

    D_x = np.apply_along_axis(distance, axis=1, arr=arr)
    dists = np.minimum(dists, D_x)  # update D(x) for each point
    return centroids, dists



def main():
    global iter
    k = 0
    epsilon = 0.0
    try:
        if len(sys.argv) > 6 or len(sys.argv) < 5:
            raise Exception
        
        try:
            k = int(sys.argv[1])
        except Exception as e:
            print("Invalid number of clusters!")
            exit(1)
        if len(sys.argv) == 6:
            try:
                iter = int(sys.argv[2])
                if iter != float(sys.argv[2]):
                    raise Exception
            except Exception as e:
                print("Invalid maximum iteration!")
                exit(1)
            
        
        file_name_2 = sys.argv[-1]
        file_name_1 = sys.argv[-2]
        epsilon = float(sys.argv[-3])

        df1 = pd.read_csv(file_name_1, sep=",", header = None)
        df2 = pd.read_csv(file_name_2, sep=",", header = None)

        merged_df = pd.merge(df1, df2, left_on=df1.columns[0], right_on=df2.columns[0], how='inner')

        if len(merged_df) <= k or k <= 1 or float(sys.argv[1]) != k:
            print("Invalid number of clusters!")
            exit(1)

        if not (1 < iter < 1000):
            print("Invalid maximum iteration!")
            exit(1)

    except Exception as e:
        print("An Error Has Occurred")
        exit(1)

    sorted_df = merged_df.sort_values(by=df1.columns[0])
    array = sorted_df.values[:, 1:].astype(float)
    centroids = []
    indecies = [i for i in range(array.shape[0])]
    dists = [float("inf") for i in range(array.shape[0])]
    it = 0
    while it < k:
        centroids, dists = kmeans_pp_iter(array, centroids, indecies, it, dists)
        it += 1
    print(*centroids, sep = ",")
    centroid_to_pass = [array[i, :].tolist() for i in centroids]
    points = array.tolist()
    d = len(points[0])
    
    #fitting with the C module
    ans = kp.fit(centroid_to_pass, points,k, iter, d, epsilon)
    for centroid in ans:
        print(*centroid, sep = ",")
    #print()







if __name__ == '__main__':
    main()

