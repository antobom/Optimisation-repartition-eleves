CXX ?= g++
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra
TARGET := opti_eleve
SRC := src/main.cpp src/genetique.cpp src/eleves.cpp src/parametres.cpp

$(TARGET): $(SRC) src/genetique.hpp src/eleves.hpp src/parametres.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)

clean:
	rm -f $(TARGET) $(TARGET).exe

.PHONY: clean
