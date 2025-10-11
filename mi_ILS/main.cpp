#include <random>
#include <iostream>
#include "localsearchsolver/iterated_local_search.hpp"

// Mock de LocalScheme para ejemplo
struct DummyScheme {
    using Solution = int;
    using Perturbation = struct { int global_cost; };
    using GlobalCost = int;

    int global_cost(const Solution& s) const { return s; }
    Solution initial_solution(int, std::mt19937_64&) const { return 0; }
    void local_search(Solution&, std::mt19937_64&) const {}
    void local_search(Solution&, std::mt19937_64&, const Perturbation&) const {}
    std::vector<Perturbation> perturbations(const Solution&, std::mt19937_64&) const {
        return { {1}, {2}, {3} };
    }
    void apply_perturbation(Solution& s, const Perturbation& p, std::mt19937_64&) const { s += p.global_cost; }

    // Métodos requeridos por el framework:
    Solution empty_solution() const { return 0; }
    std::string to_string(const GlobalCost& gc) const { return std::to_string(gc); }
};

int main() {
    std::cout << "Ejecutando ejemplo de Iterated Local Search..." << std::endl;

    DummyScheme scheme;
    localsearchsolver::IteratedLocalSearchParameters<DummyScheme> params;
    params.maximum_number_of_iterations = 5;
    params.maximum_number_of_restarts = 2;

    auto output = localsearchsolver::iterated_local_search(scheme, params);

    std::cout << "Número de iteraciones: " << output.number_of_iterations << std::endl;
    std::cout << "Número de reinicios: " << output.number_of_restarts << std::endl;

    return 0;
}