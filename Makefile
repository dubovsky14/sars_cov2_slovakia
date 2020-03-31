MACHINE = $(shell uname -s)
LINUX   = Linux
MAC     = Darwin

BASEDIR  = .

DEBUG=-g -Wall -ggdb -O3 -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0

INCDIR=$(PWD) 

INCFLAGS=$(DEBUG) -I$(INCDIR)

all: bin/main

bin/Logging.o:src/Logging.cxx sars_cov2_sk/Logging.h
	@echo "**"
	@echo "** Compiling C++ Source: Logging.cxx Logging.h"
	@echo "**"
	g++ $(DEBUG) $(INCFLAGS) -g -c -o $@  $<

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

bin/RandomGenerators.o:src/RandomGenerators.cxx sars_cov2_sk/RandomGenerators.h
	@echo "**"
	@echo "** Compiling C++ Source: RandomGenerators.cxx RandomGenerators.h"
	@echo "**"
	g++ $(DEBUG) $(INCFLAGS) -g -c -o $@  $<

bin/ConfigParser.o:src/ConfigParser.cxx sars_cov2_sk/ConfigParser.h
	@echo "**"
	@echo "** Compiling C++ Source: ConfigParser.cxx ConfigParser.h"
	@echo "**"
	g++ $(DEBUG) $(INCFLAGS) -g -c -o $@  $<

bin/Simulation.o:src/Simulation.cxx sars_cov2_sk/Simulation.h
	@echo "**"
	@echo "** Compiling C++ Source: Simulation.cxx Simulation.h"
	@echo "**"
	g++ $(DEBUG) $(INCFLAGS) -g -c -o $@  $<

bin/main.o:src/main.cxx
	@echo "**"
	@echo "** Compiling C++ Source"
	@echo "**"
	g++ $(DEBUG) $(INCFLAGS) -g -c -o $@  $<

bin/main:bin/main.o bin/Person.o bin/Household.o bin/PopulationCenter.o bin/HelperFunctions.o bin/InputData.o bin/RandomGenerators.o bin/Logging.o bin/ConfigParser.o bin/Simulation.o
	@echo "**"
	@echo "** Linking"
	@echo "**"
	g++ $(DEBUG) -o $@ bin/main.o bin/Person.o bin/Household.o bin/PopulationCenter.o bin/HelperFunctions.o bin/InputData.o bin/RandomGenerators.o bin/Logging.o bin/ConfigParser.o bin/Simulation.o

clean:
	rm -rf bin/*.o main

