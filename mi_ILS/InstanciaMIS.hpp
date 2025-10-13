#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <iostream>

struct Graph {
    int num_vertices;
    std::vector<std::vector<int>> adj_list;

    Graph(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open())
            throw std::runtime_error("No se pudo abrir el archivo: " + filename);

        file >> num_vertices;
        adj_list.assign(num_vertices, {});

        int u, v;
        while (file >> u >> v) {
            if (u >= num_vertices || v >= num_vertices || u < 0 || v < 0) continue;
            adj_list[u].push_back(v);
            adj_list[v].push_back(u);
        }
    }

    bool son_adyacentes(int u, int v) const {
        for (int x : adj_list[u])
            if (x == v) return true;
        return false;
    }

    int n() const { return num_vertices; }
};

struct InstanciaMIS {
    const Graph& graph;

    InstanciaMIS(const Graph& g) : graph(g) {}

    struct Solucion {
        std::vector<bool> in_set;
        int size = 0;

        void inicializar(int n) {
            in_set.assign(n, false);
            size = 0;
        }
    };

    struct Perturbacion {
        int vertex = -1;
        bool add = false;
        int global_cost = 0;
    };

    bool puede_agregar(const Solucion& s, int v) const {
        if (s.in_set[v]) return false;
        for (int u : graph.adj_list[v])
            if (s.in_set[u]) return false;
        return true;
    }

    bool agregar(Solucion& s, int v) const {
        if (!puede_agregar(s, v)) return false;
        s.in_set[v] = true;
        s.size++;
        return true;
    }

    bool quitar(Solucion& s, int v) const {
        if (!s.in_set[v]) return false;
        s.in_set[v] = false;
        s.size--;
        return true;
    }

    int evaluar(const Solucion& s) const {
        return s.size;
    }

    bool es_valida(const Solucion& s) const {
        for (int v = 0; v < graph.num_vertices; ++v) {
            if (!s.in_set[v]) continue;
            for (int u : graph.adj_list[v])
                if (s.in_set[u]) return false;
        }
        return true;
    }
};