import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from sklearn.cluster import KMeans
from sklearn import datasets
import numpy as np

def calc_inertia(data, k):
    kmeans = KMeans(init = 'k-means++', n_clusters=k, random_state=0, n_init=1).fit_transform(data)
    mux = np.min(kmeans, axis=1)
    mux = mux ** 2
    return sum(mux)

def make_elbow(x, y):
    fig, ax = plt.subplots(figsize=(10, 10))
    line, = ax.plot(x, y, lw=2)
    ax.annotate('Elbow point 1', xy=(3, y[2]), xytext=(3, 200), arrowprops=dict(arrowstyle="->"))
    ax.annotate('Elbow point 2', xy=(2, y[1]), xytext=(2, 40), arrowprops=dict(arrowstyle="->"))
    ax.plot((3), (y[2]), 'o', color='y')
    ax.plot((2), (y[1]), 'o', color='y')
    ax.set_title('Elbow methos for Kmeans: inertia VS k', color = 'navy')
    ax.set(xlabel='K', ylabel='inertia')
    return fig,ax

def make_axis(data):
    x = [i for i in range(1,11)]
    y = [calc_inertia(data, k) for k in x]
    return x,y

def main():
    iris = datasets.load_iris()
    x, y = make_axis(iris['data'])
    fig, axs = make_elbow(x,y)
    plt.savefig('elbow.png')





if __name__ == '__main__':
    main()
