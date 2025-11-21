#ifndef NETWORKSIMULATOR_H
#define NETWORKSIMULATOR_H

#include "Graph.h"
#include "GraphAlgorithms.h" // Dijkstra + WeightedEdge
#include "Network.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>


/**
 * @brief Абстрактний інтерфейс для алгоритмів маршрутизації.
 */
class RoutingAlgorithm {
public:
    virtual ~RoutingAlgorithm() = default;

    /**
     * @brief Знаходить послідовність вузлів (шлях) для передачі пакета.
     * @param g Граф мережі (з ребрами типу Link).
     * @param src Ім'я початкового вузла.
     * @param dst Ім'я цільового вузла.
     * @param payloadBytes Розмір пакета, що впливає на вагу ребра.
     * @return std::vector<std::string> Послідовність імен вузлів, що формують найкоротший шлях.
     */
    virtual std::vector<std::string> route(
        const Graph<std::string, Link>& g,
        const std::string& src,
        const std::string& dst,
        std::size_t payloadBytes) = 0;
};

/**
 * @brief Реалізація алгоритму маршрутизації на основі Дейкстри.
 */
class DijkstraRouting : public RoutingAlgorithm {
private:
    /**
     * @brief Службовий метод: обчислює вагу ребра для Дейкстри на основі Link та розміру пакета.
     * @param link Об'єкт Link (властивості каналу).
     * @param payloadBytes Розмір пакета, що впливає на час передачі.
     * @return WeightedEdge Ребро з вагою (час у секундах).
     */
    static WeightedEdge linkToWeightedEdge(const Link& link, std::size_t payloadBytes) {
        double cost = link.costForBytes(payloadBytes);
        return WeightedEdge{cost};
    }

public:
    /**
     * @brief Реалізація пошуку шляху за Дейкстрою. Будує тимчасовий зважений граф.
     * @param g Граф мережі.
     * @param src Ім'я початкового вузла.
     * @param dst Ім'я цільового вузла.
     * @param payloadBytes Розмір пакета.
     * @return std::vector<std::string> Найкоротший шлях.
     */
    std::vector<std::string> route(
        const Graph<std::string, Link>& g,
        const std::string& src,
        const std::string& dst,
        std::size_t payloadBytes) override
    {
        // ... (Тіло методу, як було раніше)
        Graph<std::string, WeightedEdge> wg(true);
        for (auto& [u, _] : g.data()) wg.addNode(u);

        for (auto& [u, vec] : g.data()) {
            for (auto& [v, link] : vec) {
                WeightedEdge w_edge = linkToWeightedEdge(link, payloadBytes);
                wg.addEdge(u, v, w_edge);
            }
        }

        Dijkstra<std::string> dj;
        dj.run(wg, src);
        return dj.getPathTo(src, dst);
    }
};

/**
 * @brief Основний клас-симулятор, що містить топологію та управляє пристроями.
 */
class NetworkSimulator {
private:
    /// @brief Граф топології, де вершини — імена пристроїв, ребра — Link.
    Graph<std::string, Link>      graph_;
    /// @brief Мапа зареєстрованих пристроїв.
    std::map<std::string, Device*> devices_;

public:
    // ... (Деструктор)

    /**
     * @brief Реєструє новий пристрій у мережі та додає його ім'я як вузол до графа.
     * @param d Вказівник на об'єкт Device.
     * @throws std::runtime_error якщо вказівник d є nullptr.
     */
    void addDevice(Device* d) {
        if (!d) throw std::runtime_error("Null device");
        devices_[d->name()] = d;
        graph_.addNode(d->name());
    }

    /**
     * @brief З'єднує два вузли каналом зв'язку.
     * @param a Ім'я першого вузла.
     * @param b Ім'я другого вузла.
     * @param link Властивості каналу зв'язку.
     * @param bidir Якщо true (за замовчуванням) — створюється двосторонній зв'язок.
     * @throws std::runtime_error якщо вузол 'a' або 'b' не зареєстрований.
     */
    void connect(const std::string& a, const std::string& b, const Link& link, bool bidir = true) {
        if (!graph_.hasNode(a) || !graph_.hasNode(b))
            throw std::runtime_error("Unknown node in connect()");
        graph_.addEdge(a, b, link);
        if (bidir) graph_.addEdge(b, a, link);
    }

    // ... (інші методи)

    /**
     * @brief Симулює відправку пакета по заданому маршруту.
     * @param path Послідовність вузлів (шлях).
     * @param pkt Пакет, який потрібно відправити (TTL і Hops будуть змінені).
     * @return double Загальний час передачі пакета в секундах.
     */
    double sendPacket(const std::vector<std::string>& path, Packet& pkt) const {
        // ... (Тіло методу, як було раніше)
        if (path.size() < 2) return 0.0;
        double totalSeconds = 0.0;
        pkt.addHop(path.front());

        for (std::size_t i = 1; i < path.size(); ++i) {
            if (pkt.ttl() <= 0) break;
            const std::string& u = path[i-1];
            const std::string& v = path[i];

            double edgeCost = 1e9;
            auto it = graph_.data().find(u);
            if (it != graph_.data().end()) {
                for (auto& [to, link] : it->second) {
                    if (to == v) {
                        edgeCost = link.costForBytes(pkt.size());
                        break;
                    }
                }
            }

            totalSeconds += edgeCost;
            pkt.decTTL();
            pkt.addHop(v);
        }
        return totalSeconds;
    }

    // ... (saveTopology, loadTopology, printDevices)
};

#endif //NETWORKSIMULATOR_H