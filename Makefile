MACHINE = $(shell uname -s)
LINUX   = Linux
MAC     = Darwin

BASEDIR  = .

DEBUG=-g -Wall -ggdb -O3 -std=c++11

INCDIR=$(PWD) 

INCFLAGS=$(DEBUG) -I$(INCDIR)

all: bin/main

bin/Person.o:src/Person.cxx sars_cov2_sk/Person.h
	@echo "**"
	@echo "** Compiling C++ Source: Person.cxx Person.h"
	@echo "**"
	g++ $(DEBUG) $(INCFLAGS) -g -c -o $@  $<

bin/Household.o:src/Household.cxx sars_cov2_sk/Household.h
	@echo "**"
	@echo "** Compiling C++ Source: Household.cxx Household.h"
	@echo "**"
	g++ $(DEBUG) $(INCFLAGS) -g -c -o $@  $<

bin/PopulationCenter.o:src/PopulationCenter.cxx sars_cov2_sk/PopulationCenter.h
	@echo "**"
	@echo "** Compiling C++ Source: PopulationCenter.cxx PopulationCenter.h"
	@echo "**"
	g++ $(DEBUG) $(INCFLAGS) -g -c -o $@  $<

bin/HelperFunctions.o:src/HelperFunctions.cxx sars_cov2_sk/HelperFunctions.h
	@echo "**"
	@echo "** Compiling C++ Source: HelperFunctions.cxx HelperFunctions.h"
	@echo "**"
	g++ $(DEBUG) $(INCFLAGS) -g -c -o $@  $<

bin/InputData.o:src/InputData.cxx sars_cov2_sk/InputData.h
	@echo "**"
	@echo "** Compiling C++ Source: InputData.cxx InputData.h"
	@echo "**"
	g++ $(DEBUG) $(INCFLAGS) -g -c -o $@  $<

bin/RandomGeneratorPoisson.o:src/RandomGeneratorPoisson.cxx sars_cov2_sk/RandomGeneratorPoisson.h
	@echo "**"
	@echo "** Compiling C++ Source: RandomGeneratorPoisson.cxx RandomGeneratorPoisson.h"
	@echo "**"
	g++ $(DEBUG) $(INCFLAGS) -g -c -o $@  $<

bin/main.o:src/main.cxx
	@echo "**"
	@echo "** Compiling C++ Source"
	@echo "**"
	g++ $(DEBUG) $(INCFLAGS) -g -c -o $@  $<

bin/main:bin/main.o bin/Person.o bin/Household.o bin/PopulationCenter.o bin/HelperFunctions.o bin/InputData.o bin/RandomGeneratorPoisson.o
	@echo "**"
	@echo "** Linking"
	@echo "**"
	g++ $(DEBUG) -o $@ bin/main.o bin/Person.o bin/Household.o bin/PopulationCenter.o bin/HelperFunctions.o bin/InputData.o bin/RandomGeneratorPoisson.o

clean:
	rm -rf bin/*.o main

