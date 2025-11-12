#ifndef NETWORKSIMULATOR_H
#define NETWORKSIMULATOR_H

/*
КЛАСИ/ТИПИ У ФАЙЛІ:
 13) class RoutingAlgorithm (абстрактний) [КЛАС №13]
 14) class DijkstraRouting : public RoutingAlgorithm [КЛАС №14]
 15) class NetworkSimulator [КЛАС №15]

ПОЛЯ:
  - DijkstraRouting: (немає постійних полів)
  - NetworkSimulator:
      graph_  (Graph<std::string, Link>) - 1
      devices_(std::map<std::string, Device*>) - 1
    разом: 2

НЕТРИВІАЛЬНІ МЕТОДИ:
  (М21) RoutingAlgorithm::route(...) - абстрактний поліморфний метод
  (М22) DijkstraRouting::route(...) - обгортка над Дейкстрою з перетворенням Link->WeightedEdge
  (М23) NetworkSimulator::addDevice(...) - реєстрація пристрою
  (М24) NetworkSimulator::connect(...) - додавання зв’язку між вузлами
  (М25) NetworkSimulator::buildDemo(...) - побудова демо-топології
  (М26) NetworkSimulator::findRoute(...) - пошук шляху між двома вузлами
  (М27) NetworkSimulator::sendPacket(...) - симуляція передачі пакета hop-by-hop (TTL, час)
  (М28) NetworkSimulator::saveTopology(...) - збереження у файл
  (М29) NetworkSimulator::loadTopology(...) - читання з файлу
  (М30) NetworkSimulator::printDevices() - друк реєстру пристроїв
  разом: 10

ПРИМІТКА:
  - друга ієрархія успадкування: RoutingAlgorithm → DijkstraRouting (динамічний поліморфізм)
  - перша ієрархія — у Network.hpp: Device → Router/Switch/Host
*/

#include "Graph.h"
#include "GraphAlgorithms.h" // Dijkstra + WeightedEdge
#include "Network.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>


// інтерфейс алгоритму маршрутизації
class RoutingAlgorithm {
public:
    virtual ~RoutingAlgorithm() = default;
    // (М21) повертає послідовність імен вузлів (path) з src -> dst
    virtual std::vector<std::string> route(
        const Graph<std::string, Link>& g,
        const std::string& src,
        const std::string& dst,
        std::size_t payloadBytes) = 0;
};

// реалізація на основі Дейкстри
class DijkstraRouting : public RoutingAlgorithm {
public:
    // (М22) ми будуємо тимчасовий "зважений" граф, де вага ребра = час передачі payloadBytes
    std::vector<std::string> route(
        const Graph<std::string, Link>& g,
        const std::string& src,
        const std::string& dst,
        std::size_t payloadBytes) override
    {
        // будуємо тимчасовий граф з WeightedEdge
        Graph<std::string, WeightedEdge> wg(true);
        for (auto& [u, _] : g.data()) wg.addNode(u);

        for (auto& [u, vec] : g.data()) {
            for (auto& [v, link] : vec) {
                double w = link.costForBytes(payloadBytes); // час у секундах
                wg.addEdge(u, v, WeightedEdge{ w });
            }
        }

        Dijkstra<std::string> dj;
        dj.run(wg, src);
        return dj.getPathTo(src, dst);
    }
};

// симулятор мережі
class NetworkSimulator {
private:
    Graph<std::string, Link>      graph_;
    std::map<std::string, Device*> devices_;

public:
    ~NetworkSimulator() {
        // прибирання динамічно створених пристроїв (демо-спосіб)
        for (auto& [k, ptr] : devices_) delete ptr;
    }

    // (М23) реєстрація пристрою в мережі
    void addDevice(Device* d) {
        if (!d) throw std::runtime_error("Null device");
        devices_[d->name()] = d;
        graph_.addNode(d->name());
    }

    // (М24) з’єднання двох вузлів каналом Link (за замовч. — двосторонній)
    void connect(const std::string& a, const std::string& b, const Link& link, bool bidir = true) {
        if (!graph_.hasNode(a) || !graph_.hasNode(b))
            throw std::runtime_error("Unknown node in connect()");
        graph_.addEdge(a, b, link);
        if (bidir) graph_.addEdge(b, a, link);
    }

    // (М25) демо-топологія:  R1 ─ S1 ─ H1,  R1 ─ H2 (довший шлях)
    void buildDemo() {
        addDevice(new Router(1, "R1", "eth0"));
        addDevice(new Switch(2, "S1", "mgmt0"));
        addDevice(new Host(3, "H1", "10.0.0.1"));
        addDevice(new Host(4, "H2", "10.0.0.2"));

        connect("R1", "S1", Link{0.5, 100.0, 0.999});
        connect("S1", "H1", Link{1.0, 100.0, 0.999});
        connect("R1", "H2", Link{3.0, 20.0, 0.98});
    }

    // (М26) знайти маршрут між src та dst (імена вузлів), використовуючи алгоритм
    std::vector<std::string> findRoute(
        RoutingAlgorithm& algo,
        const std::string& src,
        const std::string& dst,
        std::size_t payloadBytes) const
    {
        return algo.route(graph_, src, dst, payloadBytes);
    }

    // (М27) відправити пакет за маршрутом (зменшуючи TTL, накопичуючи час)
    double sendPacket(const std::vector<std::string>& path, Packet& pkt) const {
        if (path.size() < 2) return 0.0;
        double totalSeconds = 0.0;
        pkt.addHop(path.front());

        for (std::size_t i = 1; i < path.size(); ++i) {
            if (pkt.ttl() <= 0) break;
            const std::string& u = path[i-1];
            const std::string& v = path[i];

            // знайти Link(u->v)
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

    /* (М28) зберегти топологію у простий текстовий формат:
     NODES:
     R1 Router
     H1 Host
     EDGES:
     R1 S1 0.5 100 0.999 */
    void saveTopology(const std::string& filename) const {
        std::ofstream out(filename);
        if (!out) throw std::runtime_error("Cannot open file for writing");
        out << "NODES:\n";
        for (auto& [name, dev] : devices_) {
            out << " " << name << " " << dev->kind() << "\n";
        }
        out << "EDGES:\n";
        for (auto& [u, vec] : graph_.data()) {
            for (auto& [v, link] : vec) {
                out << " " << u << " " << v << " "
                    << link.latencyMs << " "
                    << link.bandwidthMbps << " "
                    << link.reliability << "\n";
            }
        }
    }

    // (М29) завантажити топологію з такого самого формату (для простоти: пристрої створюються як Router/Switch/Host за тегом у файлі)
    void loadTopology(const std::string& filename) {
        // очистка (видалення старих пристроїв)
        for (auto& [k, ptr] : devices_) delete ptr;
        devices_.clear();
        graph_.clear();

        std::ifstream in(filename);
        if (!in) throw std::runtime_error("Cannot open file for reading");
        std::string line;
        enum Section { NONE, NODES, EDGES } sect = NONE;
        while (std::getline(in, line)) {
            if (line == "NODES:") { sect = NODES; continue; }
            if (line == "EDGES:") { sect = EDGES; continue; }
            if (line.empty() || line[0] == '#') continue;

            std::istringstream iss(line);
            if (sect == NODES) {
                std::string name, kind;
                iss >> name >> kind;
                if (kind == "Router") addDevice(new Router((int)devices_.size()+1, name));
                else if (kind == "Switch") addDevice(new Switch((int)devices_.size()+1, name));
                else if (kind == "Host") addDevice(new Host((int)devices_.size()+1, name, "0.0.0.0"));
                else addDevice(new Router((int)devices_.size()+1, name)); // за замовч.
            } else if (sect == EDGES) {
                std::string u, v; double lat, bw, rel;
                iss >> u >> v >> lat >> bw >> rel;
                connect(u, v, Link{lat, bw, rel}, false);
            }
        }
    }

    // (М30) допоміжний друк реєстру пристроїв
    void printDevices() const {
        std::cout << "Devices:\n";
        for (auto& [name, dev] : devices_) {
            std::cout << "  " << std::left << std::setw(6) << name
                      << " : " << dev->kind() << "\n";
        }
    }
};

#endif //NETWORKSIMULATOR_H