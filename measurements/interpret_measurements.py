from pathlib import Path
import re
import pandas as pd
import numpy as np
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
def prepare_data(run, containers, df_times, policy_count):
    text = Path(f"./output{run}.txt").read_text()
    # fix the slashes
    text_fixed_slashes = re.sub(" / ", ",", text.strip())
    # remove the last line since it is likely not complete
    data = re.sub(" +", ",", text_fixed_slashes).splitlines()[:-1]
    # fix the header
    data.insert(0, "CONTAINER ID,NAME,CPU %,MEM USAGE (B),MEM LIMIT (B),MEM %,NETWORK IN (kB),NETWORK OUT (kB),"
                   "BLOCK IN (kB),BLOCK OUT (kB),PIDS")
    with open('./temp_csv_output.csv', 'w') as file:
        for line in data:
            file.write(line)
            file.write('\n')
    df = pd.read_csv("./temp_csv_output.csv")

    # fix the data to be integers to be interpretable by matplotlib
    df['MEM USAGE (B)'] = df['MEM USAGE (B)'].apply(lambda x: compute_bytes(x))
    df['MEM LIMIT (B)'] = df['MEM LIMIT (B)'].apply(lambda x: compute_bytes(x))
    df['NETWORK IN (kB)'] = df['NETWORK IN (kB)'].apply(lambda x: compute_bytes(x))
    df['NETWORK OUT (kB)'] = df['NETWORK OUT (kB)'].apply(lambda x: compute_bytes(x))
    df['BLOCK IN (kB)'] = df['BLOCK IN (kB)'].apply(lambda x: compute_bytes(x))
    df['BLOCK OUT (kB)'] = df['BLOCK OUT (kB)'].apply(lambda x: compute_bytes(x))
    df['MEM %'] = df['MEM %'].apply(lambda x: float(x.strip('%')))
    df['CPU %'] = df['CPU %'].apply(lambda x: float(x.strip('%')))
    stp_df = df[df['NAME'] == containers[0]]
    ds_df = df[df['NAME'] == containers[1]]
    first_change = min(stp_df[stp_df['NETWORK OUT (kB)'] != stp_df['NETWORK OUT (kB)'].iloc[0]].first_valid_index(),
                       ds_df[ds_df['NETWORK OUT (kB)'] != ds_df['NETWORK OUT (kB)'].iloc[0]].first_valid_index(),
                       stp_df[stp_df['NETWORK IN (kB)'] != stp_df['NETWORK IN (kB)'].iloc[0]].first_valid_index(),
                       ds_df[ds_df['NETWORK IN (kB)'] != ds_df['NETWORK IN (kB)'].iloc[0]].first_valid_index())
    last_change = min(stp_df[stp_df['NETWORK OUT (kB)'] != stp_df['NETWORK OUT (kB)'].iloc[-1]].last_valid_index(),
                      ds_df[ds_df['NETWORK OUT (kB)'] != ds_df['NETWORK OUT (kB)'].iloc[-1]].last_valid_index(),
                      stp_df[stp_df['NETWORK IN (kB)'] != stp_df['NETWORK IN (kB)'].iloc[-1]].last_valid_index(),
                      ds_df[ds_df['NETWORK IN (kB)'] != ds_df['NETWORK IN (kB)'].iloc[-1]].last_valid_index())
    total_time = df_times[f'times{run_nr}'].sum()
    times = np.linspace(0, total_time, last_change - first_change)
    before_times = (times[:first_change] * -1)[::-1]
    after_values = np.add(times[:len(df.index) - last_change], np.full(len(df.index) - last_change, times[-1],
                                                                       dtype=float))
    all_times = np.concatenate((np.concatenate((before_times, times)), after_values))
    df['time'] = all_times
    network_in = df[df['NAME'] == 'sfe_drone']['NETWORK IN (kB)'].loc[last_change] - df[
        df['NAME'] == containers[1]]['NETWORK IN (kB)'].loc[first_change]
    network_out = df[df['NAME'] == 'sfe_drone']['NETWORK OUT (kB)'].loc[last_change] - df[
        df['NAME'] == containers[1]]['NETWORK OUT (kB)'].loc[first_change]
    print(f"Total evaluation time: {total_time} sec and on average per policy {total_time/policy_count} sec\r\n"
          f"Average memory use: {df[df['NAME'] == containers[1]]['MEM %'].loc[first_change:last_change].mean()}%\r\n"
          f"Average CPU use: {df[df['NAME'] == containers[1]]['CPU %'].loc[first_change:last_change].mean()}%\r\n"
          f"Total network in: {network_in}kB and average package size {network_in / policy_count}kB\r\n"
          f"Total network out: {network_out}kB and average package size {network_out / policy_count}kB")
    return df


# plot the network usage data
def network_usage(df, axes, containers):
    stp_df = df[df['NAME'] == containers[0]]
    ds_df = df[df['NAME'] == containers[1]]
    axes[0].plot('time', 'NETWORK IN (kB)', data=stp_df, label="STP input")
    axes[0].plot('time', 'NETWORK OUT (kB)', data=stp_df, label="STP output")
    axes[0].plot('time', 'NETWORK IN (kB)', data=ds_df, label="Drone input")
    axes[0].plot('time', 'NETWORK OUT (kB)', data=ds_df, label="Drone output")
    axes[0].set_ylabel("Bandwidth usage (kB)")
    axes[0].set_title("Bandwidth usage over time")
    axes[0].legend()


# plot the cpu and memory usage data
def cpu_memory_usage(df, axes, containers):
    stp_df = df[df['NAME'] == containers[0]]
    ds_df = df[df['NAME'] == containers[1]]
    axes[1].plot('time', 'MEM %', data=stp_df, label="STP memory usage")
    axes[1].plot('time', 'CPU %', data=stp_df, label="STP CPU usage")
    axes[1].plot('time', 'MEM %', data=ds_df, label="Drone memory usage")
    axes[1].plot('time', 'CPU %', data=ds_df, label="Drone CPU usage")
    axes[1].set_ylabel("Memory/CPU usage (% of total capacity)")
    axes[1].set_title("Resource usage over time")
    axes[1].legend()


# measure the time output by C++
output_run = ["3"]
nr_of_policies = 2500
# names of the docker containers, STP followed by DS
container_names = ["sfe_laptop", "sfe_drone"]
times_df = pd.read_csv("./times.csv")
# run the plot functions
for run_nr in output_run:
    print(f"Output of run {run_nr}")
    dataframe = prepare_data(run_nr, container_names, times_df, nr_of_policies)
    figs, ax = plt.subplots(2, 1, sharex='col', figsize=(10, 5))
    ax[1].set_xlabel("Time (seconds)")
    network_usage(dataframe, ax, container_names)
    cpu_memory_usage(dataframe, ax, container_names)
    print("\r\n")
    plt.show()
