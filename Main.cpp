#include "GraphSalesman.hpp"
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <ostream>
#include <vector>

const int STABILIZED_AFTER = 100;
const int PHEROMON_EVAPORATION_AFTER_N_ANTS = 5;


long getTimeMillis() {
    auto time = std::chrono::system_clock::now();
    auto since_epoch = time.time_since_epoch();
    auto millis =
        std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);
    long now = millis.count();

    return now;
}

int main() {
    GraphSalesman g;

    NodeSalesman n0("Amiens", 525, 115);
    NodeSalesman n1("Angoulème", 365, 585);
    NodeSalesman n2("Auxerre", 620, 350);
    NodeSalesman n3("Bayonne", 230, 830);
    NodeSalesman n4("Bordeaux", 305, 675);
    NodeSalesman n5("Bourges", 535, 425);
    NodeSalesman n6("Brest", 15, 265);
    NodeSalesman n7("Cherbourg", 250, 140);
    NodeSalesman n8("Clermond-Ferrand", 590, 565);
    NodeSalesman n9("Dijon", 730, 400);
    NodeSalesman n10("Grenoble", 795, 630);
    NodeSalesman n11("Le Havre", 380, 150);
    NodeSalesman n12("Le Mans", 370, 325);
    NodeSalesman n13("Lille", 580, 35);
    NodeSalesman n14("Limoges", 450, 565);
    NodeSalesman n15("Lyon", 725, 570);
    NodeSalesman n16("Marseille", 755, 840);
    NodeSalesman n17("Metz", 810, 195);
    NodeSalesman n18("Montpellier", 655, 805);
    NodeSalesman n19("Mulhouse", 910, 340);
    NodeSalesman n20("Nancy", 810, 245);
    NodeSalesman n21("Nantes", 245, 405);
    NodeSalesman n22("Nice", 925, 785);
    NodeSalesman n23("Orléans", 495, 330);
    NodeSalesman n24("Paris", 525, 235);
    NodeSalesman n25("Pau", 320, 845);
    NodeSalesman n26("Périgueux", 410, 630);
    NodeSalesman n27("Poitiers", 380, 480);
    NodeSalesman n28("Reims", 655, 185);
    NodeSalesman n29("Rennes", 230, 310);
    NodeSalesman n30("Rouen", 440, 165);
    NodeSalesman n31("Saint-Etienne", 690, 600);
    NodeSalesman n32("Strasbourg", 925, 245);
    NodeSalesman n33("Toulouse", 465, 805);
    NodeSalesman n34("Tours", 410, 390);
    NodeSalesman n35("Troyes", 655, 290);

    // Node container
    std::vector<NodeSalesman> towns = {
        n0,  n1,  n2,  n3,  n4,  n5,  n6,  n7,  n8,  n9,  n10, n11,
        n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23,
        n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35};

    std::vector<NodeSalesman> nearTowns;
    std::vector<EdgeSalesman> paths;

    for (auto n = towns.begin(); n != towns.end(); n++) {
        /* std::vector<NodeSalesman> nearTowns; */
        g.addNode(*n);
    }

    // Edges container
    std::vector<EdgeSalesman *> edges;

    for (auto n = g.nodesBegin(); n != g.nodesEnd(); n++) {
        std::vector<NodeSalesman> nearTowns;
        g.findNodesInRadius(2000, *(n->first), towns, nearTowns);

        for (auto m = nearTowns.begin(); m != nearTowns.end(); m++) {
            NodeSalesman *o = g.findNode(g, m->name);
            EdgeSalesman *e = new EdgeSalesman(n->first->name + ">" + o->name);
            edges.push_back(e);
            g.addEdge(n->first, o, e);
        }
    }

    std::cout << "Nodes count:" << g.nodeSize() << std::endl;
    std::cout << "Edges count:" << g.edgeSize() << std::endl;

    std::cout << g << std::endl;

    auto i = g.nodesBegin();
    NodeSalesman *nodeStart = i->first;

    std::vector<NodeSalesman *> probPath;

    srand(time(NULL));

    int j = 0;
    int lastPathSize = 0;
    int countSameSize = 0;
    bool stabilization = false;
    float mean = 0;
    const int HOW_MANY = 10;
    float shorter = std::numeric_limits<float>::max();
    std::vector<NodeSalesman *> shorterPath;

    for (int i = 0; i < HOW_MANY; i++) {
        while (!stabilization) {
            /* if (!stabilization) { */
                probPath.clear();
                int r = std::rand() % (g.getSize());
                nodeStart = g.getNodeByIndex(r);
                g.findProbabilisticPath(nodeStart, probPath);
                g.doAntPheromonLeftOnPath(probPath);
                float pathSize = g.pathSize(probPath);
                float minPathSize = pathSize - (0.1f / 100) * pathSize;
                float maxPathSize = pathSize + (0.1f / 100) * pathSize;
                if (lastPathSize > minPathSize && lastPathSize < maxPathSize) {
                    countSameSize++;
                } else {
                    countSameSize = 0;
                }

                if (countSameSize == STABILIZED_AFTER) {
                    countSameSize++;
                    float psz = g.pathSize(probPath);
                    std::cout << "Stabilization after " << j << " iterations. Distance=" << psz
                              << std::endl;
                    stabilization = true;
                    if (psz < shorter) {
                        shorter = psz;
                        shorterPath = probPath;
                    }
                    mean += psz;
                }

                /* g.scalePheromon(); */
                if (j % PHEROMON_EVAPORATION_AFTER_N_ANTS) {
                    // Evaparation every N ants
                    g.doPheromonEvaporation();
                }

                lastPathSize = pathSize;
            /* } */
            j++;
        }

        stabilization = false;
        lastPathSize = 0;
        countSameSize = 0;
        j = 0;
        for (auto e = g.edgesBegin(); e != g.edgesEnd(); e++) {
            e->second->ph = .5f;
        }
    }


    std::cout << "Shorter path:" << std::endl;
    for (auto n = shorterPath.begin(); n != shorterPath.end(); n++) {
        std::cout << (*n)->name << "->";
    }
    std::cout << "NULL" << std::endl;


    std::cout << "mean:" << mean / HOW_MANY << std::endl;
    std::cout << "shorter:" << shorter << std::endl;

    // cleanup
    for (auto i = edges.begin(); i != edges.end(); i++) {
        delete *i;
    }

    return 0;
} // main
