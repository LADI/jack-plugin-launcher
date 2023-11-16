.PHONY: all
all: doc

.PHONY: doc
doc: build/README.html
doc: doxdoc

doxdoc:
	mkdir -vp build
	doxygen doc/Doxyfile
	cp doc/doxygen-awesome-css/doxygen-awesome-darkmode-toggle.js build/doxout/html/

build/README.html: README.adoc doc/README-docinfo.html doc/README-docinfo-header.html
	mkdir -vp build
	asciidoc -b html5 -a data-uri -a icons --theme ladi -o build/README.html README.adoc
