#!/usr/bin/env python3
import sys
import math

def main():
    fname = sys.argv[1]
    percent_mean = float(int(sys.argv[2])/100)
    READ_OVERHEAD = float(sys.argv[3])
    DATA_SIZE = int(sys.argv[4])
    f = open(fname, "r")
    data = []
    lines = f.readlines()
    for line in lines:
        x = line.rstrip('\n')
        x_minus_overhead = (DATA_SIZE*8*1000)/((float(x) - READ_OVERHEAD )*0.344)
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
    print("trimmed mean (Mbps): " + str(trimmed_mean))
    print("trimmed stddev (Mbps): " + str(trimmed_stddev))
    tfname.write("trimmed mean (Mbps): " + str(trimmed_mean) + "\n")
    tfname.write("trimmed stddev(Mbps): " + str(trimmed_stddev) + "\n")
    tfname.close()
    f.close()
if __name__ == "__main__":
    main()
