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
El programa debe mostrar siempre los símbolos alcanzables desde s al leer la gramática, y advertir si existen no alcanzables.

En clase "ContextFreeGrammar" hay que crear: 
/**
     * @brief Devuelve el conjunto de no terminales alcanzables desde s.
     * Recorre todas las producciones partiendo de s y determina qué símbolos
     * no terminales pueden alcanzarse transitivamente.
     * @return Conjunto de símbolos no terminales alcanzables
     */
    std::set<char> ReachableNonTerminals() const;
