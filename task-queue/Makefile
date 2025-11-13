CXX = g++
CXXFLAGS = -std=c++17 -pthread -Wall -O2

TARGETS = producer worker

all: $(TARGETS)

producer: producer.cpp redis_client.cpp redis_client.h
	$(CXX) $(CXXFLAGS) producer.cpp redis_client.cpp -o producer

worker: worker.cpp redis_client.cpp redis_client.h
	$(CXX) $(CXXFLAGS) worker.cpp redis_client.cpp -o worker

clean:
	rm -f $(TARGETS)

.PHONY: all clean
