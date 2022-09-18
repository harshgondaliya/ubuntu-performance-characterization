#!/usr/bin/env python3
import sys
import math

def main():
    fname = sys.argv[1]
    percent_mean = float(int(sys.argv[2])/100)
    READ_OVERHEAD = float(sys.argv[3])
    LOOP_OVERHEAD = float(sys.argv[4])
    NO_OF_ITERATIONS = int(sys.argv[5])

    f = open(fname, "r")
    data = []
    lines = f.readlines()
    for line in lines:
        x = line.rstrip('\n')
        x_minus_overhead = ((float(x) - READ_OVERHEAD - (LOOP_OVERHEAD*NO_OF_ITERATIONS)))/NO_OF_ITERATIONS
        data.append(x_minus_overhead)
    data.sort()
    
    trim_count = int(percent_mean * len(data))
    trimmed_data = [i for i in data[trim_count:len(data)-trim_count]]
    
    x = fname.split(".")
    trimmed_data_fname = x[0] + "-trimmed.txt"
    tfname = open(trimmed_data_fname, "w")
    
    trimmed_mean = sum(trimmed_data)/len(trimmed_data)
    sum_diff = 0.0
    for x in trimmed_data:
        sum_diff += ((x - trimmed_mean)**2)
        tfname.write(str(x) + "\n")
    
    trimmed_stddev = math.sqrt(sum_diff/len(trimmed_data))
    print("Generated file " + trimmed_data_fname)
    print("trimmed mean: " + str(trimmed_mean))
    print("trimmed stddev: " + str(trimmed_stddev))
    print("trimmed mean (in ns): " + str(trimmed_mean * 0.344))
    print("trimmed stddev (in ns): " + str(trimmed_stddev * 0.344))
    print("trimmed mean (rounded): " + str(round(trimmed_mean,1)))
    print("trimmed stddev (rounded): " + str(round(trimmed_stddev,1)))
    tfname.write("trimmed mean: " + str(trimmed_mean) + "\n")
    tfname.write("trimmed stddev: " + str(trimmed_stddev) + "\n")
    tfname.write("trimmed mean (in ns): " + str(trimmed_mean * 0.344) + "\n")
    tfname.write("trimmed stddev (in ns): " + str(trimmed_stddev * 0.344) + "\n")
    tfname.write("trimmed mean (rounded): " + str(round(trimmed_mean,1)) + "\n")
    tfname.write("trimmed stddev (rounded): " + str(round(trimmed_stddev,1)) + "\n")
    tfname.close()
    f.close()
if __name__ == "__main__":
    main()
