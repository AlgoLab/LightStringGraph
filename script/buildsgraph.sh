#! /usr/bin/env sh

# buildsgraph.sh <readsfile>

tempdir="tdir"
joinedreadsfile="$tempdir/joinedreads.fa"
uniquereads="$tempdir/uniquereads.fa"
readoutfile="$tempdir/reads.fa"
rreadoutfile="$tempdir/rev_reads.fa"
bwtpo="$tempdir/BWT" # BWT prefix
rbwtpo="$tempdir/revBWT" # revBWT prefix
oldGSA="$tempdir/revBWT.pairSA" # revBWT GSA
lenfile="$tempdir/lengths"

logfile="$tempdir/execlog.log"

if [ $# -eq 2 -a -f $1 ]; then
    if [ ! -d $tempdir ]; then
	mkdir $tempdir
    fi

    outputfile=$2

    touch $logfile
    echo " " > $logfile
    
    date 1>&2
    echo "Calling awk on reads.." 1>&2
    awk '/>/{if (x)print x;print;x="";next}{x=(!x)?$0:x$0;}END{print x;}' $1 > $joinedreadsfile

    date 1>&2
    echo "Filtering only unique reads.." 1>&2
    cat $1 | grep -v '^>' | sort -u > $uniquereads
    nr=`wc -l $uniquereads | cut -d ' ' -f 1`
    echo "Number of unique reads : $nr." 1>&2

    if [ -f $readoutfile ]; then
    	rm $readoutfile
    fi
    if [ -f $rreadoutfile ]; then
    	rm $rreadoutfile
    fi

    readnum=0

    date 1>&2
    echo "Reconstructing reads.. " 1>&2
    cat $uniquereads | while read LINE; do
    	echo "> read-$readnum" >> $readoutfile
    	readnum=$((readnum+1))
    	echo $LINE >> $readoutfile
    done

    date 1>&2
    echo "Generating reverse reads for file $readoutfile. Output stored to $rreadoutfile." 1>&2
    ./script/reverse_read.py -i $readoutfile -o $rreadoutfile

    date 1>&2
    echo "Building bwt for file $readoutfile.." 1>&2
    beetl bwt -i $readoutfile -o $bwtpo -f ascii --intermediate-format ascii >> execlog.log 2>> execlog.log

    date 1>&2
    echo "Building bwt for file $rreadoutfile.." 1>&2
    beetl bwt -i $rreadoutfile -o $rbwtpo -f ascii --intermediate-format ascii >> execlog.log 2>> execlog.log

    date 1>&2
    echo "Building new GSA from $oldGSA.." 1>&2
    ./bin/bgsa -i $oldGSA -o $tempdir/GSA.sa >> execlog.log 2>> execlog.log

    date 1>&2
    echo "Generating string graph.." 1>&2
    ./bin/stringGraph -B $bwtpo-B0 -R $rbwtpo-B0 -G $tempdir/GSA.sa > $outputfile 2>> execlog.log

    date 1>&2
    echo "Removing temporary files.." 1>&2
    rm .QINT*
    rm .revQINT*

    date 1>&2
    echo "Getting lengths from $outputfile.." 1>&2
    cat $outputfile | grep length | cut -d ' ' -f 3 > $lenfile
    echo -n "Edge lengths sum is " 1>&2
    awk '{ sum += $1 } END { print sum }' $lenfile

    date 1>&2
    echo "Assembling sequence(s) to seq.fa.." 1>&2
    ./script/reconstruct_seq.py $outputfile $readoutfile seq2.fa 1>&2

    echo "Folding seq.fa..." 1>&2
    fold -w 50 seq2.fa > seq.fa

    rm seq2.fa

    # #rm -rf $tempdir
else
    echo "Usage: buildsgraph.sh <readsfile> <outputfile>"
    exit 0
fi
