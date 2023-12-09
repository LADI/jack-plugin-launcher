.PHONY: default
default:
	@echo "This is a project that uses the WAF build system."
	@echo ""
	@echo "A typical command sequence for configuring,"
	@echo "building and installing the software is:"
	@echo ""
	@echo "  ./waf configure --prefix=${HOME}/opt/"
	@echo "  ./waf"
	@echo "  ./waf install"
	@echo ""
	@echo "To get more info about waf configure parameters, run:"
	@echo ""
	@echo "  ./waf configure --help"
	@echo ""
	@echo "Read the README.rst file for more info."
	@echo ""

TOP := $(shell pwd)
EPREFIX := $(TOP)/build/destdir

.PHONY: cibuild
cibuild:
	./waf configure --prefix=$(EPREFIX) && ./waf install && $(EPREFIX)/bin/jpl

doxdoc:
	mkdir -vp build
	doxygen doc/Doxyfile
	cp doc/doxygen-awesome-css/doxygen-awesome-darkmode-toggle.js build/doxout/html/

.PHONY: cdbus_regenerate
cdbus_regenerate: appman_defs.c

# force regenerate (so cdbus-codegen changes apply)
.PHONY: appman_defs.c

appman_defs.c: org.ladish.applicationmanager.xml
	cat org.ladish.applicationmanager.xml | cdbus-codegen jpl_appman > appman_defs.c
