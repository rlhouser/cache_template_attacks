import random
import numpy
from matplotlib import pyplot
import os

#Plot the histogram of CPU cycles for cache hits and misses
if __name__ == '__main__': 
	infile = 'histogram.csv'
	if not os.path.isfile(infile):
		print(infile +' does not exist')
		exit(0)
	hits = []
	misses = []

	with open(infile) as reader:
		for line in reader:
			l = line.strip().split('\t')
			if len(l) < 2:
				continue
			hits += [int(l[0])]*int(l[1])
			misses += [int(l[0])]*int(l[2])

	bins = numpy.linspace(min(hits), max(misses), 100)
	pyplot.hist(hits, bins, alpha=0.5, label='hits')
	pyplot.hist(misses, bins, alpha=0.5, label='misses')
	pyplot.legend(loc='upper right')
	pyplot.title('flush+reload profiling')
	pyplot.xlabel('CPU Cycles')
	pyplot.ylabel('Probe Counts')
	pyplot.tight_layout()
	pyplot.savefig('histogram.png')
	pyplot.close()