# Standalone make file for building with ROOT.  To use it type
#
#   make -f Makefile.standalone
#
# There are also clean and veryclean targets
#
#   make -f Makefile.standalone veryclean
# 


# ***************************************************
MACHINE = $(shell uname -s)
LINUX   = Linux
MAC     = Darwin

BASEDIR  = .

ROOT=-lTreePlayer `root-config --cflags --libs`
ROOTINC=`root-config --cflags`
DEBUG=-g -Wall -ggdb -O3

INCDIR=$(PWD) 
LIBS=$(ROOT) #-lXMLParser

INCFLAGS=$(DEBUG) $(ROOTINC) -I$(INCDIR)

all: bin/main

#bin/dict.cxx:sars_cov2_sk/linkdef.h
#	@echo "**"
#	@echo "** Creating Root dictionary"
#	@echo "**"
#	rootcint -f bin/dict.cxx -c sars_cov2_sk/linkdef.h


bin/Person.o:src/Person.cxx sars_cov2_sk/Person.h
	@echo "**"
	@echo "** Compiling C++ Source: Person.cxx Person.h"
	@echo "**"
	g++ $(DEBUG) $(INCFLAGS) -g -c -o $@ $< $(ROOTINC)

bin/Household.o:src/Household.cxx sars_cov2_sk/Household.h
	@echo "**"
	@echo "** Compiling C++ Source: Household.cxx Household.h"
	@echo "**"
	g++ $(DEBUG) $(INCFLAGS) -g -c -o $@ $< $(ROOTINC)

bin/main.o:src/main.cxx
	@echo "**"
	@echo "** Compiling C++ Source"
	@echo "**"
	g++ $(DEBUG) $(INCFLAGS) -g -c -o $@ $< $(ROOTINC)

bin/main:bin/main.o bin/Person.o bin/Household.o
	@echo "**"
	@echo "** Linking"
	@echo "**"
	g++ $(DEBUG) -o $@ bin/main.o bin/Person.o bin/Household.o $(LIBS)  

clean:
	rm -rf bin/*.o main

