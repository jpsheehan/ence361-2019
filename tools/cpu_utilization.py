"""
ldr.py

Display analog data from Arduino using Python (matplotlib)

Author: Mahesh Venkitachalam
Website: electronut.in
"""

import sys, serial, argparse
import numpy as np
from time import sleep
from collections import deque

import matplotlib.pyplot as plt 
import matplotlib.animation as animation

    
# plot class
class AnalogPlot:
	# constr
	def __init__(self, strPort, maxLen):
		# open serial port
		self.ser = serial.Serial(strPort, 9600)
		self.a = deque([0.0]*maxLen)
          
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
		if len(data) == 11:
			total_utilization = 0
			for i in range(len(data)):
				utilization = 0
				if data[i][3] != 0:
					# duration * frequency / 1000000.0 * 100
					utilization = data[i][1] * data[i][3] / 10000.0
				if data[i][0] != "uart_kernel_data":
					total_utilization += utilization
			self.addToBuf(self.a, total_utilization)
			print(total_utilization )

  # update plot
	def update(self, frameNum, a):
		try:
			line = self.ser.readline()
			if "altitude" in line:
				valid_rows = line.split('\t')[:-1]
				raw_data = [row.split(',') for row in valid_rows]
				if len(raw_data) * 4 == sum([len(data) for data in raw_data]):
					parsed_data = [(name, int(duration), int(period), int(frequency)) for (name, duration, period, frequency) in raw_data]
					self.add(parsed_data)
					for i in range(len(parsed_data)):
						a.set_data(range(self.maxLen), self.a)
		except KeyboardInterrupt:
			print('exiting')
      
		return a

	# clean up
	def close(self):
		# close serial
		self.ser.flush()
		self.ser.close()    

# main() function
def main():
	# create parser
	parser = argparse.ArgumentParser(description="LDR serial")
	# add expected arguments
	parser.add_argument('--port', dest='port', required=True)

	# parse args
	args = parser.parse_args()
	  
	#strPort = '/dev/tty.usbserial-A7006Yqh'
	strPort = args.port

	print('reading from serial port %s...' % strPort)

	# plot parameters
	analogPlot = AnalogPlot(strPort, 100)

	print('plotting data...')

	# set up animation
	fig = plt.figure()
	ax = plt.axes(xlim=(0, 100), ylim=(0, 20))
	a = ax.plot([], [])
	plt.title = "Kernel Task Efficiency"
	plt.xlabel("Time (s)")
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
