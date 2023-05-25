from matplotlib import pyplot as plt
import numpy as np
import time

FILENAME = "dummyData.txt"
DATANAMES = ["time(s)", "v1", "v2", "p1", "p2"]
VTARGETLINE = 20

#intake and parse data
dataFile = open(FILENAME, 'r')
data = dataFile.read()
dataPoints = data.split("\n")

values = []
for dataPoint in dataPoints:
    line = (dataPoint.split("$"))

    lineArray = []
    for segment in line:
        lineArray.append(float(segment))

    values.append(lineArray)


values = np.array(values)
values = values.transpose()

#create control line
controlLine = []
counter = 0
while(counter < len(values[0])):
    controlLine.append(VTARGETLINE)
    counter += 1


#plot data
figure, axes = plt.subplots()
axes.plot(values[0], values[1], label=DATANAMES[1])
axes.plot(values[0], values[2], label=DATANAMES[2])
axes.plot(values[0], controlLine, label="Control")

plt.show()
