#ifndef GRAPH_H
#define GRAPH_H

/*
КЛАСИ/ТИПИ У ФАЙЛІ:
  1) template<class TNode, class TEdge> class Graph [КЛАС №1]

ПОЛЯ (сумарно у цьому файлі):
  - adjacency (std::map<TNode, std::vector<std::pair<TNode, TEdge>>>) - 1
  - directed_ (bool) - 1
  разом у файлі: 2

СПИСОК НЕТРИВІАЛЬНИХ МЕТОДІВ У ЦЬОМУ ФАЙЛІ (рахунок + пояснення):
  (М1) addNode - додає вершину; створює порожній список суміжності
  (М2) addEdge - додає орієнтоване/неорієнтоване ребро
  (М3) removeNode - видаляє вершину та всі ребра, що на неї вказують
  (М4) removeEdge - видаляє (u→v) і, для неорієнтованого, (v→u)
  (М5) getNeighbors - повертає копію списку суміжних вершин
  (М6) printGraph - друкує читабельне текстове уявлення графа
  (М7) clear - очищає граф
  (М8) size - кількість вершин (|V|)
  (М9) hasNode - перевірка наявності вершини
  разом у файлі: 9

ПРИМІТКИ ПРО ІНКАПСУЛЯЦІЮ:
  - поля приватні
  - доступ лише через публічні методи
*/

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

template <typename TNode, typename TEdge>
class Graph {
private:
    // список суміжності: для кожної вершини зберігаємо вектор пар (сусід, дані ребра)
    std::map<TNode, std::vector<std::pair<TNode, TEdge>>> adjacency;
    bool directed_ = true;

public:
    explicit Graph(bool directed = true) : directed_(directed) {}

    // (М1) додає вершину: створює запис у мапі, якщо його не було
    void addNode(const TNode& node) {
        adjacency[node]; // створює порожній вектор для node, якщо його ще не існує
    }

    // (М2) додає ребро (u -> v) з даними edge; якщо граф неорієнтований, додаємо дзеркальне ребро (v -> u)
    void addEdge(const TNode& from, const TNode& to, const TEdge& edge) {
        addNode(from);
        addNode(to);
        adjacency[from].push_back({to, edge});
        if (!directed_) {
            adjacency[to].push_back({from, edge});
        }
    }

    // (М3) видаляє вершину й усі ребра, що на неї вказують
    void removeNode(const TNode& node) {
        adjacency.erase(node);
        for (auto& [u, neighbors] : adjacency) {
            neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(),
                [&](auto& pair){ return pair.first == node; }), neighbors.end());
        }
    }

    // (М4) видаляє ребро (u -> v); для неорієнтованого графа — також (v -> u)
    void removeEdge(const TNode& from, const TNode& to) {
        if (auto it = adjacency.find(from); it != adjacency.end()) {
            auto& neighbors = it->second;
            neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(),
                [&](auto& p){ return p.first == to; }), neighbors.end());
        }
        if (!directed_) {
            if (auto jt = adjacency.find(to); jt != adjacency.end()) {
                auto& rev = jt->second;
                rev.erase(std::remove_if(rev.begin(), rev.end(),
                    [&](auto& p){ return p.first == from; }), rev.end());
            }
        }
    }

    // (М5) повертає список суміжних вершин (копія)
    std::vector<TNode> getNeighbors(const TNode& node) const {
        std::vector<TNode> result;
        if (auto it = adjacency.find(node); it != adjacency.end()) {
            result.reserve(it->second.size());
            for (auto& pair : it->second) result.push_back(pair.first);
        }
        return result;
    }

    // (М6) друк графа у вигляді: U -> (V, edge=...) ...
    void printGraph() const {
        for (auto& [node, neighbors] : adjacency) {
            std::cout << node << " -> ";
            for (auto& [n, e] : neighbors) {
                std::cout << "(" << n << ", edge=" << e << ") ";
            }
            std::cout << "\n";
        }
    }

    // (М7) очистити граф
    void clear() { adjacency.clear(); }

    // (М8) кількість вершин
    std::size_t size() const { return adjacency.size(); }

    // (М9) чи існує вершина
    bool hasNode(const TNode& node) const { return adjacency.count(node) != 0; }

    // доступ до сирих даних (тільки читання)
    const std::map<TNode, std::vector<std::pair<TNode, TEdge>>>& data() const {
        return adjacency;
    }
};

#endif // GRAPH_H