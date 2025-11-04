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







Aquí tienes solo los cambios que aplico al código (añadir declaraciones en la cabecera, implementación en el .cc y la llamada + salida en main.cc). He añadido además un método auxiliar para obtener los no terminales declarados (necesario para advertir de no alcanzables). Todos los cambios respetan Doxygen para la API pública.


---

1) Grammar2CNF.h — añadir las declaraciones públicas (añadir dentro de la clase Grammar)

@@
     /**
      * @brief Devuelve el símbolo de arranque de la gramática.
      * @return cadena con el símbolo de arranque.
      */
     std::string StartSymbol() const;
 
+    /**
+     * @brief Devuelve el conjunto de no terminales alcanzables desde s.
+     * Recorre todas las producciones partiendo de s y determina qué símbolos
+     * no terminales pueden alcanzarse transitivamente.
+     * @return Conjunto de símbolos no terminales alcanzables
+     */
+    std::set<char> ReachableNonTerminals() const;
+
+    /**
+     * @brief Devuelve el conjunto de no terminales declarados en la entrada (símbolos de una letra).
+     * @return Conjunto de no terminales declarados (letras mayúsculas).
+     */
+    std::set<char> DeclaredNonTerminals() const;


---

2) Grammar2CNF.cc — añadir include y implementar los métodos (añadir las siguientes porciones)

(a) Añadir include al inicio del fichero:

#include <algorithm>
+#include <queue>

(b) Implementación de DeclaredNonTerminals() y ReachableNonTerminals() (añadir en cualquier lugar después de las otras implementaciones)

/**
 * @brief Devuelve el conjunto de no terminales declarados en la gramática de entrada.
 *
 * Sólo se consideran los no terminales que tienen tamaño 1 y son letras mayúsculas,
 * que es la convención exigida para la entrada.
 */
std::set<char> Grammar::DeclaredNonTerminals() const {
    std::set<char> out;
    for (const auto& nt : nonterminals_) {
        if (nt.size() == 1 && std::isupper(static_cast<unsigned char>(nt[0]))) {
            out.insert(nt[0]);
        }
    }
    return out;
}

/**
 * @brief Devuelve el conjunto de no terminales alcanzables desde el símbolo inicial.
 *
 * Realiza un recorrido en anchura (BFS) empezando desde start_symbol_ y explora
 * las producciones; añade a 'reachable' cada no terminal que aparece en RHS
 * y que puede alcanzarse transitivamente.
 */
std::set<char> Grammar::ReachableNonTerminals() const {
    std::set<char> reachable;

    // Si no hay símbolo inicial definido, devolver conjunto vacío
    if (start_symbol_.empty()) return reachable;

    // El símbolo inicial, en la entrada, es una única letra mayúscula (por convenio)
    char s = start_symbol_[0];
    if (!std::isupper(static_cast<unsigned char>(s))) return reachable;

    std::queue<char> q;
    reachable.insert(s);
    q.push(s);

    // BFS: iterar mientras haya no terminales por explorar
    while (!q.empty()) {
        char A = q.front();
        q.pop();
        // LHS esperado como string de longitud 1
        std::string lhs(1, A);
        // Recorrer todas las producciones y buscar las de LHS == A
        for (const auto& p : productions_) {
            if (p.lhs != lhs) continue;
            // Examinar tokens de RHS; si son no terminales de 1 carácter, considerarlos
            for (const auto& tok : p.rhs) {
                if (tok.size() == 1 && std::isupper(static_cast<unsigned char>(tok[0]))) {
                    char B = tok[0];
                    // Si no estaba visitado, marcar y encolar
                    if (reachable.insert(B).second) {
                        q.push(B);
                    }
                }
            }
        }
    }

    return reachable;
}


---

3) main.cc — insertar la llamada y la salida informativa justo después de g.ValidateFormat();

Sustituye (o añade) inmediatamente tras g.ValidateFormat(); lo siguiente:

g.ValidateFormat();
+        // Mostrar no terminales alcanzables desde el símbolo inicial y advertir si hay no alcanzables.
+        // Esta comprobación se hace siempre al iniciar el programa (antes de cualquier conversión).
+        {
+            std::set<char> reachable = g.ReachableNonTerminals();
+            std::set<char> declared = g.DeclaredNonTerminals();
+
+            // Mostrar alcanzables
+            std::cout << "No terminales alcanzables desde " << g.StartSymbol() << ": ";
+            for (char c : reachable) std::cout << c << ' ';
+            std::cout << std::endl;
+
+            // Si hay declarados que no están en alcanzables, advertir
+            if (reachable.size() != declared.size()) {
+                std::cerr << "Advertencia: existen no terminales NO alcanzables: ";
+                for (char c : declared) {
+                    if (reachable.find(c) == reachable.end()) std::cerr << c << ' ';
+                }
+                std::cerr << std::endl;
+            }
+        }


---

Con estos cambios:

Se calcula siempre el conjunto de no terminales alcanzables desde s al inicio (antes de la conversión).

Se imprime la lista de alcanzables por std::cout.

Si existen no terminales declarados que no son alcanzables, se emite una advertencia por std::cerr.

DeclaredNonTerminals() permite obtener los no terminales de entrada (letras mayúsculas) para comparar con los alcanzables.



---

Si quieres, puedo:

aplicarte el parche directamente al fichero (mostrar el fichero completo modificado), o

generar un diff en formato patch/unified para que lo apliques con git apply o patch.


¿Qué prefieres?
