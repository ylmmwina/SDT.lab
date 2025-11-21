#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

/**
 * @brief Шаблонний клас, що представляє граф зі списком суміжності.
 *
 * Граф може бути орієнтованим або неорієнтованим. Використовує std::map для зберігання
 * вершин та їхніх сусідів.
 *
 * @tparam TNode Тип даних для вершин (наприклад, int, std::string).
 * @tparam TEdge Тип даних для даних/ваги ребер.
 */
template <typename TNode, typename TEdge>
class Graph {
private:
    /// @brief Список суміжності: для кожної вершини зберігаємо вектор пар (сусід, дані ребра).
    std::map<TNode, std::vector<std::pair<TNode, TEdge>>> adjacency;
    /// @brief Прапор, що вказує, чи є граф орієнтованим.
    bool directed_ = true;

public:
    /**
     * @brief Конструктор графа.
     * @param directed Якщо true (за замовчуванням) — граф орієнтований, інакше — неорієнтований.
     */
    explicit Graph(bool directed = true) : directed_(directed) {}

    /**
     * @brief Додає вершину до графа, якщо вона ще не існує.
     * @param node Вершина, яку потрібно додати.
     */
    void addNode(const TNode& node) {
        adjacency[node];
    }

    /**
     * @brief Додає ребро між вершинами з даними edge.
     *
     * Якщо граф неорієнтований, додається також дзеркальне ребро (v -> u).
     *
     * @param from Вершина-джерело.
     * @param to Вершина-призначення.
     * @param edge Дані/вага ребра.
     */
    void addEdge(const TNode& from, const TNode& to, const TEdge& edge) {
        addNode(from);
        addNode(to);
        adjacency[from].push_back({to, edge});
        if (!directed_) {
            adjacency[to].push_back({from, edge});
        }
    }

    /**
     * @brief Видаляє вершину з графа разом з усіма вхідними та вихідними ребрами.
     * @param node Вершина, яку потрібно видалити.
     */
    void removeNode(const TNode& node) {
        adjacency.erase(node);
        for (auto& [u, neighbors] : adjacency) {
            neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(),
                [&](auto& pair){ return pair.first == node; }), neighbors.end());
        }
    }

    /**
     * @brief Видаляє ребро (u -> v). Для неорієнтованого графа видаляє також (v -> u).
     * @param from Вершина-джерело.
     * @param to Вершина-призначення.
     */
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

    /**
     * @brief Повертає вектор суміжних вершин (сусідів) для заданої вершини.
     * @param node Вершина, сусідів якої потрібно знайти.
     * @return std::vector<TNode> Вектор вершин-сусідів. Порожній, якщо вершини немає.
     */
    std::vector<TNode> getNeighbors(const TNode& node) const {
        std::vector<TNode> result;
        if (auto it = adjacency.find(node); it != adjacency.end()) {
            result.reserve(it->second.size());
            for (auto& pair : it->second) result.push_back(pair.first);
        }
        return result;
    }

    /**
     * @brief Друкує текстове представлення графа у стандартний потік.
     */
    void printGraph() const {
        for (auto& [node, neighbors] : adjacency) {
            std::cout << node << " -> ";
            for (auto& [n, e] : neighbors) {
                std::cout << "(" << n << ", edge=" << e << ") ";
            }
            std::cout << "\n";
        }
    }

    /**
     * @brief Очищає весь граф, видаляючи всі вершини та ребра.
     */
    void clear() { adjacency.clear(); }

    /**
     * @brief Повертає кількість вершин у графі.
     * @return std::size_t Кількість вершин.
     */
    std::size_t size() const { return adjacency.size(); }

    /**
     * @brief Перевіряє наявність заданої вершини в графі.
     * @param node Вершина, яку потрібно перевірити.
     * @return bool true, якщо вершина існує, інакше false.
     */
    bool hasNode(const TNode& node) const { return adjacency.count(node) != 0; }

    /// @brief Повертає константне посилання на внутрішні дані графа.
    const std::map<TNode, std::vector<std::pair<TNode, TEdge>>>& data() const {
        return adjacency;
    }
};

#endif // GRAPH_H