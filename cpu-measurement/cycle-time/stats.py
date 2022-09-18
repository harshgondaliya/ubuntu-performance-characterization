#!/usr/bin/env python3
import sys
import math

def main():
    fname = sys.argv[1]
    READ_OVERHEAD = float(sys.argv[2])
    SLEEP_DURATION = int(sys.argv[3])

    f = open(fname, "r")
    data = []
    lines = f.readlines()
    for line in lines:
        x = line.rstrip('\n')
        x_minus_overhead = float(x) - READ_OVERHEAD
        data.append(x_minus_overhead)
    data.sort()
    
    x = fname.split(".")
    stats_fname = x[0] + "-stats.txt"
    sf = open(stats_fname, "w")
    
    mean = sum(data)/len(data)
    sum_diff = 0.0
    for x in data:
        sum_diff += ((x - mean)**2)
        sf.write(str(x) + "\n")
    
    stddev = math.sqrt(sum_diff/len(data))
    mean_cycle_time = float(1000000000*SLEEP_DURATION/mean)
    print("Generated file " + stats_fname)
    print("Mean: " + str(mean))
    print("Stddev: " + str(stddev))
    print("Mean Cycle Time: " + str(mean_cycle_time))
    
    print("Mean (rounded): " + str(round(mean,1)))
    print("Stddev (rounded): " + str(round(stddev,1)))
    print("Mean Cycle Time (rounded): " + str(round(mean_cycle_time,3)))
    
    sf.write("Mean: " + str(mean) + "\n")
    sf.write("Stddev: " + str(stddev) + "\n")
    sf.write("Mean Cycle Time: " + str(mean_cycle_time) + "\n")
    
    sf.write("Mean (rounded): " + str(round(mean,1)) + "\n")
    sf.write("Stddev (rounded): " + str(round(stddev,1)) + "\n")
    sf.write("Mean Cycle Time (rounded): " + str(round(mean_cycle_time,3)) + "\n")
    
    sf.close()
    f.close()
if __name__ == "__main__":
    main()
