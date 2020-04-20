CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra

traceroute: main.o packets_sender.o packets_receiver.o 
	$(CXX) $(CXXFLAGS) -o traceroute main.o packets_sender.o packets_receiver.o

main.o: main.cpp

packets_sender.o: packets_sender.cpp packets_sender.h

packets_receiver.o: packets_receiver.cpp packets_receiver.h

clean:
	rm -rf *.o

distclean:
	rm -rf traceroute *.0

.PHONY: clean
