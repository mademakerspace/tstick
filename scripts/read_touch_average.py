# The "default values" of the touch sensors can drift, so sometimes we need
# to update the touch_steady array.
# Use the ardiuno serial console to print out the raw values of the strips, and
# then run this on the values to take the averages

import argparse
import statistics

def main(logfile):
    with open(logfile) as fp:
        measurements = fp.read().splitlines()

    newmeasurements = []
    for line in measurements:
        parts = line.split("-")
        parts = parts[:-1]
        parts = [int(p) for p in parts]
        newmeasurements.append(parts)

    averages = []
    # Use the first measurement as a guide, should be the same number on all lines
    for i in range(len(newmeasurements[0])):
        vals = [m[i] for m in newmeasurements]
        averages.append(str(int(statistics.mean(vals))))

    print("{{{}}}".format(", ".join(averages)))



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Take average of t-stick touch ensors")
    parser.add_argument("logfile", help="log file with lines of 1-2-3-4-5")

    args = parser.parse_args()

    main(args.logfile)
