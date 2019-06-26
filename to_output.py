#!/usr/bin/env python3

import sys
import time

outputs = []
output = ""
start = time.time()
for line in sys.stdin:
    if line[0] == "[":
        output = line[:-1] + output
        print(output)
        outputs.append(output)
        output = ""
    elif line[:8] == "Online =":
        words = line.split("\t")
        parts = words[1].split(" ")
        output += "[" + str(round(float(parts[0])/1000, 6)) + "]"
    elif line[:11] == "Online Sent":
        words = line.split("\t")
        parts = words[1].split(" ")
        output += "===[" + parts[0] + "]"

result = "\n".join(outputs)

with open('data/output.txt', 'w+') as fh:
    fh.write(result)

end = time.time()
difference = end - start

print("Done, evaluated %d policies in %f seconds" % (len(outputs), difference))
