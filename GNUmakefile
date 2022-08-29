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
