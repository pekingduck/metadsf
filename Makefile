CXX=g++
CXXFLAGS=-Wall -I/usr/local/include -std=c++11
#OBJS=metadsf.o main.o
SRCS=main.cpp metadsf.cpp options.cpp utils.cpp
OBJS=metadsf.o main.o options.o utils.o dsfproperties.o dsfheader.o dsffile.o
LDFLAGS=
LDLIBS=-lz -L/usr/local/lib -ltag
EXEC=metadsf

all: $(EXEC)

$(EXEC): $(OBJS)
	g++ -o $@ $(OBJS) $(LDLIBS)

mkdsf: mkdsf.cpp
	g++ -o $@ $< 

test: test.cpp
	g++ -o $@ $(CXXFLAGS) $< $(LDLIBS)

#-include $(OBJS:.o=.d)

#%o: %.cpp
#	$(CXX) -c $(CXXFLAGS) -o $*.o $*.cpp 
#	$(CXX) -MM $(CXXFLAGS) $*.cpp > $*.d

clean:
	rm -f $(EXEC) $(OBJS) test mkdsf

# %.d: %.cpp
# 	@set -e; rm -f; \
# 	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
# 	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
# 	rm -f $@.$$$$

#include $(SRCS:.cpp=.d)
