#pragma once
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <string>
#include "InstanciaMIS.hpp"

// CORRECTED: The entire scheme definition is moved into the solver's namespace
namespace localsearchsolver {

struct EsquemaMIS
{
    using Solution = InstanciaMIS::Solucion;
    using Perturbation = InstanciaMIS::Perturbacion;
    using GlobalCost = int;

    const Graph& graph;

    EsquemaMIS(const Graph& g) : graph(g) {}

    // Generates a greedy randomized initial solution
    Solution initial_solution(int, std::mt19937_64& generator)
    {
        Solution sol;
        sol.in_set.assign(graph.num_vertices, false);
        sol.size = 0;

        std::vector<int> order(graph.num_vertices);
        std::iota(order.begin(), order.end(), 0);
        std::shuffle(order.begin(), order.end(), generator);

        for (int v : order) {
            bool is_neighbor_in_set = false;
            for (int u : graph.adj_list[v]) {
                if (sol.in_set[u]) {
                    is_neighbor_in_set = true;
                    break;
                }
            }
            if (!is_neighbor_in_set) {
                sol.in_set[v] = true;
                sol.size++;
            }
        }
        return sol;
    }

    // The framework minimizes cost, so we return the negative of the size
    GlobalCost global_cost(const Solution& s) const
    {
        return -s.size;
    }

    Solution empty_solution() const
    {
        Solution s;
        s.in_set.assign(graph.num_vertices, false);
        s.size = 0;
        return s;
    }

    // A simple local search: greedily add any possible vertex
    void local_search(Solution& s, std::mt19937_64&)
    {
        bool improved = true;
        while (improved) {
            improved = false;
            for (int v = 0; v < graph.num_vertices; ++v) {
                if (!s.in_set[v]) { // If vertex is not in the set
                    bool can_add = true;
                    for (int u : graph.adj_list[v]) {
                        if (s.in_set[u]) {
                            can_add = false;
                            break;
                        }
                    }
                    if (can_add) {
                        s.in_set[v] = true;
                        s.size++;
                        improved = true; // Found an improvement, restart scan
                    }
                }
            }
        }
    }

    // Generate perturbations: each move consists of removing one vertex from the solution
    std::vector<Perturbation> perturbations(const Solution& s, std::mt19937_64&)
    {
        std::vector<Perturbation> moves;
        for (int v = 0; v < graph.num_vertices; ++v) {
            if (s.in_set[v]) {
                Perturbation p;
                p.vertex = v;
                p.add = false; 
                p.global_cost = -(s.size - 1); 
                moves.push_back(p);
            }
        }
        return moves;
    }

    // Apply the perturbation (remove the vertex)
    void apply_perturbation(Solution& s, const Perturbation& p, std::mt19937_64&)
    {
        if (!p.add && s.in_set[p.vertex]) {
            s.in_set[p.vertex] = false;
            s.size--;
        }
    }

    // After perturbing, run the same local search to repair the solution
    void local_search(Solution& s, std::mt19937_64& generator, const Perturbation&)
    {
        local_search(s, generator);
    }
};

// This function allows the framework to print the "cost" in a readable format.
// We convert our internal cost (-size) back to the actual size (a positive number).
inline std::string to_string(const EsquemaMIS&, const int& cost)
{
    return std::to_string(-cost);
}

} // namespace localsearchsolver