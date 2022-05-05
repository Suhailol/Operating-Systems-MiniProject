menu: menu.cpp roundRobin.o priority.o
	g++ -o menu menu.cpp

roundRobin.o: roundRobin.cpp
	g++ -o roundRobin roundRobin.cpp

priority.o: priority.cpp
	g++ -o priority priority.cpp
