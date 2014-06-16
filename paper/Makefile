TARGETS=EM-string-graph.pdf
LATEXMK = latexmk -recorder -use-make -pdf

pdf: $(TARGETS)

bibs=$(wildcard *.bib)
sources=$(wildcard figures/*.ipe)
pdfs=$(sources:.ipe=.pdf)
burstpdfs=$(sources:.ipe=-1.pdf)
svgfigs=$(wildcard figures/*.svg)
svgfigspdf=$(svgfigs:.svg=.pdf)

figs: $(pdfs) $(burstpdfs) $(svgfigspdf)

%-1.pdf : %.pdf
	                pdftk $< burst output $(<:.pdf=-%d.pdf)

%.pdf : %.ipe
	                ipetoipe -pdf $<

%.pdf : %.svg
	        inkscape $< --export-pdf=$@

clean :
	        rm -f ./*.pdf ./figures/*.pdf && latexmk -c

%.pdf : %.tex $(bibs) figs
	$(LATEXMK) $<

####
# Reproduce WABI 2014 experiment
####
SHELL=/bin/bash

fafile = pp.90_fqhar_70_ov.filter.pass.fa
fafile = pp.250k.filter.pass.fa
fa = input-data/reads/$(fafile)
lsg_fafile = output/$(fafile)
bin = progrs

experiment: lsg.log sga.log


sga/$(fafile): $(fa)
	ln -s $< $@


$(fa): $(fafile)-xaa.gz $(fafile)-xal.gz
	zcat $(fafile)-xa? > $(fa)

$(fafile)-xaa.gz $(fafile)-xal.gz: input-data/reads/1053074.zip
	cd input-data/reads && unzip $<

input-data/reads/1053074.zip:
	test -d input-data/reads/ && mkdir -p input-data/reads/
	test -d output && mkdir -p output
	test -d sga && mkdir -p sga
	curl http://downloads.figshare.com/article/public/1053074 -o $@

.SECONDARY: input-data/reads/1053074.zip $(fafile)-xaa.gz $(fafile)-xal.gz $(fa)

$(lsg_fafile).sa: input-data/reads/$(fafile)
	/usr/bin/time -o beetl-time.log $(bin)/beetl-bwt --verbosity quiet -i $< -a ext --generate-lcp -o $(lsg_fafile)  --output-format ASCII

lsg.log: lsg1.log lsg2.log lsg3.log lsg4.log
	cat $^ > $@

lsg1.log: $(lsg_fafile).sa
	ulimit -v 12582912 && ulimit -f unlimited && ulimit -n 102400 && /usr/bin/time -ao $@ $(bin)/lsg -B $(lsg_fafile) -T 65 -C 26 &> $@

lsg2.log: $(lsg_fafile).sa lsg1.log
	ulimit -v 12582912 && ulimit -f unlimited && ulimit -n 102400 && /usr/bin/time -ao $@ $(bin)/redbuild -b $(lsg_fafile) -m 26 -g 1000000 &> $@

lsg3.log: $(lsg_fafile).sa lsg2.log
	ulimit -v 12582912 && ulimit -f unlimited && ulimit -n 102400 && /usr/bin/time -ao $@ $(bin)/sgbuild -b $(lsg_fafile) -m 26 -l 90 -r $< -c -x &> $@ | gzip -c > $(lsg_fafile).outlsg.exhaustive.asqg.gz

lsg4.log: $(lsg_fafile).sa lsg3.log
	ulimit -v 12582912 && ulimit -f unlimited && ulimit -n 102400 && /usr/bin/time -ao $@ $(bin)/graph2asqg -b $(lsg_fafile) -l 90 -r $(lsg_fafile) 2> $@ > $(lsg_fafile).filter.pass.fa.outlsg.reduced.asqg

sga.log : sga/$(fafile)
	/usr/bin/time -ao $@ $(bin)/sga-1way index -a ropebwt $< &> $@ && /usr/bin/time -ao $@ $(bin)/sga-1way overlap -m 65 $< &>> $@
