"""
ldr.py

Display analog data from Arduino using Python (matplotlib)

Author: Mahesh Venkitachalam
Website: electronut.in
"""

import sys
import argparse
import numpy as np
from time import sleep
from collections import deque

import matplotlib.pyplot as plt
import matplotlib.animation as animation


# plot class
class AnalogPlot:
    # constr
    def __init__(self, filename, maxLen):
        # open file
        self.file = open(filename, 'r')
        self.a = deque([0.0]*maxLen)
        self.all = {}
        self.has_made_pi_chart = False
        self.maxLen = maxLen

    # add to buffer
    def addToBuf(self, buf, val):
        if len(buf) < self.maxLen:
            buf.append(val)
        else:
            buf.pop()
            buf.appendleft(val)

    # add data
    def add(self, data):
        total_utilization = 0
        for i in range(len(data)):

            utilization = 0
            if data[i][3] != 0:
                # duration * frequency / 1000000.0 * 100
                utilization = data[i][1] * data[i][3] / 10000.0
            if data[i][0] != "uart_kernel_data":
                total_utilization += utilization

            if data[i][0] not in self.all:
                self.all[data[i][0]] = []
            self.all[data[i][0]].append(utilization)
        self.addToBuf(self.a, total_utilization)
        print(total_utilization)

    def make_pi_chart(self):
        averages = {}
        for key in self.all:
            averages[key] = sum(self.all[key]) / len(self.all[key])
            print("{}: {}%".format(key, averages[key]))

    def update(self, frameNum, a):
        try:
            line = self.file.readline()
            if line == "" and self.has_made_pi_chart == False:
                self.make_pi_chart()
                self.has_made_pi_chart = True

            elif "altitude" in line:
                valid_rows = line.strip().split()[:-1]
                raw_data = [row.split(',') for row in valid_rows]
                if len(raw_data) * 4 == sum([len(data) for data in raw_data]):
                    parsed_data = [(name, int(duration), int(period), int(frequency)) for (
                        name, duration, period, frequency) in raw_data]
                    self.add(parsed_data)
                    for _i in range(len(parsed_data)):
                        a.set_data(range(self.maxLen), self.a)
        except KeyboardInterrupt:
            print('exiting')

        return a

    # clean up
    def close(self):
        # close file
        self.file.close()

# main() function


def main():
    # create parser
    parser = argparse.ArgumentParser(description="LDR serial")
    # add expected arguments
    parser.add_argument('--file', dest='file', required=True)

    # parse args
    args = parser.parse_args()

    filename = args.file

    print('reading from filename %s...' % filename)

    # plot parameters
    analogPlot = AnalogPlot(filename, 200)

    print('plotting data...')

    # set up animation
    fig = plt.figure()
    ax = plt.axes(xlim=(0, 200), ylim=(0, 20))
    a = ax.plot([], [])
    plt.title = "Kernel Task Efficiency"
    plt.xlabel("- Time (s)")
    plt.ylabel("CPU Utilization (%)")
    anim = animation.FuncAnimation(fig, analogPlot.update,
                                   fargs=a,
                                   interval=50)

    # show plot
    plt.show()

    # clean up
    analogPlot.close()

    print('exiting.')


# call main
if __name__ == '__main__':
    main()
