#include "Graph.h"
#include "GraphAlgorithms.h"
#include "Network.h"
#include "NetworkSimulator.h"
#include <clocale>
#include <iostream>

/*
DEMO
- показуємо:
  1) граф на int + BFS/DFS (демо шаблонів і алгоритмів)
  2) дейкстру на зваженому графі (WeightedEdge)
  3) мережу: побудова, пошук маршруту (DijkstraRouting), передача пакета
*/


int main() {

    std::cout << "OOP Lab 1 - Networks + Graphs\n\n";

    // 1) базовий граф + BFS/DFS
    {
        Graph<int, int> g(false); // неорієнтований
        g.addNode(1); g.addNode(2); g.addNode(3);
        g.addEdge(1, 2, 1);
        g.addEdge(2, 3, 1);
        std::cout << "[Simple Graph]\n";
        g.printGraph();

        BFS<int,int> bfs;
        DFS<int,int> dfs;
        bfs.run(g, 1); // демонструє BFS: 1 2 3
        dfs.run(g, 1); // демонструє DFS
        std::cout << "\n";
    }

    // 2) дейкстра на зваженому графі
    {
        Graph<std::string, WeightedEdge> wg(true);
        wg.addNode("A"); wg.addNode("B"); wg.addNode("C");
        wg.addEdge("A","B", WeightedEdge{5.0});
        wg.addEdge("B","C", WeightedEdge{2.0});
        wg.addEdge("A","C", WeightedEdge{9.0});

        Dijkstra<std::string> dj;
        dj.run(wg, "A");
        auto path = dj.getPathTo("A","C");
        std::cout << "[Dijkstra]\nA->C: ";
        for (auto& x : path) std::cout << x << (x==path.back() ? '\n' : ' ');
        std::cout << "dist(C) = " << dj.dist["C"] << " (expect 7.0)\n\n";
    }

    // 3) мережа + маршрутизація + передача пакета
    {
        NetworkSimulator sim;
        sim.buildDemo(); // R1 - S1 - H1, і альтернативний R1 - H2
        sim.printDevices();

        DijkstraRouting routerAlgo;
        Packet pkt("H1","H2", /*ttl*/8, /*size*/1500); // 1500 bytes

        auto route = sim.findRoute(routerAlgo, "H1", "H2", pkt.size());
        std::cout << "\n[Routing] H1 -> H2 path: ";
        if (route.empty()) std::cout << "(no path)\n";
        else {
            for (auto& n : route) std::cout << n << (n==route.back()?'\n':' ');
            double seconds = sim.sendPacket(route, pkt);
            std::cout << "Packet TTL left: " << pkt.ttl() << "\n";
            std::cout << "Packet hops   : ";
            for (auto& h : pkt.hops()) std::cout << h << (h==pkt.hops().back()?'\n':' ');
            std::cout << "Total time (s): " << seconds << "\n";
        }

        // збереження / завантаження (демо):
        sim.saveTopology("topology.txt");
        NetworkSimulator sim2;
        sim2.loadTopology("topology.txt");
        std::cout << "\n[Loaded topology] ";
        sim2.printDevices();
    }

    std::cout << "\nDONE.\n";
    return 0;
}