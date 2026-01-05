EXE = pa3

OBJS_EXE = RGBAPixel.o lodepng.o PNG.o main.o hextree.o hextree-given.o

CXX = clang++
CXXFLAGS = -std=c++1y -c -g -O0 -Wall -Wextra -pedantic 
LD = clang++
#LDFLAGS = -std=c++1y -stdlib=libc++ -lc++abi -lpthread -lm
LDFLAGS = -std=c++1y -lpthread -lm 

all : pa3

$(EXE) : $(OBJS_EXE)
	$(LD) $(OBJS_EXE) $(LDFLAGS) -o $(EXE)

#object files
RGBAPixel.o : util/RGBAPixel.cpp util/RGBAPixel.h
	$(CXX) $(CXXFLAGS) util/RGBAPixel.cpp -o $@

PNG.o : util/PNG.cpp util/PNG.h util/RGBAPixel.h util/lodepng/lodepng.h
	$(CXX) $(CXXFLAGS) util/PNG.cpp -o $@

lodepng.o : util/lodepng/lodepng.cpp util/lodepng/lodepng.h
	$(CXX) $(CXXFLAGS) util/lodepng/lodepng.cpp -o $@

hextree.o : hextree.h hextree-private.h hextree.cpp util/PNG.h util/RGBAPixel.h
	$(CXX) $(CXXFLAGS) hextree.cpp -o $@

hextree-given.o : hextree.h hextree-private.h hextree-given.cpp util/PNG.h util/RGBAPixel.h
	$(CXX) $(CXXFLAGS) hextree-given.cpp -o $@

main.o : main.cpp util/PNG.h util/RGBAPixel.h hextree.h hextree.h
	$(CXX) $(CXXFLAGS) main.cpp -o main.o

clean :
	-rm -f *.o $(EXE) images-output/*.png
