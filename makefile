cacheSim: cacheSim.cpp
	g++ -o cacheSim cacheSim.cpp memory.cpp

.PHONY: clean
clean:
	rm -f *.o
	rm -f cacheSim
