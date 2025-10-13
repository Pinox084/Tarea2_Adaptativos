#include <iostream>
#include <string>
#include <vector>
#include <random>

#include "localsearchsolver/iterated_local_search.hpp"
#include "InstanciaMIS.hpp"
#include "EsquemaMIS.hpp"

// SOLUCIÓN: Añadir esta línea para usar el namespace de la biblioteca.
// Esto resuelve todos los errores de "not declared in this scope".
using namespace localsearchsolver;

// Ahora 'EsquemaMIS' se encuentra correctamente porque está dentro del namespace.
using my_scheme = EsquemaMIS;

void mostrar_uso(const std::string& prog_name) {
    std::cerr << "Uso: " << prog_name << " -i <archivo.graph> -t <tiempo_segundos> [opciones]\n"
              << "Opciones:\n"
              << "  --iter <n>     Número máximo de iteraciones (por defecto: -in, sin límite)\n"
              << "  --pert <n>     Número mínimo de perturbaciones por ciclo (por defecto: 1)\n";
}

int main(int argc, char* argv[])
{
    std::string filename = "";
    double time_limit = -1.0;
    // 'Counter' ahora es reconocido gracias al 'using namespace'.
    Counter max_iterations = -1;
    Counter min_perturbations = 1;

    std::vector<std::string> args(argv + 1, argv + argc);
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "-i" && i + 1 < args.size()) {
            filename = args[++i];
        } else if (args[i] == "-t" && i + 1 < args.size()) {
            time_limit = std::stod(args[++i]);
        } else if (args[i] == "--iter" && i + 1 < args.size()) {
            max_iterations = std::stoll(args[++i]);
        } else if (args[i] == "--pert" && i + 1 < args.size()) {
            min_perturbations = std::stoll(args[++i]);
        }
    }

    if (filename.empty() || time_limit < 0) {
        mostrar_uso(argv[0]);
        return 1;
    }

    std::cout << "Cargando instancia: " << filename << std::endl;
    Graph graph(filename);
    std::cout << "Número de vértices: " << graph.num_vertices << std::endl;

    my_scheme esquema(graph);

    IteratedLocalSearchParameters<my_scheme> params;
    params.timer.set_time_limit(time_limit);
    params.maximum_number_of_iterations = max_iterations;
    params.minimum_number_of_perturbations = min_perturbations;
    params.seed = 123456789ULL;
    params.maximum_number_of_restarts = -1;
    params.maximum_size_of_the_solution_pool = 1;

    std::cout << "\n=== Iniciando Búsqueda Iterada Local ===" << std::endl;
    auto output = iterated_local_search(esquema, params);

    std::cout << "\n=== Resultados ===" << std::endl;
    if (output.solution_pool.size() > 0) {
        const auto& best_sol = output.solution_pool.best();
        std::cout << "Mejor tamaño de conjunto independiente: " << best_sol.size << std::endl;
        std::cout << "Tiempo total de ejecución: " << params.timer.elapsed_time() << " segundos" << std::endl;

        std::cout << "\nVértices en la mejor solución:" << std::endl;
        bool first = true;
        for (int i = 0; i < graph.num_vertices; ++i) {
            if (best_sol.in_set[i]) {
                if (!first) std::cout << " ";
                std::cout << i;
                first = false;
            }
        }
        std::cout << std::endl;
    } else {
        std::cout << "No se encontró una solución." << std::endl;
    }

    return 0;
}