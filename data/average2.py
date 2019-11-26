#!/usr/bin/env python3

graph = ""
single = ["wea", "not"]
double = ["smax", "smin", "wmax", "wmin", "po", "do", "fa"]
ops = single + double

res = ['P', 'D', 'I']
first = ['P'] * 23 + ['D'] * 23 + ['I'] * 23
second = ['-'] * 2 + res * 7 + ['-'] * 2 + res * 7 + ['-'] * 2 + res * 7

labels = single + double * 3 + single + double * 3 + single + double * 3

results = {'wea': [], 'not': [], 'smax': [], 'smin': [], 'wmax': [], 'wmin': [], 'po': [], 'do': [], 'fa': []} 

with open("output-online.txt") as fh:
    outputs = fh.readlines()
    cur_time = 0
    cur_bandwidth = 0

    i = 0;
    for output in outputs:
        line = output.split("===")
        data = line[0].split(",")
        targets = int(data[0][1:])
        operations = int(data[1])
        query_size = int(data[2])
        smoothness = int(data[3][:-1])
        time = float(line[2][1:-1])
        bandwidth = int(line[3][1:-2])

        cur_time += time
        cur_bandwidth += bandwidth

        if smoothness == 50:
            avg_time = round(cur_time / 50, 6)
            avg_bandwidth = round(cur_bandwidth / 50)
            results[labels[i]].append("%s, %s, %s, %f, %d" % (labels[i], first[i], second[i], avg_time, avg_bandwidth))

            cur_time = 0
            cur_bandwidth = 0
            i += 1

for o in ops:
    for r in results[o]:
        print(r)
