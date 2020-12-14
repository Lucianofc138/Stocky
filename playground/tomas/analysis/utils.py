import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.image as mpimg


def plot_distance_method(method, dataset, xlim, ylim):
    distance = dataset[dataset["method"] == method]
    groups = pd.unique(dataset["group_num"])
    colors = []
    for i in groups:
        colors.append(np.random.rand(3,))
    color = []
    for index, value in distance["group_num"].iteritems():
        color.append(colors[value])

    fig, ax = plt.subplots()
    fig.set_figwidth(15)
    fig.set_figheight(10)
    ax.scatter(distance["n_bins"], distance["distance"],
        c=color, s=50)
    dist_title = method + " Distance"
    ax.set_title(dist_title)
    ax.set_xlabel("Number of bins")
    ax.set_ylabel("Distance")
    ax.axis
    plt.xlim(xlim)
    plt.ylim(ylim)

def plot_diff_distance(method, dataset, diff_dataset, xlim, ylim):
    distance = dataset[dataset["method"] == method]
    diff_distance = diff_dataset[diff_dataset["method"] == method]

    fig, ax = plt.subplots()
    fig.set_figwidth(15)
    fig.set_figheight(10)
    ax.scatter(distance["n_bins"], distance["distance"],
        color='red', s=50)
    ax.scatter(diff_distance["n_bins"], diff_distance["distance"],
        color='blue', s=50)
    dist_title = method + " Distance"
    ax.set_title(dist_title)
    ax.set_xlabel("Number of bins")
    ax.set_ylabel("Distance")
    ax.axis
    plt.xlim(xlim)
    plt.ylim(ylim)