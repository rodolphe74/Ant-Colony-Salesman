#pragma once

#include "Graph.hpp"
#include <SFML/Graphics.hpp>
#include <ostream>
#include <vector>

const float Q = 400.0f;
const float B = 4.0f;
const float A = 1.0f;
const float PHI = 0.5;
const float PHI_E_R = 0.88f;
const float PHI_MIN = 0.0000001;

class NodeSalesman : public rc::NodeBase {
  public:
    std::string name;
    int east;
    int south;
    float probability = 0.0f;

    NodeSalesman(std::string n, int e, int s) : east(e), south(s) { name = n; }

    friend std::ostream &operator<<(std::ostream &os, const NodeSalesman n) {
        os << n.name << "[" << n.index << "]" << "=" << n.east << ","
           << n.south;
        return os;
    }

    bool operator<(const NodeSalesman &other) const {
        return probability < other.probability;
    }

    static bool cmp(const NodeSalesman *a, const NodeSalesman *b) {
        return a->probability < b->probability;
    }
};

class EdgeSalesman : public rc::EdgeBase {
  public:
    std::string name;
    float ph = PHI; // pheromon left on path
    float scaledPh = 50.0f;

    EdgeSalesman(std::string n) { name = n; }

    friend std::ostream &operator<<(std::ostream &os, const EdgeSalesman e) {
        os << "--{" << e.name << "[" << e.index << "]" << "[" << e.ph << "]"
           << "}--";
        return os;
    }

    static bool
    cmp(std::pair<std::pair<NodeSalesman *, NodeSalesman *>, EdgeSalesman *> &a,
        std::pair<std::pair<NodeSalesman *, NodeSalesman *>, EdgeSalesman *>
            &b) {
        return a.second->scaledPh < b.second->scaledPh;
    }
};

class GraphSalesman : public rc::Graph<NodeSalesman, EdgeSalesman> {
  private:
    void swapEdges(std::vector<NodeSalesman *> &path, int i, int j);

  public:
    float distance(NodeSalesman &n0, NodeSalesman &n1);
    float pathSize(std::vector<NodeSalesman *> &n);
    void renderGraph(sf::RenderWindow &window,
                     std::vector<NodeSalesman *> *path = NULL,
                     float zoom = 1.0f);
    void findNodesInRadius(float dist, NodeSalesman &n,
                           std::vector<NodeSalesman> &nodes,
                           std::vector<NodeSalesman> &retNodes);
    NodeSalesman *findNode(rc::Graph<NodeSalesman, EdgeSalesman> g,
                           std::string name);
    float twoOpt(std::vector<NodeSalesman *> &n);
    void findProbabilisticPath(NodeSalesman *nodeStart,
                               std::vector<NodeSalesman *> &path);
    void doPheromonEvaporation();
    void doAntPheromonLeftOnPath(std::vector<NodeSalesman *> &path,
                                 float factor = 1.0f);
    void scalePheromon();
};
