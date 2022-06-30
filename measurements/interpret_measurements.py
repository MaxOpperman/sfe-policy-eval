from pathlib import Path
import re
import pandas as pd
import matplotlib.pyplot as plt


# compute the byte value of kibibyte, mebibyte, and gibibyte to bytes
def compute_bytes(to_compute):
    if to_compute[-3:] == "KiB":
        return float(to_compute[:-3]) * 0.0009765625
    elif to_compute[-3:] == "MiB":
        return float(to_compute[:-3])
    elif to_compute[-3:] == "GiB":
        return float(to_compute[:-3]) * 1024
    # also do the conversion for kilo, mega, and giga bytes
    elif to_compute[-2:] == "kB":
        return float(to_compute[:-2])
    elif to_compute[-2:] == "MB":
        return float(to_compute[:-2]) * 1024
    elif to_compute[-2:] == "GB":
        return float(to_compute[:-2]) * 1048576
    return 0


# convert the output to be workable data
text = Path("output_test.txt").read_text()
text_fixed_slashes = re.sub(" / ", ",", text.strip())  # fix the slashes
data = re.sub(" +", ",", text_fixed_slashes).splitlines()[:-1]  # remove the last line since it is likely not complete
# fix the header
data[0] = "CONTAINER ID,NAME,CPU %,MEM USAGE (B),MEM LIMIT (B),MEM %,NETWORK IN (kB),NETWORK OUT (kB)," \
          "BLOCK IN (kB),BLOCK OUT (kB),PIDS"
with open('csv_output.csv', 'w') as file:
    for line in data:
        file.write(line)
        file.write('\n')
df = pd.read_csv("csv_output.csv")

# fix the data
df['MEM USAGE (B)'] = df['MEM USAGE (B)'].apply(lambda x: compute_bytes(x))
df['MEM LIMIT (B)'] = df['MEM LIMIT (B)'].apply(lambda x: compute_bytes(x))
df['NETWORK IN (kB)'] = df['NETWORK IN (kB)'].apply(lambda x: compute_bytes(x))
df['NETWORK OUT (kB)'] = df['NETWORK OUT (kB)'].apply(lambda x: compute_bytes(x))
df['BLOCK IN (kB)'] = df['BLOCK IN (kB)'].apply(lambda x: compute_bytes(x))
df['BLOCK OUT (kB)'] = df['BLOCK OUT (kB)'].apply(lambda x: compute_bytes(x))
df['MEM %'] = df['MEM %'].apply(lambda x: float(x.strip('%')))
df['CPU %'] = df['CPU %'].apply(lambda x: float(x.strip('%')))


def network_usage():
    plt.plot('NETWORK IN (kB)', data=df[df['NAME'] == 'sfe_drone'], label="Drone input")
    plt.plot('NETWORK OUT (kB)', data=df[df['NAME'] == 'sfe_drone'], label="Drone output")
    plt.plot('NETWORK IN (kB)', data=df[df['NAME'] == 'sfe_laptop'], label="STP input")
    plt.plot('NETWORK OUT (kB)', data=df[df['NAME'] == 'sfe_laptop'], label="STP output")
    plt.xlabel("Time (seconds)")
    plt.ylabel("Bandwidth usage (kB)")
    plt.title("Bandwidth usage over time")
    plt.legend()
    plt.show()


def cpu_memory_usage():
    plt.plot('MEM %', data=df[df['NAME'] == 'sfe_drone'], label="Drone memory usage")
    plt.plot('CPU %', data=df[df['NAME'] == 'sfe_drone'], label="Drone CPU usage")
    plt.plot('MEM %', data=df[df['NAME'] == 'sfe_laptop'], label="STP memory usage")
    plt.plot('CPU %', data=df[df['NAME'] == 'sfe_laptop'], label="STP CPU usage")
    plt.xlabel("Time (seconds)")
    plt.ylabel("Memory/CPU usage (% of total capacity)")
    plt.title("Resource usage over time")
    plt.legend()
    plt.show()


network_usage()
cpu_memory_usage()
