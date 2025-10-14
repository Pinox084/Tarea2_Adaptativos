#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include "localsearchsolver/iterated_local_search.hpp"
#include "InstanciaMIS.hpp"
#include "EsquemaMIS.hpp"

using namespace localsearchsolver;
using my_scheme = EsquemaMIS;

void mostrar_uso(const std::string& prog_name) {
    std::cerr << "Uso: " << prog_name << " -i <archivo.graph> -t <tiempo_segundos> [opciones]\n"
              << "Opciones:\n"
              << "  --alpha <valor>  Aleatoriedad para GRASP (0.0 a 1.0, def: 0.3)\n"
              << "  --fuerza <n>   Fuerza de la perturbacion (nodos a quitar, def: 2)\n"
              << "  --iter <n>     Numero maximo de iteraciones (def: sin limite)\n"
              << "  --pert <n>     Numero minimo de perturbaciones por ciclo (def: 1)\n";
}

int main(int argc, char* argv[]) {
    std::string filename = "";
    double time_limit = -1.0;
    double alpha_param = 0.3;
    int fuerza_param = 2;
    Counter max_iterations = -1;
    Counter min_perturbations = 1;

    std::vector<std::string> args(argv + 1, argv + argc);
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "-i" && i + 1 < args.size()) {
            filename = args[++i];
        } else if (args[i] == "-t" && i + 1 < args.size()) {
            time_limit = std::stod(args[++i]);
        } else if (args[i] == "--alpha" && i + 1 < args.size()) {
            alpha_param = std::stod(args[++i]);
        } else if (args[i] == "--fuerza" && i + 1 < args.size()) {
            fuerza_param = std::stoi(args[++i]);
        } else if (args[i] == "--iter" && i + 1 < args.size()) {
            max_iterations = std::stoll(args[++i]);
        } else if (args[i] == "--pert" && i + 1 < args.size()) {
            min_perturbations = std::stoll(args[++i]);
        }
    }

    if (filename.empty() || time_limit <= 0) {
        mostrar_uso(argv[0]);
        return 1;
    }
    
    auto total_time_start = std::chrono::high_resolution_clock::now();
    
    Graph graph(filename);
    my_scheme esquema(graph);
    esquema.alpha = alpha_param;
    esquema.fuerza_perturbacion = fuerza_param;

    IteratedLocalSearchParameters<my_scheme> params;
    params.timer.set_time_limit(time_limit);
    params.maximum_number_of_iterations = max_iterations;
    params.minimum_number_of_perturbations = min_perturbations;
    params.seed = std::random_device{}();
    params.verbosity_level = 1; // Salida silenciosa para un parseo limpio

    iterated_local_search(esquema, params);

    auto total_time_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> total_elapsed = total_time_end - total_time_start;

    if (esquema.get_best_solution_size() > 0) {
        std::cout << "FINAL_STATS: "
                  << esquema.get_best_solution_size() << ","
                  << total_elapsed.count() << ","
                  << esquema.get_time_to_best()
                  << std::endl;
    } else {
        std::cout << "FINAL_STATS: 0,0,0" << std::endl;
    }

    return 0;
}