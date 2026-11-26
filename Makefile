CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Iinclude
SRC      := main.cpp $(wildcard src/*.cpp)

erl_hub: $(SRC) $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) $(SRC) -o erl_hub

clean:
	rm -f erl_hub erl_hub.exe data/*.tmp
