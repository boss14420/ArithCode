all: ac

CXX 		= g++
#CXXFLAGS 	= -std=c++11 -g -Wall -pipe
CXXFLAGS 	= -std=c++11 -Wall -O3 -march=native -pipe
#CXXFLAGS 	= -std=c++11 -Wall -O3 -march=native -pipe -g -pg

#OBJS 		= encoder.o decoder.o SemiAdaptiveModel.o \
#		  arithmetic.o bitstream.o main.o
OBJS 		= main.o bitstream.o model.o SemiAdaptiveModel.o AdaptiveModel.o

#%.o: %.cc %.hh
#	$(CXX) $(CXXFLAGS) -c -o $@ $<
   
ac: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

#encoder.o: encoder.cc encoder.hh SemiAdaptiveModel.hh bitstream.hh
#	$(CXX) $(CXXFLAGS) -c -o $@ $<

#decoder.o: decoder.cc decoder.hh SemiAdaptiveModel.hh bitstream.hh
#	$(CXX) $(CXXFLAGS) -c -o $@ $<

model.o: model.cc model.hh
	$(CXX) $(CXXFLAGS) -c -o $@ $<

SemiAdaptiveModel.o: SemiAdaptiveModel.cc SemiAdaptiveModel.hh model.hh bitstream.hh
	$(CXX) $(CXXFLAGS) -c -o $@ $<

AdaptiveModel.o: AdaptiveModel.cc AdaptiveModel.hh model.hh bitstream.hh fenwick_tree.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

#arithmetic.o: arithmetic.cc arithmetic.hh encoder.hh decoder.hh integer.hpp bitstream.hh
#	$(CXX) $(CXXFLAGS) -c -o $@ $<

bitstream.o: bitstream.cc bitstream.hh integer.hpp endian.hh
	$(CXX) $(CXXFLAGS) -c -o $@ $<

main.o: main.cc arithmetic.hh AdaptiveModel.hh SemiAdaptiveModel.hh encoder.hh decoder.hh \
    	encoder.cc decoder.cc model.hh
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -rf *.o
