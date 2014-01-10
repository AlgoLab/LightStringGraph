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
