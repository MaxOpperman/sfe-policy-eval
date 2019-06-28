#!/usr/bin/env python3

import sys
import time


fho = open('data/output-online.txt', 'a+')
fht = open('data/output-total.txt', 'a+')

total_output = ""
online_output = ""
count = 0
start = time.time()
for line in sys.stdin:
    if line[0] == "[":
        online_output = line[:-1] + online_output
        total_output = line[:-1] + total_output
        print(online_output)
        fho.write(online_output + "\n")
        fho.flush()
        fht.write(total_output + "\n")
        fht.flush()
        online_output = ""
        total_output = ""
        count = count + 1
    elif line[:8] == "Online =":
        words = line.split("\t")
        parts = words[1].split(" ")
        online_output += "[" + str(round(float(parts[0])/1000, 6)) + "]"
    elif line[:7] == "Total =":
        words = line.split("\t")
        parts = words[2].split(" ")
        total_output += "[" + str(round(float(parts[0])/1000, 6)) + "]"
    elif line[:11] == "Online Sent":
        words = line.split("\t")
        parts = words[1].split(" ")
        total_output += "===[" + parts[0] + "]"
        online_output += "===[" + parts[0] + "]"

fho.close()
fht.close()
end = time.time()
difference = end - start

print("Done, evaluated %d policies in %f seconds" % (count, difference))
