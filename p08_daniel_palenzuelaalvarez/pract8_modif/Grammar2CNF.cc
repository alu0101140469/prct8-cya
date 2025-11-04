/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Computabilidad y Algoritmia
 * Curso: 2º
 * Práctica 8: Gramáticas en Forma Normal de Chomsky.
 * Autor: Daniel Palenzuela Álvarez
 * Correo: alu0101140469@ull.edu.es
 * Fecha: 04/11/2025
 * Archivo: Grammar2CNF.cc: Código fuente que implementa la clase Grammar.
 *    Contiene la lógica para transformar gramáticas a Forma Normal de Chomsky, aplicando el Algoritmo 1.
 * Referencias:
 *    Transparencias del Tema 3 de la asignatura: Lenguajes y Gramáticas Independientes
 *    del Contexto:
 *    https://campusvirtual.ull.es/2526/ingenieriaytecnologia/mod/resource/view.php?id=11876
 * Historial de revisiones
 *    01/11/2025 - Creación (primera versión) del código
 *    02/11/2025 - Documentación y comentarios
*/

/**
 * @file Grammar2CNF.cc
 * @brief Implementación de la clase Grammar.
 *
 * Implementa lectura/escritura de gramáticas en formato .gra, validación y
 * transformación a Forma Normal de Chomsky (aplicando el Algoritmo 1).
 */

#include "Grammar2CNF.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <iomanip>
#include <algorithm>
#include <queue>

using std::string;
using std::vector;
using std::map;
using std::set;

/**
 * @brief Constructor por defecto.
 *
 * Inicializa el contador de D's a 0.
 */
Grammar::Grammar()
    : counter_d_(0) {
}

/**
 * @brief Función auxiliar para eliminar espacios a izquierda/derecha.
 * @param s Cadena que se va a recortar.
 */
static void TrimString(string &s) {
    // Buscar primer carácter no espacio
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) { s.clear(); return; }
    // Buscar último carácter no espacio
    size_t b = s.find_last_not_of(" \t\r\n");
    s = s.substr(a, b - a + 1);
}

/**
 * @brief Lee una gramática desde un fichero en formato .gra.
 * @param path Ruta del fichero de entrada.
 *
 * @throws std::runtime_error Si el fichero no existe o el formato es inválido.
 */
void Grammar::ReadFromFile(const string& path) {
    // Abrir fichero de entrada
    std::ifstream ifs(path);
    if (!ifs) {
        throw std::runtime_error("No se pudo abrir el fichero de entrada: " + path);
    }

    // Reiniciar estructuras internas antes de leer
    terminals_.clear();
    nonterminals_.clear();
    productions_.clear();
    terminal_to_nt_.clear();
    counter_d_ = 0;
    start_symbol_.clear();

    // Función auxiliar para leer una línea con un entero
    auto read_int_line = [&](int &out) {
        string line;
        while (std::getline(ifs, line)) {
            TrimString(line); // quitar espacios alrededor
            if (line.empty()) continue; // ignorar líneas vacías
            std::istringstream iss(line);
            if (iss >> out) return; // leído correctamente
            // si no se pudo parsear como entero, error
            throw std::runtime_error("Formato inválido: se esperaba un número en una línea específica.");
        }
        // Si llegamos al EOF sin leer el número
        throw std::runtime_error("Formato inválido: archivo terminado inesperadamente al leer número.");
    };

    // 1) leer terminals
    int n_terms = 0;
    read_int_line(n_terms); // lee número de terminales
    for (int i = 0; i < n_terms; ++i) {
        string line;
        if (!std::getline(ifs, line)) throw std::runtime_error("Formato inválido: faltan símbolos terminales.");
        TrimString(line);
        if (line.empty()) { --i; continue; } // tolerar líneas vacías intercaladas
        if (line.size() != 1) {
            // cada terminal en entrada debe ser un único carácter
            throw std::runtime_error("Formato inválido: cada símbolo terminal debe ser un único carácter (línea: '" + line + "').");
        }
        // insertar el carácter terminal en el conjunto
        terminals_.insert(line[0]);
    }

    // 2) leer nonterminals
    int n_nt = 0;
    read_int_line(n_nt); // lee número de no terminales
    vector<string> nt_list;
    for (int i = 0; i < n_nt; ++i) {
        string line;
        if (!std::getline(ifs, line)) throw std::runtime_error("Formato inválido: faltan símbolos no terminales.");
        TrimString(line);
        if (line.empty()) { --i; continue; } // tolera líneas vacías
        nt_list.push_back(line); // guardar orden para obtener símbolo inicial
        nonterminals_.insert(line); // insertar en conjunto de no terminales
    }
    if (!nt_list.empty()) start_symbol_ = nt_list[0];
    // Si no hay no terminales, lanzar error
    else throw std::runtime_error("Formato inválido: no hay no terminales definidos.");

    // 3) leer productions
    int pcount = 0;
    read_int_line(pcount); // número de producciones
    for (int i = 0; i < pcount; ++i) {
        string line;
        if (!std::getline(ifs, line)) throw std::runtime_error("Formato inválido: faltan producciones.");
        TrimString(line);
        if (line.empty()) { --i; continue; } // tolera líneas vacías

        // parsear línea de producción: LHS -> RHS
        std::istringstream iss(line);
        string left;
        if (!(iss >> left)) throw std::runtime_error("Formato inválido en producción: '" + line + "'");
        string rhs_all;
        if (!(iss >> rhs_all)) throw std::runtime_error("Formato inválido en producción (falta RHS): '" + line + "'");

        // construir producción
        Production prod;
        prod.lhs = left;

        // Tokenización de RHS
        if (rhs_all == "&") {
            // producción vacía
            prod.rhs.push_back("&");
        } else {
            size_t idx = 0;
            while (idx < rhs_all.size()) {
                char c = rhs_all[idx];
                // Si es mayúscula -> no terminal de la entrada
                if (std::isupper(static_cast<unsigned char>(c))) {
                    prod.rhs.push_back(std::string(1, c));
                    ++idx;
                } else {
                    // terminal: siempre un único carácter de longitud 1
                    prod.rhs.push_back(std::string(1, c));
                    ++idx;
                }
            }
        }

        // Añadir producción al vector de producciones
        productions_.push_back(prod);
    }
}

/**
 * @brief Escribe la gramática en formato .gra en el fichero indicado.
 * @param path Ruta del fichero de salida.
 *
 * @throws std::runtime_error Si no se puede crear el fichero.
 */
void Grammar::WriteToFile(const string& path) const {
    std::ofstream ofs(path); // Abrir fichero de salida
    if (!ofs) {
        throw std::runtime_error("No se pudo crear el fichero de salida: " + path);
    }

    // Escribir número de terminales y lista
    ofs << terminals_.size() << "\n";
    for (char t : terminals_) {
        ofs << t << "\n";
    }

    // Escribir número de no terminales y lista
    ofs << nonterminals_.size() << "\n";
    for (const auto& nt : nonterminals_) {
        ofs << nt << "\n";
    }

    // Escribir número de producciones y cada producción en una línea
    ofs << productions_.size() << "\n";
    for (const auto& p : productions_) {
        // LHS + espacio + RHS concatenada
        ofs << p.lhs << " ";
        if (p.rhs.size() == 1 && p.rhs[0] == "&") {
            // epsilon representado por &
            ofs << "&";
        } else {
            // concatenar RHS, por ejemplo A B C -> "ABC"
            ofs << RhsToString(p.rhs);
        }
        ofs << "\n";
    }
}

/**
 * @brief Valida el formato de la gramática leída.
 *
 * Revisa:
 *  - que los terminales sean caracteres imprimibles
 *  - que los no terminales de la entrada sean una única letra mayúscula
 *  - que las producciones se refieran a símbolos declarados.
 *
 * @throws std::runtime_error en caso de error de formato.
 */
void Grammar::ValidateFormat() const {
    // Validar terminales: no deben ser caracteres de control
    for (char t : terminals_) {
        if (std::iscntrl(static_cast<unsigned char>(t)))
            throw std::runtime_error("Terminal inválido (carácter de control encontrado).");
    }

    // Validar no terminales: en la entrada deben ser exactamente una letra mayúscula
    for (const auto& nt : nonterminals_) {
        if (nt.size() != 1 || !std::isupper(static_cast<unsigned char>(nt[0]))) {
            throw std::runtime_error("Formato inválido: en la entrada, cada no terminal debe ser una única letra mayúscula. Encontrado: '" + nt + "'.");
        }
    }

    // Validar que las producciones refieran símbolos declarados
    for (const auto& p : productions_) {
        // LHS debe estar declarado
        if (nonterminals_.find(p.lhs) == nonterminals_.end())
            throw std::runtime_error("Producción con LHS no declarado: '" + p.lhs + "'.");
        // RHS puede ser epsilon
        if (p.rhs.size() == 1 && p.rhs[0] == "&") continue;

        // Para cada token en RHS: o es un no terminal declarado o un terminal declarado
        for (const auto& tok : p.rhs) {
            if (tok.empty()) throw std::runtime_error("Producción con token vacío en RHS.");
            if (std::isupper(static_cast<unsigned char>(tok[0]))) {
                // token empieza por mayúscula => no terminal
                if (nonterminals_.find(tok) == nonterminals_.end())
                    throw std::runtime_error("Producción con no terminal en RHS no declarado: '" + tok + "'.");
            } else {
                // token terminal debe ser de longitud 1 y estar declarado
                if (tok.size() != 1) throw std::runtime_error("Terminal en RHS necesita ser un único carácter: '" + tok + "'.");
                if (terminals_.find(tok[0]) == terminals_.end())
                    throw std::runtime_error("Terminal en RHS no declarado: '" + tok + "'.");
            }
        }
    }
}

/**
 * @brief Comprueba precondiciones requeridas por el Algoritmo 1:
 *        - no hay producciones vacías
 *        - no hay producciones unitarias A -> B
 *
 * @throws std::runtime_error si detecta alguna de las condiciones prohibidas.
 */
void Grammar::CheckPreconditions() const {
    // Buscar producciones epsilon
    for (const auto& p : productions_) {
        if (p.rhs.size() == 1 && p.rhs[0] == "&")
            throw std::runtime_error("La gramática contiene la producción vacía: " + p.lhs + " -> &. Abortando.");
    }
    // Buscar producciones unitarias A -> B
    for (const auto& p : productions_) {
        if (p.rhs.size() == 1) {
            const string &tok = p.rhs[0];
            if (!tok.empty() && std::isupper(static_cast<unsigned char>(tok[0])))
                throw std::runtime_error("La gramática contiene una producción unitaria: " + p.lhs + " -> " + tok + ". Abortando.");
        }
    }
}

/**
 * @brief Genera un nombre nuevo para un no terminal auxiliar Dk y lo añade al conjunto.
 * @return cadena con el nuevo nombre ("D1", "D2", etc).
 */
std::string Grammar::NewD() {
    // Incrementar contador de las D
    ++counter_d_;
    // Crear nombre único para el no terminal
    std::string name = std::string("D") + std::to_string(counter_d_);
    // Insertar en el conjunto de no terminales
    nonterminals_.insert(name);
    // Devolver el nombre generado
    return name;
}

/**
 * @brief Obtiene (o crea) un no terminal que represente al terminal t.
 * @param t Caracter terminal.
 * @return nombre del no terminal creado o ya existente (ej "Ca").
 */
std::string Grammar::TerminalToNonTerminal(char t) {
    // Buscar si ya existe el mapping para este terminal
    auto it = terminal_to_nt_.find(t);
    if (it != terminal_to_nt_.end()) return it->second;

    // Crear nombre base para el no terminal auxiliar: "C" + carácter (ej. "Ca")
    std::string name = std::string("C") + t;

    // Asegurar que sea único, si ya existe añadir sufijo numérico incrementando
    int suffix = 1;
    std::string candidate = name;
    while (nonterminals_.find(candidate) != nonterminals_.end()) {
        candidate = name + std::to_string(suffix++);
    }
    name = candidate;

    // Insertar nuevo no terminal
    nonterminals_.insert(name);
    // Guardar en el mapa para futuras consultas
    terminal_to_nt_[t] = name;

    // Añadir la producción <name> -> t
    Production p;
    p.lhs = name;
    p.rhs.clear();
    p.rhs.push_back(std::string(1, t));
    productions_.push_back(p);

    // Asegurarse de que el terminal esté registrado
    terminals_.insert(t);

    return name;
}

/**
 * @brief Convierte un vector de tokens RHS a una cadena concatenada para escritura.
 * @param rhs Vector de tokens (ej {"Ca","X","D1"})
 * @return cadena concatenada (ej "CaXD1")
 */
std::string Grammar::RhsToString(const std::vector<std::string>& rhs) {
    std::string out;
    for (const auto& tok : rhs) out += tok;
    return out;
}

/**
 * @brief Aplica el Algoritmo 1 para transformar la gramática a Forma Normal de Chomsky.
 */
void Grammar::TransformToCNF() {
    // Primer loop: reemplazar terminales en producciones con m >= 2
    size_t original_size = productions_.size();
    for (size_t i = 0; i < original_size; ++i) {
        Production &p = productions_[i];
        // Solo nos interesa cuando RHS tiene 2 o más símbolos
        if (p.rhs.size() >= 2) {
            for (std::string &tok : p.rhs) {
                // Si es un terminal, reemplazar por no terminal auxiliar
                if (tok.size() == 1 && !std::isupper(static_cast<unsigned char>(tok[0]))) {
                    char t = tok[0];
                    // Obtener/crear no terminal auxiliar que genera t
                    std::string nt = TerminalToNonTerminal(t);
                    // Reemplazar el token terminal por el nombre del no terminal creado
                    tok = nt;
                }
            }
        }
    }

    // Segundo loop: para producciones con m >= 3, descomponer en producciones binarias
    vector<Production> new_productions; // nuevas producciones resultantes
    for (const auto& p : productions_) {
        if (p.rhs.size() >= 3) {
            // m = longitud de RHS
            size_t m = p.rhs.size();

            // primera producción: A -> B1 D1
            Production first;
            first.lhs = p.lhs;
            first.rhs.clear();
            first.rhs.push_back(p.rhs[0]); // B1
            std::string prevD = NewD(); // D1
            first.rhs.push_back(prevD); // A -> B1 D1
            new_productions.push_back(first);

           // Crear producciones intermedias D_i -> B_{i+1} D_{i+1}
            for (size_t i = 1; i < m - 2; ++i) {
                Production mid;
                mid.lhs = prevD; // D_i
                mid.rhs.clear();
                mid.rhs.push_back(p.rhs[i]); // B_{i+1}
                std::string nextD = NewD(); // D_{i+1}
                mid.rhs.push_back(nextD);
                new_productions.push_back(mid);
                prevD = nextD; // avanzar el puntero de D
            }

            // Última producción: D_{m-2} -> B_{m-1} B_m
            Production last;
            last.lhs = prevD; // D_{m-2}
            last.rhs.clear();
            last.rhs.push_back(p.rhs[m - 2]); // B_{m-1}
            last.rhs.push_back(p.rhs[m - 1]); // B_m
            new_productions.push_back(last);
        } else {
            // Producción con m < 3, se mantiene igual
            new_productions.push_back(p);
        }
    }

    // Reemplazar el conjunto de producciones por las nuevas (binarizadas)
    productions_.swap(new_productions);
}

/**
 * @brief Devuelve el símbolo inicial de la gramática.
 * @return símbolo de arranque (string).
 */
std::string Grammar::StartSymbol() const {
    return start_symbol_;
}

// MODIF:
/**
 * @brief Devuelve el conjunto de no terminales declarados en la gramática de entrada.
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
 */
std::set<char> Grammar::ReachableNonTerminals() const {
    std::set<char> reachable;

    // Si no hay símbolo inicial definido, devolver conjunto vacío
    if (start_symbol_.empty()) return reachable;

    // El símbolo inicial, en la entrada, es una única letra mayúscula
    char s = start_symbol_[0];
    if (!std::isupper(static_cast<unsigned char>(s))) return reachable;

    std::queue<char> q;
    reachable.insert(s);
    q.push(s);

    // iterar mientras haya no terminales por explorar
    while (!q.empty()) {
        char A = q.front();
        q.pop();
        // LHS esperado como string de longitud 1
        std::string lhs(1, A);
        // Recorrer todas las producciones y buscar las de LHS == A
        for (const auto& p : productions_) {
            if (p.lhs != lhs) continue;
            // Examinar tokens de RHS, si son no terminales de 1 carácter se consideran
            for (const auto& tok : p.rhs) {
                if (tok.size() == 1 && std::isupper(static_cast<unsigned char>(tok[0]))) {
                    char B = tok[0];
                    // Si no estaba visitado
                    if (reachable.insert(B).second) {
                        q.push(B);
                    }
                }
            }
        }
    }

    return reachable;
}