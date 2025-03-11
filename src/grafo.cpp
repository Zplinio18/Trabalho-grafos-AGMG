#include "../include/grafo.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <algorithm>

/**
 * @file grafo.cpp
 * @brief Implementação da classe grafo.
 */

grafo::grafo() {}

struct ArestaInfo {
    int origem;
    int destino;
    int peso;
};

bool compararArestas(const ArestaInfo& a, const ArestaInfo& b) {
    return a.peso < b.peso;
}

/**
 * @brief Constroi o grafo a partir de um arquivo.
 * @param arquivo O caminho para o arquivo contendo a descrição do grafo.
 */
void grafo::carrega_grafo(const std::string& arquivo) {
    std::ifstream file(arquivo);
    if (!file.is_open()) throw std::runtime_error("Arquivo não encontrado");

    int num_nos, dir, pond_vertices, pond_arestas;
    file >> num_nos >> dir >> pond_vertices >> pond_arestas;

    this->direcionado = dir;
    this->ponderado_vertices = pond_vertices;
    this->ponderado_arestas = pond_arestas;
    this->num_nos = num_nos;

    if (ponderado_vertices) {
        for (int i = 1; i <= num_nos; ++i) {
            int peso;
            file >> peso;
            add_no(i, peso);
        }
    } else {
        for (int i = 1; i <= num_nos; ++i) {
            add_no(i, 0);
        }
    }

    int origem, destino, peso = 0;
    while (file >> origem >> destino) {
        if (ponderado_arestas) file >> peso;
        add_aresta(origem, destino, peso);
    }
}

/**
 * @brief Verifica se o grafo é completo.
 * @return true se o grafo é completo, false caso contrário.
 */
bool grafo::eh_completo() {
    int n = get_ordem();
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (i != j && !existe_aresta(i, j)) {
                if (direcionado) return false;
                if (!existe_aresta(j, i)) return false;
            }
        }
    }
    return true;
}

/**
 * @brief Libera a memória alocada para as arestas temporárias.
 * @param cabeca O ponteiro para a primeira aresta.
 */
void liberar_arestas_temp(aresta_grafo* cabeca) {
    while (cabeca) {
        aresta_grafo* temp = cabeca;
        cabeca = cabeca->proxima;
        delete temp;
    }
}

/**
 * @brief Retorna o grau do grafo.
 * @return O grau do grafo.
 */
int grafo::get_grau() {
    int grau_maximo = 0;
    int n = get_ordem();
    for (int i = 1; i <= n; ++i) {
        int grau_atual = 0;
        aresta_grafo* vizinhos = get_vizinhos(i);
        aresta_grafo* atual = vizinhos;
        while (atual) {
            grau_atual++;
            atual = atual->proxima;
        }
        liberar_arestas_temp(vizinhos);

        if (direcionado) {
            for (int j = 1; j <= n; ++j) {
                if (existe_aresta(j, i)) grau_atual++;
            }
        }

        if (grau_atual > grau_maximo) grau_maximo = grau_atual;
    }
    return grau_maximo;
}

/**
 * @brief Algoritmo guloso para AGMG (Kruskal modificado).
 * @param num_arestas Ponteiro para armazenar o número de arestas na AGMG.
 * @return Array de inteiros representando as arestas selecionadas (origem, destino, peso).
 */
int* grafo::agmg_gulosa(int* num_arestas) {
    int n = get_ordem();
    int* pesos_vertices = new int[n + 1];
    for (int i = 1; i <= n; ++i) {
        no_grafo* no = get_no(i);
        if (!no) {
            delete[] pesos_vertices;
            *num_arestas = 0;
            return nullptr;
        }
        pesos_vertices[i] = no->peso;
    }

    ArestaInfo* arestas = nullptr;
    int contador = 0;
    int capacidade = 0;

    for (int u = 1; u <= n; ++u) {
        aresta_grafo* vizinhos = get_vizinhos(u);
        aresta_grafo* atual = vizinhos;
        while (atual) {
            if (!direcionado && u > atual->destino) {
                atual = atual->proxima;
                continue;
            }

            if (contador >= capacidade) {
                int nova_capacidade = (capacidade == 0) ? 1 : capacidade * 2;
                ArestaInfo* nova_arestas = new ArestaInfo[nova_capacidade];
                for (int i = 0; i < contador; ++i) {
                    nova_arestas[i] = arestas[i];
                }
                delete[] arestas;
                arestas = nova_arestas;
                capacidade = nova_capacidade;
            }

            arestas[contador++] = {u, atual->destino, atual->peso};
            atual = atual->proxima;
        }
        liberar_arestas_temp(vizinhos);
    }

    std::sort(arestas, arestas + contador, compararArestas);

    int* cluster_id = new int[n + 1];
    int num_clusters = 0;
    int* cluster_map = new int[n + 1]{0};

    for (int i = 1; i <= n; ++i) {
        bool encontrado = false;
        for (int j = 1; j < i; ++j) {
            if (pesos_vertices[i] == pesos_vertices[j]) {
                cluster_id[i] = cluster_id[j];
                encontrado = true;
                break;
            }
        }
        if (!encontrado) {
            cluster_id[i] = num_clusters++;
        }
    }

    int* pai = new int[num_clusters];
    int* rank = new int[num_clusters];
    for (int i = 0; i < num_clusters; ++i) {
        pai[i] = i;
        rank[i] = 0;
    }

    auto find = [&](int u) {
        while (pai[u] != u) {
            pai[u] = pai[pai[u]];
            u = pai[u];
        }
        return u;
    };

    auto unite = [&](int u, int v) {
        u = find(u);
        v = find(v);
        if (u != v) {
            if (rank[u] < rank[v]) std::swap(u, v);
            pai[v] = u;
            if (rank[u] == rank[v]) rank[u]++;
        }
    };

    int* resultado = new int[3 * (num_clusters - 1)];
    *num_arestas = 0;
    int idx = 0;

    for (int i = 0; i < contador; ++i) {
        int u = arestas[i].origem;
        int v = arestas[i].destino;
        int c_u = cluster_id[u];
        int c_v = cluster_id[v];

        if (find(c_u) != find(c_v)) {
            resultado[idx++] = u;
            resultado[idx++] = v;
            resultado[idx++] = arestas[i].peso;
            (*num_arestas)++;
            unite(c_u, c_v);
            if (*num_arestas == num_clusters - 1) break;
        }
    }

    delete[] arestas;
    delete[] pesos_vertices;
    delete[] cluster_id;
    delete[] cluster_map;
    delete[] pai;
    delete[] rank;

    return resultado;
}

/**
 * @brief Algoritmo randomizado para AGMG.
 * @param num_arestas Ponteiro para armazenar o número de arestas na AGMG.
 * @param alpha Parâmetro de aleatoriedade (0-1).
 * @return Array de inteiros representando as arestas selecionadas.
 */
int* grafo::agmg_randomizada(int* num_arestas, double alpha = 0.5) {
    int n = get_ordem();
    int* pesos_vertices = new int[n + 1];
    for (int i = 1; i <= n; ++i) {
        no_grafo* no = get_no(i);
        pesos_vertices[i] = no->peso;
    }

    ArestaInfo* arestas = nullptr;
    int contador = 0;
    int capacidade = 0;

    for (int u = 1; u <= n; ++u) {
        aresta_grafo* vizinhos = get_vizinhos(u);
        aresta_grafo* atual = vizinhos;
        while (atual) {
            int v = atual->destino;
            if (!direcionado && u > v) {
                atual = atual->proxima;
                continue;
            }
            if (contador >= capacidade) {
                capacidade = (capacidade == 0) ? 1 : capacidade * 2;
                ArestaInfo* nova = new ArestaInfo[capacidade];
                for (int i = 0; i < contador; ++i) nova[i] = arestas[i];
                delete[] arestas;
                arestas = nova;
            }
            arestas[contador++] = {u, v, atual->peso};
            atual = atual->proxima;
        }
        liberar_arestas_temp(vizinhos);
    }

    int k = std::max(1, (int)(contador * alpha));
    for (int i = 0; i < k; ++i) {
        int r = i + rand() % (contador - i);
        std::swap(arestas[i], arestas[r]);
    }

    int* cluster_id = new int[n + 1];
    int num_clusters = 0;
    int* cluster_map = new int[n + 1]{0};

    for (int i = 1; i <= n; ++i) {
        bool encontrado = false;
        for (int j = 1; j < i; ++j) {
            if (pesos_vertices[i] == pesos_vertices[j]) {
                cluster_id[i] = cluster_id[j];
                encontrado = true;
                break;
            }
        }
        if (!encontrado) {
            cluster_id[i] = num_clusters++;
        }
    }

    int* pai = new int[num_clusters];
    int* rank = new int[num_clusters];
    for (int i = 0; i < num_clusters; ++i) {
        pai[i] = i;
        rank[i] = 0;
    }

    auto find = [&](int u) {
        while (pai[u] != u) {
            pai[u] = pai[pai[u]];
            u = pai[u];
        }
        return u;
    };

    auto unite = [&](int u, int v) {
        u = find(u);
        v = find(v);
        if (u != v) {
            if (rank[u] < rank[v]) std::swap(u, v);
            pai[v] = u;
            if (rank[u] == rank[v]) rank[u]++;
        }
    };

    int* resultado = new int[3 * (num_clusters - 1)];
    *num_arestas = 0;
    int idx = 0;

    for (int i = 0; i < contador; ++i) {
        int u = arestas[i].origem;
        int v = arestas[i].destino;
        int c_u = cluster_id[u];
        int c_v = cluster_id[v];

        if (find(c_u) != find(c_v)) {
            resultado[idx++] = u;
            resultado[idx++] = v;
            resultado[idx++] = arestas[i].peso;
            (*num_arestas)++;
            unite(c_u, c_v);
            if (*num_arestas == num_clusters - 1) break;
        }
    }

    delete[] arestas;
    delete[] pesos_vertices;
    delete[] cluster_id;
    delete[] cluster_map;
    delete[] pai;
    delete[] rank;

    return resultado;
}

/**
 * @brief Algoritmo reativo para AGMG.
 * @param num_arestas Ponteiro para armazenar o número de arestas.
 * @return Array de inteiros com as arestas selecionadas.
 */
int* grafo::agmg_reativa(int* num_arestas) {
    const int MAX_ITER = 50;
    const double INITIAL_PROB = 0.5;
    double prob_guloso = INITIAL_PROB;
    int* melhor = nullptr;
    int menor_custo = INT_MAX;
    int falhas_guloso = 0, falhas_random = 0;

    for (int iter = 0; iter < MAX_ITER; ++iter) {
        int* solucao;
        int temp_size;
        bool usar_guloso = ((double)rand() / RAND_MAX < prob_guloso);

        if (usar_guloso) {
            solucao = agmg_gulosa(&temp_size);
        } else {
            solucao = agmg_randomizada(&temp_size);
        }

        int custo = 0;
        for (int i = 2; i < temp_size * 3; i += 3) {
            custo += solucao[i];
        }

        if (custo < menor_custo) {
            delete[] melhor;
            menor_custo = custo;
            melhor = new int[temp_size * 3];
            for (int i = 0; i < temp_size * 3; ++i) {
                melhor[i] = solucao[i];
            }
            *num_arestas = temp_size;
        } else {
            if (usar_guloso) falhas_guloso++;
            else falhas_random++;
        }

        if (falhas_guloso + falhas_random > 0) {
            prob_guloso = 1.0 - ((double)falhas_guloso / (falhas_guloso + falhas_random));
        }

        delete[] solucao;
    }

    return melhor;
}


/**
 * @brief Exibe a descrição do grafo.
 */
void grafo::exibe_descricao() {
    std::cout << "Grau: " << get_grau() << std::endl;
    std::cout << "Ordem: " << get_ordem() << std::endl;
    std::cout << "Direcionado: " << (eh_direcionado() ? "Sim" : "Nao") << std::endl;
    std::cout << "Vertices ponderados: " << (vertice_ponderado() ? "Sim" : "Nao") << std::endl;
    std::cout << "Arestas ponderadas: " << (aresta_ponderada() ? "Sim" : "Nao") << std::endl;
    std::cout << "Completo: " << (eh_completo() ? "Sim" : "Nao") << std::endl;
}

/**
 * @brief Retorna as flags: direcionado, ponderado_vertices e ponderado_arestas.
 */
bool grafo::eh_direcionado() const { return direcionado; }
bool grafo::vertice_ponderado() const { return ponderado_vertices; }
bool grafo::aresta_ponderada() const { return ponderado_arestas; }
