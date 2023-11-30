CXX = g++
CXXFLAGS = -std=c++11 -pthread
FILE1=shell.cpp
FILE2=mv-threaded.cpp
FILE3=cp-threaded.cpp
all: normal cpthreaded mvthreaded

normal: shell.cpp
	$(CXX) $(FILE1) 
cpthreaded: cp-threaded.cpp
	$(CXX) $(FILE2)

mvthreaded: mv-threaded.cpp
	$(CXX) $(FILE3)

clean:
	rm -f normal cpthreaded mvthreaded
