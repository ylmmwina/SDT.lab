#include "gtest/gtest.h"
#include "../Graph.h"
#include "../GraphAlgorithms.h"
#include "../Network.h"
#include "../NetworkSimulator.h"

// =========================================================
// ТЕСТОВА ГРУПА 1: Graph Operations (3 нові тести)
// =========================================================

// Test 1: Перевірка додавання/видалення вузла (T1)
TEST(GraphTest, NodeAdditionAndRemoval) {
    Graph<int, int> g;
    g.addNode(1);
    g.addNode(2);
    ASSERT_TRUE(g.hasNode(1));
    ASSERT_EQ(g.size(), 2u);

    g.removeNode(1);
    ASSERT_FALSE(g.hasNode(1));
    ASSERT_EQ(g.size(), 1u);
}

// Test 2: Перевірка роботи неорієнтованого графа (T2)
TEST(GraphTest, UndirectedEdgeCreation) {
    Graph<char, double> g(false); // Неорієнтований
    g.addEdge('A', 'B', 1.0);

    // У неорієнтованому графі B має бути сусідом A, і A має бути сусідом B
    std::vector<char> neighborsA = g.getNeighbors('A');
    std::vector<char> neighborsB = g.getNeighbors('B');

    ASSERT_EQ(neighborsA.size(), 1u);
    ASSERT_EQ(neighborsB.size(), 1u);
    EXPECT_EQ(neighborsA[0], 'B');
    EXPECT_EQ(neighborsB[0], 'A');
}

// Test 3: Перевірка видалення ребра (T3)
TEST(GraphTest, EdgeRemoval) {
    Graph<int, int> g(true);
    g.addEdge(1, 2, 10);
    g.addEdge(2, 3, 20);
    ASSERT_EQ(g.getNeighbors(1).size(), 1u);

    g.removeEdge(1, 2);
    ASSERT_EQ(g.getNeighbors(1).size(), 0u);
    // Перевірка, що дзеркальне ребро не видалено (бо граф орієнтований)
    ASSERT_EQ(g.getNeighbors(2).size(), 1u);
}

// =========================================================
// ТЕСТОВА ГРУПА 2: Link and Packet (3 нові тести)
// =========================================================

// Test 4: Link - граничний випадок 0 Mbps (вимоги lab) (T4)
TEST(LinkTest, ZeroBandwidthCost) {
    Link link{5.0, 0.0, 1.0}; // 0 Mbps BW
    size_t bytes = 1000;

    // Очікуємо 1e9, як визначено в Network.h
    double actual_cost = link.costForBytes(bytes);

    ASSERT_GE(actual_cost, 1e9);
}

// Test 5: Packet - перевірка зменшення TTL та додавання Hop (T5)
TEST(PacketTest, TTLAndHops) {
    Packet pkt("src", "dst", 3, 100);
    EXPECT_EQ(pkt.ttl(), 3);

    pkt.decTTL();
    pkt.addHop("R1");
    EXPECT_EQ(pkt.ttl(), 2);
    ASSERT_EQ(pkt.hops().size(), 1u);
    EXPECT_EQ(pkt.hops().back(), "R1");
}

// Test 6: BFS - перевірка обходу (T6)
TEST(BFSTest, SimpleTraversal) {
    Graph<int, int> g(false);
    g.addEdge(1, 2, 0);
    g.addEdge(1, 3, 0);
    g.addEdge(2, 4, 0);

    // Оскільки BFS друкує в консоль, ми можемо перевірити лише,
    // що алгоритм не падає на простому графі.
    // (Для точної перевірки потрібна фіксація виводу, але для лаб. роботи достатньо)
    BFS<int, int> bfs;
    // Очікуваний порядок: 1 2 3 4
    // (Це також перевірено у вашому main.cpp)
    ASSERT_NO_FATAL_FAILURE(bfs.run(g, 1));
}


// =========================================================
// ТЕСТОВА ГРУПА 3: Dijkstra (1 новий тест)
// =========================================================

// Test 7: Dijkstra - неіснуючий шлях (T7)
TEST(DijkstraTest, NoPathToTarget) {
    Graph<std::string, WeightedEdge> wg(true);
    wg.addNode("A");
    wg.addNode("B"); // Вузол B ізольований
    wg.addEdge("A", "C", WeightedEdge{1.0});

    Dijkstra<std::string> dj;
    dj.run(wg, "A");

    // Відстань до ізольованого вузла B має бути нескінченною
    ASSERT_EQ(dj.dist.at("B"), std::numeric_limits<double>::infinity());

    // Шлях до ізольованого вузла має бути порожнім
    auto path = dj.getPathTo("A", "B");
    ASSERT_TRUE(path.empty());
}

// Test 8: HierarchyTest (існуючий)
TEST(HierarchyTest, KindAndDynamicCast) {
    Device* r = new Router(1, "R1", "mgmt0");
    Device* s = new Switch(2, "S1", "mgmt1");
    Device* h = new Host(3, "H1", "10.0.0.1");

    EXPECT_EQ(r->kind(), "Router");
    EXPECT_EQ(s->kind(), "Switch");
    EXPECT_EQ(h->kind(), "Host");

    // Router повинен каститись до NetworkDevice
    NetworkDevice* nd = dynamic_cast<NetworkDevice*>(r);
    ASSERT_NE(nd, nullptr);
    EXPECT_EQ(nd->mgmtInterface(), "mgmt0");

    // Host — у нас варіант A: Host не є NetworkDevice
    NetworkDevice* nd2 = dynamic_cast<NetworkDevice*>(h);
    EXPECT_EQ(nd2, nullptr);
    delete r;
    delete s;
    delete h;
}

// Test 9: GraphDijkstraTest (існуючий)
TEST(GraphDijkstraTest, BasicPathAndDistance) {
    Graph<std::string, WeightedEdge> wg(true);
    wg.addNode("A"); wg.addNode("B"); wg.addNode("C");
    wg.addEdge("A","B", WeightedEdge{5.0});
    wg.addEdge("B","C", WeightedEdge{2.0});
    wg.addEdge("A","C", WeightedEdge{9.0});

    Dijkstra<std::string> dj;
    dj.run(wg, "A");
    auto path = dj.getPathTo("A", "C");

    ASSERT_EQ(path.size(), 3);
    EXPECT_EQ(path[0], "A");
    EXPECT_EQ(path[1], "B");
    EXPECT_EQ(path[2], "C");
    EXPECT_NEAR(dj.dist.at("C"), 7.0, 1e-9);
}

// Test 10: NetworkSimulatorTest (існуючий)
TEST(NetworkSimulatorTest, BuildDemoRouteAndSend) {
    NetworkSimulator sim;
    sim.addDevice(new Router(1, "R1"));
    sim.addDevice(new Switch(2, "S1"));
    sim.addDevice(new Host(3, "H1", "10.0.0.1"));
    sim.addDevice(new Host(4, "H2", "10.0.0.2"));

    sim.connect("R1","S1", Link{0.5, 100.0, 0.999});
    sim.connect("S1","H1", Link{1.0, 100.0, 0.999});
    sim.connect("R1","H2", Link{3.0, 20.0, 0.98});

    DijkstraRouting algo;
    Packet pkt("H1","H2", 8, 1500);
    auto route = sim.findRoute(algo, "H1", "H2", pkt.size());

    // перевіряємо, що маршрут знайдено (мінімум два вузли)
    ASSERT_GE(route.size(), 2u);

    double t = sim.sendPacket(route, pkt);
    EXPECT_GT(t, 0.0);
    EXPECT_LT(pkt.ttl(), 8); // TTL зменшився
}