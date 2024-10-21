import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
import numpy as np
import random
import serial
from time import sleep
from time import time
import csv

# Initialize serial port
ser = serial.Serial()
ser.port = "/dev/ttyACM0"  # Arduino serial port
ser.baudrate = 115200
# ser.timeout = 1 #specify timeout when using readline()
ser.open()

if ser.is_open == True:
    print("\nAll right, serial port now open. Configuration:\n")
    print(ser, "\n")

# Create figure for plotting
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
xs = []
y1 = []
y2 = []
y3 = []

# Open CSV file in append mode
csv_file = open("accelerometer_data.csv", "a", newline="")
csv_writer = csv.writer(csv_file)

# Write header if the file is empty
if csv_file.tell() == 0:
    csv_writer.writerow(["Timestamp", "X-axis", "Y-axis", "Z-axis"])


# This function is called periodically from FuncAnimation
def animate(i, xs, y1, y2, y3):
    lines = ser.readlines(400)

    for line in lines:

        line = line.removesuffix(b"\r\n")
        line_as_list = line.split(b" ")

        if len(line_as_list) != 4:
            print("Error: Data not formatted correctly")
            return

        line_as_list = [x.decode("utf-8") for x in line_as_list]
        try:

            x_val = int(line_as_list[0])
            y_val = int(line_as_list[1])
            z_val = int(line_as_list[2])
            timestamp = int(line_as_list[3])

            xs.append(timestamp)
            y1.append(x_val)
            y2.append(y_val)
            y3.append(z_val)

            # Append data to CSV
            csv_writer.writerow([timestamp, x_val, y_val, z_val])
        except:
            print("Error: Data not formatted correctly")
            return

    xs = xs[-1000:]
    y1 = y1[-1000:]
    y2 = y2[-1000:]
    y3 = y3[-1000:]

    ax.clear()
    ax.plot(xs, y1, label="X-axis")
    ax.plot(xs, y2, label="Y-axis")
    ax.plot(xs, y3, label="Z-axis")

    ax.set_ylim(bottom=-16520, top=16520)

    # Format plot
    plt.title("Accelerometer Data")
    plt.legend()
    plt.ylabel("Accelerometer Output")
    plt.xlabel("Time (s)")


# Set up plot to call animate() function periodically
ani = animation.FuncAnimation(fig, animate, fargs=(xs, y1, y2, y3), interval=50)

plt.show()

# Close the CSV file gracefully
csv_file.close()