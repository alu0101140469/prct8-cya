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



Implementar un método que permita determinar qué símbolos no terminales son alcanzables desde el símbolo inicial s.
Esta funcionalidad debe ejecutarse siempre al iniciar el programa, antes de cualquier conversión.
En clase "ContextFreeGrammar" hay que crear: std::set<char> ReachableNonTerminals() const;
El programa debe mostrar siempre los símbolos alcanzables desde s al leer la gramática, y advertir si existen no alcanzables.

