#!/usr/bin/env python3

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
        if((targets == 1 and (operations == 1 or operations % 5 == 0)) or (targets % 5 == 0 and operations % 5 == 0)):
            time = float(line[2][1:-1])
            bandwidth = int(line[3][1:-2])
            if smoothness < 10:
                average += bandwidth
            else:
                average += bandwidth
                average = round(average / 10, 0)
                print("[%d, %d]: %d" % (targets, operations, average))
                average = 0


