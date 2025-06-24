.PHONY: clean all

LLVM_CONFIG := llvm-config-19
LLVM_CXXFLAGS := $(shell $(LLVM_CONFIG) --cxxflags)
LLVM_LDFLAGS := $(shell $(LLVM_CONFIG) --ldflags --libs --libfiles --system-libs)

all: lfmc

lfmc: driver.o parser.o scanner.o lfmc.o
	clang++ -o lfmc driver.o parser.o scanner.o lfmc.o $(LLVM_LDFLAGS)

lfmc.o: lfmc.cpp driver.hpp
	clang++ -c lfmc.cpp $(LLVM_CXXFLAGS)

parser.o: parser.cpp
	clang++ -c parser.cpp $(LLVM_CXXFLAGS)

scanner.o: scanner.cpp parser.hpp
	clang++ -c scanner.cpp $(filter-out -fno-exceptions, $(LLVM_CXXFLAGS))


driver.o: driver.cpp parser.hpp driver.hpp
	clang++ -c driver.cpp $(LLVM_CXXFLAGS)

parser.cpp parser.hpp: parser.yy
	bison -o parser.cpp parser.yy

scanner.cpp: scanner.ll
	flex -o scanner.cpp scanner.ll

clean:
	rm -f *~ driver.o scanner.o parser.o lfmc.o lfmc scanner.cpp parser.cpp parser.hpp

