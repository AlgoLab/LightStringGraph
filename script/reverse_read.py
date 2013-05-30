#! /usr/bin/env python

from optparse import OptionParser
from Bio import SeqIO
import sys
import random
import re

def main():
	parser = OptionParser(
		description = "Script to extract a set of reads from fasta sequence(s).",
		usage = "%prog [-N] -i <fasta_file> -l <read_length> -c <coverage>")
	parser.add_option("-i", metavar = "<fasta_file>", help = "Input fasta file.")
	(options, args) = parser.parse_args()

	#print options.i
	fasta_file = options.i

	if not (fasta_file):
		print "Error: missing argument(s)."
		return

	if not (fasta_file.endswith(".fasta") or fasta_file.endswith(".fa")):
		print "Read file error. Expected .fasta or .fa"
		return

	fin = open(fasta_file)
	#file_name = fasta_file.rsplit(".",1)
	out = open(fasta_file + "_rev", "w")

	try:
		for line in fin.readlines():
			if ">" in line:
				out.write(line)
			else:
				out.write(line[0:-1][::-1] + "\n")
		out.close()

	except IOError as e:
		print "Error: {0}".format(e.strerror)

if __name__ == '__main__':
	main()
