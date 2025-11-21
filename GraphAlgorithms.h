#ifndef GRAPHALGORITHMS_H
#define GRAPHALGORITHMS_H

#include "Graph.h"
#include <queue>
#include <stack>
#include <limits>
#include <unordered_map>
#include <set>

/**
 * @brief Абстрактний інтерфейс для алгоритмів обходу графа.
 * @tparam TNode Тип вузла графа.
 * @tparam TEdge Тип ребра графа.
 */
template <typename TNode, typename TEdge>
class GraphAlgorithm {
public:
    /**
     * @brief Чисто віртуальний метод для запуску алгоритму обходу.
     * @param g Граф, на якому виконується алгоритм.
     * @param start Початковий вузол обходу.
     */
    virtual void run(const Graph<TNode, TEdge>& g, const TNode& start) = 0;
    virtual ~GraphAlgorithm() = default;
};

/**
 * @brief Алгоритм обходу графа в ширину (Breadth-First Search, BFS).
 * @tparam TNode Тип вузла графа.
 * @tparam TEdge Тип ребра графа.
 */
template <typename TNode, typename TEdge>
class BFS : public GraphAlgorithm<TNode, TEdge> {
    /// @brief Множина для відстеження відвіданих вузлів.
    std::set<TNode> visited;
public:
    /**
     * @brief Запускає обхід графа в ширину, використовуючи чергу.
     * @param g Граф для обходу.
     * @param start Початковий вузол.
     */
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

/**
 * @brief Алгоритм обходу графа в глибину (Depth-First Search, DFS).
 * @tparam TNode Тип вузла графа.
 * @tparam TEdge Тип ребра графа.
 */
template <typename TNode, typename TEdge>
class DFS : public GraphAlgorithm<TNode, TEdge> {
    /// @brief Множина для відстеження відвіданих вузлів.
    std::set<TNode> visited;
public:
    /**
     * @brief Запускає ітеративний обхід графа в глибину, використовуючи стек.
     * @param g Граф для обходу.
     * @param start Початковий вузол.
     */
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

/**
 * @brief Структура, що представляє зважене ребро. Використовується алгоритмом Дейкстри.
 */
struct WeightedEdge {
    /// @brief Вага (ціна) ребра.
    double weight{0.0};
};

/**
 * @brief Алгоритм пошуку найкоротшого шляху (Дейкстри).
 *
 * Призначений для роботи з графами, що мають ребра типу WeightedEdge.
 *
 * @tparam TNode Тип вузла графа.
 */
template <typename TNode>
class Dijkstra {
public:
    /// @brief Мапа, що зберігає найкоротші відстані від start до всіх вузлів.
    std::map<TNode, double> dist;
    /// @brief Мапа, що зберігає батьківський вузол для відновлення шляху.
    std::map<TNode, TNode> parent;

    /**
     * @brief Виконує алгоритм Дейкстри для знаходження найкоротших шляхів від початкового вузла.
     * @param g Зважений граф (з ребрами типу WeightedEdge).
     * @param start Початковий вузол.
     */
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

    /**
     * @brief Відновлює найкоротший шлях від start до target.
     * @param start Початковий вузол.
     * @param target Цільовий вузол.
     * @return std::vector<TNode> Вектор вузлів, що формують шлях. Порожній, якщо шлях не знайдено.
     */
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