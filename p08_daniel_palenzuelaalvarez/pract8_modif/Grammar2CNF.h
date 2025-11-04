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
 * Archivo: Grammar2CNF.h: Declaraciones de la clase Grammar.
 *    Contiene la definición de la clase Grammar y estructuras
 *    públicas para leer, validar y transformar una gramática
 *    independiente del contexto a Forma Normal de Chomsky (aplicando el Algoritmo 1).
 * Referencias:
 *    Transparencias del Tema 3 de la asignatura: Lenguajes y Gramáticas Independientes
 *    del Contexto:
 *    https://campusvirtual.ull.es/2526/ingenieriaytecnologia/mod/resource/view.php?id=11876
 * Historial de revisiones
 *    01/11/2025 - Creación (primera versión) del código
 *    02/11/2025 - Documentación y comentarios
*/

/**
 * @file Grammar2CNF.h
 * @brief Declaraciones de la clase Grammar
 *
 * Este archivo contiene la definición de la clase Grammar y estructuras
 * públicas para leer, validar y transformar una gramática independiente
 * del contexto a Forma Normal de Chomsky (aplicando el Algoritmo 1).
 *
 * El programa comprueba que la gramática de entrada NO contenga
 * producciones vacías ni producciones unitarias; si encuentra
 * alguna, informa y finaliza sin hacer la conversión.
 *
 */

#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <string>
#include <vector>
#include <map>
#include <set>

/**
 * @brief Representa una producción de la gramática.
 *
 * lhs: símbolo no terminal en la parte izquierda (por ejemplo "S" o "X").
 * rhs: secuencia de símbolos (cada símbolo es una cadena; las no terminales
 *      comienzan por una letra mayúscula, los terminales son un solo carácter
 *      imprimible representado como cadena de longitud 1). La cadena vacía
 *      (epsilon) se representa con un único elemento "&".
 */
struct Production {
    std::string lhs; // Lado izquierdo de la producción
    std::vector<std::string> rhs; // Lado derecho de la producción
};

/**
 * @class Grammar
 * @brief Clase que modela una gramática independiente del contexto y ofrece
 *        operaciones para validarla y transformarla a FNC.
 *
 * Uso principal:
 *  - leer la gramática desde un fichero .gra
 *  - validar formato y precondiciones (sin producciones unitarias ni vacías)
 *  - aplicar Algoritmo 1 para convertir a FNC (solo si pasa la validación)
 *  - escribir la gramática resultante en fichero .gra
 */
class Grammar {
public:
    /**
     * @brief Construye una gramática vacía.
     */
    Grammar();

    /**
     * @brief Lee una gramática desde el fichero pasado por path en formato .gra.
     * @param path Ruta del fichero de entrada.
     * @throws std::runtime_error En caso de error de formato o lectura.
     */
    void ReadFromFile(const std::string& path);

    /**
     * @brief Escribe la gramática actual en el fichero path en formato .gra.
     * @param path Ruta del fichero de salida.
     * @throws std::runtime_error En caso de error al escribir.
     */
    void WriteToFile(const std::string& path) const;

    /**
     * @brief Valida que la gramática de entrada cumpla el convenio exigido por la práctica.
     *
     * Revisa que:
     *  - todos los no terminales de la entrada sean una única letra mayúscula
     *  - los terminales sean símbolos de un solo carácter
     *  - las producciones usen símbolos declarados
     *
     * @throws std::runtime_error Si detecta un error de formato.
     */
    void ValidateFormat() const;

    /**
     * @brief Comprueba precondiciones requeridas por el Algoritmo 1.
     *
     * Detecta si existen producciones vacías (rhs == "&") o producciones unitarias
     * (A -> B con B no terminal). Si se detecta cualquiera, se lanza excepción.
     *
     * @throws std::runtime_error Si detecta producciones vacías o unitarias.
     */
    void CheckPreconditions() const;

    /**
     * @brief Aplica el Algoritmo 1 (solo) para convertir la gramática a FNC.
     *
     * Precondición: CheckPreconditions() ya ha sido invocado y no ha detectado
     * errores. El método añade nuevos no terminales y producciones según el
     * algoritmo y deja como resultado una gramática en FNC.
     */
    void TransformToCNF();

    /**
     * @brief Devuelve el símbolo de arranque de la gramática.
     * @return cadena con el símbolo de arranque.
     */
    std::string StartSymbol() const;

    // MODIF:
    /**
     * @brief Devuelve el conjunto de no terminales alcanzables desde s.
     * Recorre todas las producciones partiendo de s y determina qué símbolos
     * no terminales pueden alcanzarse.
     * @return Conjunto de símbolos no terminales alcanzables
     */
    std::set<char> ReachableNonTerminals() const;

    /**
     * @brief Devuelve el conjunto de no terminales declarados en la entrada.
     * @return Conjunto de no terminales declarados.
     */
    std::set<char> DeclaredNonTerminals() const;

private:
    /**
     * @brief Conjunto de símbolos terminales (cada uno es un carácter).
     *
     * Cada entrada es un carácter imprimible que representa un terminal.
     */
    std::set<char> terminals_;

    /**
     * @brief Conjunto de símbolos no terminales (cadenas alfanuméricas).
     *
     * En la entrada se requiere que los no terminales sean una sola letra mayúscula.
     * Sin embargo, la transformación puede añadir nuevos no terminales
     * (ej Ca, D1, etc).
     */
    std::set<std::string> nonterminals_;

    /**
     * @brief Símbolo de arranque (el primero en la lista de no terminales de la entrada).
     */
    std::string start_symbol_;

    /**
     * @brief Lista de producciones completas de la gramática.
     */
    std::vector<Production> productions_;

    /**
     * @brief Contador auxiliar para generar nombres únicos de no terminales Dk.
     */
    int counter_d_;

    /**
     * @brief Mapa para símbolos auxiliares que representan terminales: terminal -> nombre del no terminal.
     *
     * Ejemplo: 'a' -> "Ca"
     */
    std::map<char, std::string> terminal_to_nt_;

    /**
     * @brief Genera un nuevo nombre para un no terminal de tipo Dk.
     * @return nombre único (ej "D1", "D2", etc).
     */
    std::string NewD();

    /**
     * @brief Obtiene (o crea si no existe) un no terminal que represente al terminal `t`.
     * @param t Terminal (carácter).
     * @return nombre del no terminal asociado (ej "Ca").
     */
    std::string TerminalToNonTerminal(char t);

    /**
     * @brief Función auxiliar que convierte una rhs (vector de tokens) en una única cadena
     * adecuada para escribir en el fichero .gra (se concatenan las representaciones).
     * @param rhs Vector de tokens de la rhs.
     */
    static std::string RhsToString(const std::vector<std::string>& rhs);
};

#endif