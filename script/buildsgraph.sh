#! /usr/bin/env sh

# buildsgraph.sh <readsfile>

tempdir=".tdir"

if [ $# -eq 1 -a -f $1 ]; then
    if [ ! -f $tempdir ]; then
	mkdir $tempdir
    fi
    touch execlog.log
    echo " " > execlog.log

    cat $1 | grep -v '^>' | sort -u > $tempdir/uniquereads.fa
    nr=`wc -l $tempdir/uniquereads.fa | cut -d ' ' -f 1`
    echo "Number of unique reads : $nr." 1>&2

    readoutfile="$tempdir/reads.fa"
    rreadoutfile="$tempdir/rev_reads.fa"

    if [ -f $readoutfile ]; then
	rm $readoutfile
    fi
    if [ -f $rreadoutfile ]; then
	rm $rreadoutfile
    fi

    readnum=0

    cat $tempdir/uniquereads.fa | while read LINE; do
	echo "> read-$readnum" >> $readoutfile
	readnum=$((readnum+1))
	echo $LINE >> $readoutfile
    done

    echo "Generating reverse reads for file $readoutfile. Output stored to $rreadoutfile." 1>&2
    ./script/reverse_read.py -i $readoutfile -o $rreadoutfile

    bwtpo="$tempdir/BWT" # BWT prefix
    rbwtpo="$tempdir/revBWT" # revBWT prefix
    oldGSA="$tempdir/revBWT.pairSA" # revBWT GSA

    echo "Building bwt for file $readoutfile.." 1>&2
    beetl bwt -i $readoutfile -o $bwtpo -f ascii >> execlog.log 2>> execlog.log
    echo "Building bwt for file $rreadoutfile.." 1>&2
    beetl bwt -i $rreadoutfile -o $rbwtpo -f ascii >> execlog.log 2>> execlog.log

    echo "Building new GSA from $oldGSA.." 1>&2
    ./bin/bgsa -i $oldGSA -o $tempdir/GSA.sa >> execlog.log 2>> execlog.log

    echo "Generating string graph.." 1>&2
    ./bin/stringGraph -B $bwtpo-B0 -R $rbwtpo-B0 -G $tempdir/GSA.sa 2>> execlog.log
    #rm -rf $tempdir
else
    echo "Usage: buildsgraph.sh <readsfile>"
    exit 0
fi
