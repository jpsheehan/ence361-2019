import matplotlib.pyplot as plt
import re
import numpy as np
import scipy as sp
import scipy.interpolate
import matplotlib.pyplot as plt
from scipy.interpolate import splrep, splev


x = []
y = []

with open('yawStep.txt','r') as datafile:
        stripped = [line.strip() for line in datafile.readlines()]
        lines = [line for line in stripped if line]
        i = 0
        for line in lines:
                line = line.split()
                line[1] = re.sub("\D","", line[1])
                print(line)
                i += 1/50
                x.append(i)
                y.append(int(line[1]))
                
                
tck = splrep(x, y)
xnew = np.linspace(x[0], x[-1])
ynew = splev(xnew, tck)                

plt.plot(xnew,15*np.ones(len(ynew)), label = 'Target reference')        
plt.plot(xnew,ynew, label='Average step response from all rigs')
plt.xlabel('Time (seconds)')
plt.ylabel('Yaw (degrees)')
plt.title('Yaw step response')
plt.legend()
plt.show()