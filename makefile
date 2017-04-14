CXX=g++

kk: kk.cpp
	$(CXX) -std=c++11 -Wall -o kk kk.cpp

clean:
	rm -f *.o
	rm -f kk

