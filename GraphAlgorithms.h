#ifndef GRAPHALGORITHMS_H
#define GRAPHALGORITHMS_H

/*
КЛАСИ/ТИПИ У ФАЙЛІ:
  2) template<class TNode, class TEdge> class GraphAlgorithm [КЛАС №2 - абстр.]
  3) template<class TNode, class TEdge> class BFS [КЛАС №3]
  4) template<class TNode, class TEdge> class DFS [КЛАС №4]
  5) struct WeightedEdge { double weight; } [КЛАС/СТРУКТ. №5]
  6) template<class TNode> class Dijkstra [КЛАС №6]

ПОЛЯ (сумарно в цьому файлі, приклади):
  - BFS: visited (std::set) - 1
  - DFS: visited (std::set) - 1
  - Dijkstra: dist (map), parent (map) - 2
  разом: 4

СПИСОК НЕТРИВІАЛЬНИХ МЕТОДІВ У ЦЬОМУ ФАЙЛІ:
  (М10) GraphAlgorithm::run(...) - абстрактний інтерфейс (описує поліморфізм)
  (М11) BFS::run - обхід у ширину з чергою та маркуванням відвіданих
  (М12) DFS::run - обхід у глибину зі стеком і маркуванням відвіданих
  (М13) Dijkstra::run - алгоритм Дейкстри з пріоритетною чергою (мін-купа)
  (М14) Dijkstra::getPathTo(target) - відновлення шляху за масивом батьків
  разом: 5

ПРИМІТКИ:
  - статичний поліморфізм: усі ці класи шаблонні (templates)
  - динамічний поліморфізм: можна зберігати вказівники на GraphAlgorithm<...> і викликати run()
*/

#include "Graph.h"
#include <queue>
#include <stack>
#include <limits>
#include <unordered_map>
#include <set>

// абстрактний інтерфейс алгоритму на графі (динамічний поліморфізм)
template <typename TNode, typename TEdge>
class GraphAlgorithm {
public:
    virtual void run(const Graph<TNode, TEdge>& g, const TNode& start) = 0; // (М10)
    virtual ~GraphAlgorithm() = default;
};

// BFS
template <typename TNode, typename TEdge>
class BFS : public GraphAlgorithm<TNode, TEdge> {
    std::set<TNode> visited; // поле для маркування
public:
    // (М11) класичний BFS: черга + відвідані
    void run(const Graph<TNode, TEdge>& g, const TNode& start) override {
        std::queue<TNode> q;
        visited.clear();
        q.push(start);
        visited.insert(start);

        std::cout << "BFS: ";
        while (!q.empty()) {
            TNode node = q.front(); q.pop();
            std::cout << node << " ";
            for (auto& neighbor : g.getNeighbors(node)) {
                if (!visited.count(neighbor)) {
                    visited.insert(neighbor);
                    q.push(neighbor);
                }
            }
        }
        std::cout << "\n";
    }
};

// DFS
template <typename TNode, typename TEdge>
class DFS : public GraphAlgorithm<TNode, TEdge> {
    std::set<TNode> visited; // поле для маркування
public:
    // (М12) ітеративний DFS: стек + відвідані
    void run(const Graph<TNode, TEdge>& g, const TNode& start) override {
        std::stack<TNode> st;
        visited.clear();
        st.push(start);

        std::cout << "DFS: ";
        while (!st.empty()) {
            TNode node = st.top(); st.pop();
            if (!visited.count(node)) {
                std::cout << node << " ";
                visited.insert(node);
                for (auto& neighbor : g.getNeighbors(node))
                    st.push(neighbor);
            }
        }
        std::cout << "\n";
    }
};

// вага для зважених графів (викор. у Дейкстрі)
struct WeightedEdge {
    double weight{0.0};
};

// Dijkstra (на графі з ребрами WeightedEdge)
template <typename TNode>
class Dijkstra {
public:
    std::map<TNode, double> dist; // найкоротші відстані
    std::map<TNode, TNode> parent; // батьки для відновлення шляху

    // (М13) алгоритм Дейкстри: мін-купа, релаксація ребер
    void run(const Graph<TNode, WeightedEdge>& g, const TNode& start) {
        // Ініціалізація
        dist.clear(); parent.clear();
        for (auto& [u, _] : g.data()) dist[u] = std::numeric_limits<double>::infinity();
        if (!g.hasNode(start)) return;
        dist[start] = 0.0;

        // мін-купа (черга з парою (distance, node))
        using QItem = std::pair<double, TNode>;
        std::priority_queue<QItem, std::vector<QItem>, std::greater<QItem>> pq;
        pq.push({0.0, start});

        while (!pq.empty()) {
            auto [du, u] = pq.top(); pq.pop();
            if (du != dist[u]) continue; // пропускаємо застарілі значення

            // перебираємо сусідів
            auto it = g.data().find(u);
            if (it == g.data().end()) continue;
            for (auto& [v, w] : it->second) {
                double nd = du + w.weight;
                if (nd < dist[v]) { // релаксація
                    dist[v] = nd;
                    parent[v] = u;
                    pq.push({nd, v});
                }
            }
        }
    }

    // (М14) відновлення шляху до target (якщо існує)
    std::vector<TNode> getPathTo(const TNode& start, const TNode& target) const {
        std::vector<TNode> path;
        if (!dist.count(target) || dist.at(target) == std::numeric_limits<double>::infinity())
            return path; // пусто: шляху немає
        TNode cur = target;
        while (cur != start) {
            path.push_back(cur);
            auto it = parent.find(cur);
            if (it == parent.end()) { path.clear(); return path; }
            cur = it->second;
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());
        return path;
    }
};

#endif //GRAPHALGORITHMS_H