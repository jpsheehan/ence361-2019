import sys
import argparse
import numpy as np

import matplotlib.pyplot as plt
from matplotlib.font_manager import FontProperties


def extract_column_i(data, length, x, t=None):
    cols = np.zeros(length)
    for key in data:
        if t is None or key == t:
            for i, datum in enumerate(data[key]):
                col = datum[x]
                cols[i] += col
    return cols


def extract_utilization(data, length, t=None):
    return extract_column_i(data, length, 3, t)


def extract_time_error(data, length, t=None):
    return extract_column_i(data, length, 4, t)


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

                    # calculate the time error (in micros and %)
                    time_error = 0
                    time_error_us = 0
                    if frequency != 0:
                        time_error_us = abs(1.0 / frequency * 1000000 - period)
                        if period != 0:
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
    # CPU Utilization
    total_utilization = extract_utilization(data, n)

    # make the ylimit be the nearest 10%
    cpu_util_ymax = round(max(total_utilization) / 10.0) * 10
    plt.subplot(2, 1, 1)
    plt.title("Kernel Task Information")
    plt.ylabel("CPU Time Utilization (%)")
    plt.xlim((0, n))
    plt.ylim((0, cpu_util_ymax))

    plt.plot(range(n), total_utilization)
    for key in data:
        plt.plot(range(n), extract_utilization(data, n, key))

    # time error graph
    total_time_error = extract_time_error(data, n)
    time_error_ymax = min(50, round(max(total_time_error) / 5.0) * 5)
    plt.subplot(2, 1, 2)
    plt.xlabel("Time (s)")
    plt.ylabel("Time Error (%)")
    plt.xlim((0, n))
    plt.ylim((0, time_error_ymax))
    plt.plot(range(n), total_time_error)
    for key in data:
        plt.plot(range(n), extract_time_error(data, n, key))
    plt.legend(labels=["Total"] + pretty_keys,
               loc="upper center", fontsize="small", ncol=6)

    plt.show()


# call main
if __name__ == '__main__':
    main()
