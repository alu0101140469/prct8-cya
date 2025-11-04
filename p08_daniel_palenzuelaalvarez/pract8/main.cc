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
 * Archivo: main.cc: Programa principal para Grammar2CNF.
 *    Contiene el código para ejecutar la transformación de gramáticas a FNC.
 * Referencias:
 *    Transparencias del Tema 3 de la asignatura: Lenguajes y Gramáticas Independientes
 *    del Contexto:
 *    https://campusvirtual.ull.es/2526/ingenieriaytecnologia/mod/resource/view.php?id=11876
 * Historial de revisiones
 *    01/11/2025 - Creación (primera versión) del código
 *    02/11/2025 - Documentación y comentarios
*/

/**
 * @file main.cc
 * @brief Programa principal para Grammar2CNF
 *
 * Compilar con el Makefile proporcionado y ejecutar:
 *   ./Grammar2CNF input.gra output.gra
 *
 * El programa valida la gramática de entrada (formato y precondiciones) y
 * si todo es correcto, aplica únicamente el Algoritmo 1 para convertir
 * la gramática a Forma Normal de Chomsky y escribe el resultado.
 */

#include <iostream>
#include <string>
#include <stdexcept>

#include "Grammar2CNF.h"

// Mensaje de ayuda
static const char* kUsage =
    "Uso: Grammar2CNF input.gra output.gra\n"
    "Opciones:\n"
    "  --help    Muestra este texto de ayuda.\n";

/**
 * @brief Función principal.
 *
 * Funciona de esta forma:
 *  1) Validar argumentos de línea de comandos.
 *  2) Leer la gramática desde el fichero input.gra.
 *  3) Validar formato y precondiciones (sin epsilon ni unitarias).
 *  4) Aplicar el Algoritmo 1 para convertir a FNC.
 *  5) Escribir la gramática resultante en output.gra.
 *
 * Códigos de salida:
 *  0 - ejecución correcta
 *  1 - uso incorrecto / argumentos
 *  2 - error durante la lectura/validación/transformación
 */
int main(int argc, char* argv[]) {
    // Comprobar argumentos
    try {
        if (argc == 2) {
            // Si se pasa un único argumento y es --help o -h se muestra ayuda
            std::string arg = argv[1];
            if (arg == "--help" || arg == "-h") {
                std::cout << kUsage;
                return 0;
            }
        }
        // Si no se han pasado 2 argumentos, mostrar uso y salir
        if (argc != 3) {
            std::cerr << "Modo de empleo: ./Grammar2CNF input.gra output.gra\n";
            std::cerr << "Pruebe 'Grammar2CNF --help' para más información.\n";
            return 1;
        }

        // Obtener nombres de ficheros de entrada y salida
        std::string input = argv[1]; // fichero .gra de entrada
        std::string output = argv[2]; // fichero .gra de salida

        // Crear gramática
        Grammar g;
        // Leer gramática desde fichero de entrada
        g.ReadFromFile(input);
        // Validar formato de la gramática
        g.ValidateFormat();
        // Comprobar precondiciones (sin producciones vacías ni unitarias)
        g.CheckPreconditions();
        // Aplicar el Algoritmo 1 para convertir a FNC
        g.TransformToCNF();
        // Escribir gramática resultante en fichero de salida
        g.WriteToFile(output);

        // Informar de que se ha completado la conversión correctamente
        std::cout << "Conversión completada. Fichero de salida: " << output << "\n";
        return 0;

    } catch (const std::exception& e) {
        // Capturar cualquier excepción lanzada y mostrar el mensaje de error
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }
}