#!/usr/bin/env python3

graph = ""
with open("output-online.txt") as fh:
    outputs = fh.readlines()
    averages = []
    average = 0
    for output in outputs:
        line = output.split("===")
        data = line[0].split(",")
        targets = int(data[0][1:])
        operations = int(data[1])
        smoothness = int(data[2][:-1])
        if(operations == targets):
            time = float(line[2][1:-1])
            bandwidth = int(line[3][1:-2])
            if smoothness < 10:
                average += bandwidth
            else:
                average += bandwidth
                average = round(average / 10, 0)
                print("(%d, %f)" % (targets, average))
                graph += "(%d, %d) " % (targets, average)
                average = 0

print(graph)

