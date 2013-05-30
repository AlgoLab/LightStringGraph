#! /usr/bin/env python

from optparse import OptionParser

def main():
	parser = OptionParser(
		description = "Script to revert every read in a fasta file.",
		usage = "%prog -i <fasta_file> [-o <output_file>]")
	parser.add_option("-i", metavar = "<fasta_file>", help = "Input fasta file.")
        parser.add_option("-o", metavar = "<output_file", help = "Output fasta file.", default = "reverse.fa")
	(options, args) = parser.parse_args()

	#print options.i
	fasta_file = options.i
        outputfilename = options.o

	if not (fasta_file):
		print "Error: missing argument(s)."
		return

	if not (fasta_file.endswith(".fasta") or fasta_file.endswith(".fa")):
		print "Read file error. Expected .fasta or .fa"
		return

	fin = open(fasta_file)
	out = open(outputfilename, "w")

	try:
                for line in fin.readlines():
                        if ">" in line:
                                out.write(line)
			else:
				out.write(line[0:-1][::-1] + "\n")
	except IOError as e:
		print "Error: {0}".format(e.strerror)

        out.close()
        fin.close()

if __name__ == '__main__':
	main()
