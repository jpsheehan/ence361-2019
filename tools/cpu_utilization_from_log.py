"""
ldr.py

Display analog data from Arduino using Python (matplotlib)

Author: Mahesh Venkitachalam
Website: electronut.in
"""

import sys
import argparse
import numpy as np

import matplotlib.pyplot as plt


def extract_utilization(data, length, t=None):
    u = np.zeros(length)

    for key in data:
        if t is None or key == t:
            for i, datum in enumerate(data[key]):
                utilization = datum[3]
                u[i] += utilization
    return u


def main():
    # create parser
    parser = argparse.ArgumentParser(description="LDR serial")
    # add expected arguments
    parser.add_argument('--file', dest='file', required=True)

    # parse args
    args = parser.parse_args()

    filename = args.file

    print('reading from filename %s...' % filename)

    file = open(filename)

    data = {}

    ignore_uart_kernel = True

    # crunch the data
    for line in file.readlines():
        if "altitude" in line:
            valid_rows = line.strip().split()
            raw_data = [row.split(',') for row in valid_rows]
            if len(raw_data) * 4 == sum([len(data) for data in raw_data]):
                for (name, duration, period, frequency) in [(name, int(duration), int(period), int(frequency)) for (
                        name, duration, period, frequency) in raw_data]:

                    # ignore the kernel uart task if specified
                    if name == "uart_kernel_data" and ignore_uart_kernel:
                        continue

                    # add an array when the key doesn't exist
                    if name not in data:
                        data[name] = []

                    # calculate the % time utilization
                    utilization = duration * frequency / 10000.0

                    # calculate the time error (in micros)
                    time_error_us = 0
                    if frequency != 0:
                        time_error_us = abs(1.0 / frequency * 1000000 - period)

                    # calculate time error (as a % of period)
                    time_error = time_error_us / period * 100.0

                    data[name].append(
                        (duration, period, frequency, utilization, time_error))

    file.close()

    # get the number of data points
    n = len(data["uart_flight_data"])

    # replace underscores with spaces, capitalize each word
    pretty_keys = [' '.join([x.capitalize() for x in s.split('_')])
                   for s in data.keys()]

    # plot the data
    fig = plt.figure()

    # CPU Utilization
    total_utilization = extract_utilization(data, n)

    # make the ylimit be the nearest 10%
    cpu_util_ymax = round(max(total_utilization) / 10.0) * 10
    cpu_util = plt.axes(xlim=(0, n), ylim=(0, cpu_util_ymax),
                        title="Kernel Task Time", xlabel="Time (s)", ylabel="CPU Time Utilization (%)")
    cpu_util.plot(range(n), total_utilization)

    for key in data:
        cpu_util.plot(range(n), extract_utilization(data, n, key))
    cpu_util.legend(labels=["Total"] + pretty_keys, loc="upper left")

    plt.show()


# call main
if __name__ == '__main__':
    main()
