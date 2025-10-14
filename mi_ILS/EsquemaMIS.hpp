#pragma once
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <string>
#include <chrono> // Usamos la librería estándar de C++ para el tiempo
#include "InstanciaMIS.hpp"

namespace localsearchsolver {

struct EsquemaMIS
{
    using Solution = InstanciaMIS::Solucion;
    using Perturbation = InstanciaMIS::Perturbacion;
    using GlobalCost = int;

    const Graph& graph;
    double alpha = 0.3;
    int fuerza_perturbacion = 2;

    // --- Miembros de seguimiento ---
    // 'mutable' permite modificar estos valores desde funciones 'const' como global_cost
    mutable int best_cost_so_far = 0; // Guardamos el costo (negativo del tamaño)
    mutable double time_to_best_found = 0.0;
    mutable std::chrono::high_resolution_clock::time_point start_time;
    mutable bool is_chrono_started = false;

    EsquemaMIS(const Graph& g) : graph(g) {}

    // --- El "espía" en la función objetivo ---
    GlobalCost global_cost(const Solution& s) const
    {
        // Inicia el cronómetro en la primera llamada
        if (!is_chrono_started) {
            start_time = std::chrono::high_resolution_clock::now();
            is_chrono_started = true;
        }

        int current_cost = -s.size;

        // Comprueba si hemos encontrado un nuevo récord
        if (current_cost < best_cost_so_far) {
            best_cost_so_far = current_cost;
            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = now - start_time;
            time_to_best_found = elapsed.count();
        }
        return current_cost;
    }

    // --- Métodos para obtener los resultados ---
    int get_best_solution_size() const { return -best_cost_so_far; }
    double get_time_to_best() const { return time_to_best_found; }

    Solution empty_solution() const {
        Solution s;
        s.in_set.assign(graph.num_vertices, false);
        s.size = 0;
        return s;
    }

    Solution initial_solution(int, std::mt19937_64& generator) {
        Solution sol;
        sol.in_set.assign(graph.num_vertices, false);
        sol.size = 0;
        std::vector<bool> nodos_disponibles(graph.num_vertices, true);
        while (true) {
            std::vector<std::pair<int, int>> candidatos_con_grado;
            int min_grado = -1, max_grado = -1;
            for (int v = 0; v < graph.num_vertices; ++v) {
                if (nodos_disponibles[v]) {
                    int grado_actual = 0;
                    for (int u : graph.adj_list[v]) {
                        if (nodos_disponibles[u]) grado_actual++;
                    }
                    candidatos_con_grado.push_back({v, grado_actual});
                    if (min_grado == -1 || grado_actual < min_grado) min_grado = grado_actual;
                    if (max_grado == -1 || grado_actual > max_grado) max_grado = grado_actual;
                }
            }
            if (candidatos_con_grado.empty()) break;
            double umbral = min_grado + alpha * (max_grado - min_grado);
            std::vector<int> rcl;
            for (const auto& par : candidatos_con_grado) {
                if (par.second <= umbral) rcl.push_back(par.first);
            }
            if (rcl.empty() && !candidatos_con_grado.empty()) {
                 rcl.push_back(candidatos_con_grado[0].first);
            } else if (rcl.empty()){
                break;
            }
            std::uniform_int_distribution<int> dist(0, rcl.size() - 1);
            int nodo_elegido = rcl[dist(generator)];
            sol.in_set[nodo_elegido] = true;
            sol.size++;
            nodos_disponibles[nodo_elegido] = false;
            for (int vecino : graph.adj_list[nodo_elegido]) {
                nodos_disponibles[vecino] = false;
            }
        }
        return sol;
    }

    void local_search(Solution& s, std::mt19937_64&) {
        bool improved = true;
        while (improved) {
            improved = false;
            for (int v = 0; v < graph.num_vertices; ++v) {
                if (!s.in_set[v]) {
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
                        improved = true;
                    }
                }
            }
        }
    }

    void apply_perturbation(Solution& s, const Perturbation&, std::mt19937_64& generator) {
        std::vector<int> nodos_en_solucion;
        for (int i = 0; i < graph.num_vertices; ++i) {
            if (s.in_set[i]) nodos_en_solucion.push_back(i);
        }
        if (nodos_en_solucion.empty()) return;
        std::shuffle(nodos_en_solucion.begin(), nodos_en_solucion.end(), generator);
        for (int i = 0; i < fuerza_perturbacion && i < nodos_en_solucion.size(); ++i) {
            int nodo_a_quitar = nodos_en_solucion[i];
            if (s.in_set[nodo_a_quitar]) {
                s.in_set[nodo_a_quitar] = false;
                s.size--;
            }
        }
    }
    
    std::vector<Perturbation> perturbations(const Solution&, std::mt19937_64&) { return {}; }
    void local_search(Solution& s, std::mt19937_64& g, const Perturbation&) { local_search(s, g); }
};

inline std::string to_string(const EsquemaMIS&, const int& cost) { return std::to_string(-cost); }

}