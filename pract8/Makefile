CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g
SRCS = main.cc Grammar2CNF.cc
OBJS = $(SRCS:.cc=.o)
TARGET = Grammar2CNF

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)