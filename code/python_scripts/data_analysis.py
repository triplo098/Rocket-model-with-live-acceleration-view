from matplotlib import pyplot as plt
from time import time
import csv
import numpy as np


file_name = "../../flight_data/accelerometer_data.csv"


# Open CSV file for reading
with open(file_name, mode="r") as csv_file:
    csv_reader = csv.reader(csv_file)
    header = next(csv_reader)

    # Read data from CSV file
    timestamps = []
    x_axis = []
    y_axis = []
    z_axis = []

    for row in csv_reader:
        timestamps.append(int(row[0]))
        x_axis.append(int(row[1]))
        y_axis.append(int(row[2]))
        z_axis.append(int(row[3]))

    # Create figure for plotting
    fig = plt.figure()
    ax = fig.add_subplot(1, 1, 1)

    ax.plot(timestamps, x_axis, label="X-axis")
    ax.plot(timestamps, y_axis, label="Y-axis")
    ax.plot(timestamps, z_axis, label="Z-axis")

    plt.xlabel("Timestamp")
    plt.ylabel("Acceleration [mg]")
    plt.title("Accelerometer Data")

    plt.legend()
    plt.show()
