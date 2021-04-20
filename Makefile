SHELL=/usr/bin/env bash
FILENAME=jet-cross-section

-include Makefile.inc

CXX_COMMON:=-I$(PREFIX_INCLUDE) $(CXX_COMMON) $(GZIP_LIB)
CXX_COMMON+= -L$(PREFIX_LIB) -Wl,-rpath,$(PREFIX_LIB) -lpythia8 -ldl
PYTHIA=$(PREFIX_LIB)/libpythia8$(LIB_SUFFIX)

.SECONDEXPANSION:
.PHONY: all 

all:
	$(info Usage: make mainXX)

$(PYTHIA):
	$(error Error: PYTHIA must be built, please run "make"\
                in the top PYTHIA directory)


$(FILENAME).so: $(FILENAME)Dct.cc
	$(CXX) $< -o $@ -w $(CXX_SHARED) $(CXX_COMMON)\
	 $(ROOT_LIB) `$(ROOT_CONFIG) --cflags`

$(FILENAME)Dct.cc: $(FILENAME).h $(FILENAME)LinkDef.h
	$(ROOT_BIN)rootcint -f $@ -I$(PREFIX_INCLUDE) $^

$(FILENAME): $(PYTHIA) $$@.cc
	$(CXX) $@.cc -o $@ $(FASTJET3_INCLUDE) -w $(CXX_COMMON) $(FASTJET3_LIB)\
	 `$(ROOT_CONFIG) --cflags --glibs`
