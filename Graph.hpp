/* Graph lib */

#pragma once

#include <iostream>
#include <map>
#include <ostream>
#include <utility>
#include <vector>

namespace rc {
class NodeBase {
  public:
    int index;
    friend std::ostream &operator<<(std::ostream &os, const NodeBase n) {
        os << n.index;
        return os;
    };
};

class EdgeBase {
  public:
    int index;
    friend std::ostream &operator<<(std::ostream &os, const EdgeBase n) {
        os << n.index;
        return os;
    };
};

template <class T, class U> class Graph {
    static_assert(std::is_base_of<NodeBase, T>::value,
                  "T must derive from NodeBase");
    static_assert(std::is_base_of<EdgeBase, U>::value,
                  "U must derive from EdgeBase");

    typedef std::vector<T *> Path;

  private:
    std::map<T *, std::vector<T *>> nodes;
    std::map<int, T *> nodesPerIndex;
    std::map<std::pair<T *, T *>, U *> edges;
    int nodeIndex = 0;
    int edgeIndex = 0;
    bool hasCycle = false;
    bool *visited;
    int howMuch = 0;
    int countHamCycles = 0;
    bool stopRecursion = false;

    int getNodeIndex(T *t) {
        NodeBase *e = dynamic_cast<NodeBase *>(t);

        return e->index;
    }

    int getEdgeIndex(U *u) {
        EdgeBase *e = dynamic_cast<EdgeBase *>(u);

        return e->index;
    }

    bool isSafe(T *n, Path &p, int pos) {
        // If the vertex is adjacent to
        // the vertex of the previously
        // added vertex
        if (!isAdjacent(n, p.back())) {
            return false;
        }

        // If the vertex has already
        // been included in the path
        for (int i = 0; i < pos; i++) {
            if (p.at(i) == n) {
                return false;
            }
        }

        // Both the above conditions are
        // not true, return true
        return true;
    }

    void findHamCycle(int pos, Path &p, bool visited[], int size,
                      std::vector<Path> *hamCycles = NULL) {
        // If all vertices are included
        // in Hamiltonian Cycle
        if (pos == size) {
            // If there is an edge
            // from the last vertex to
            // the source vertex
            if (isAdjacent(p.back(), getNodeByIndex(0))) {
                //        if (getPath(path[path.size() - 1], path[0]) != NULL) {

                // Include source vertex
                // into the path and
                // print the path
                p.push_back(getNodeByIndex(0));

                // displayPath(p);

                if (hamCycles) {
                    hamCycles->push_back(p);
                }

                if (howMuch > 0) {
                    countHamCycles++;
                    if (countHamCycles == howMuch) {
                        stopRecursion = true;
                    }
                }
                //                std::cout << std::endl;
                //            }

                // Remove the source
                // vertex added
                p.pop_back();

                // Update the hasCycle
                // as true
                hasCycle = true;
            }
            return;
        }

        // Try different vertices
        // as the next vertex
        for (int v = 0; v < size && !stopRecursion; v++) {
            // Check if this vertex can
            // be added to Cycle
            T *n = getNodeByIndex(v);
            if (isSafe(n, p, pos) && !visited[v]) {
                p.push_back(n);
                visited[v] = true;

                // Recur to construct
                // rest of the path
                findHamCycle(pos + 1, p, visited, size, hamCycles);

                // Remove current vertex
                // from path and process
                // other vertices
                visited[v] = false;
                p.pop_back();
            }
        }
    } // findHamCycle

  public:
    Graph<>() {
        nodeIndex = 0;
        edgeIndex = 0;
    }

    int addNode(T &t) {
        if (nodes.find(&t) != nodes.end()) {
            NodeBase *n = dynamic_cast<NodeBase *>(&t);
            return n->index;
        } else {
            std::vector<T *> v;
            std::pair<T *, std::vector<T *>> p(&t, v);
            NodeBase *n = dynamic_cast<NodeBase *>(p.first);
            n->index = nodeIndex;
            nodes.insert(p);
            nodesPerIndex.insert(std::pair<int, T *>(nodeIndex, &t));
            nodeIndex++;
            return n->index;
        }
    }

    int getSize() { return nodeIndex; }

    std::vector<T *> getAdjacents(T &t) { return nodes.at(&t); }

    bool isAdjacent(T *n, T *m) {
        std::vector<T *> *v = &nodes.at(n);

        for (auto i = v->begin(); i != v->end(); i++) {
            if (*i == m) {
                return true;
            }
        }
        return false;
    }

    int addEdge(T *n, T *m, U *e) {
        if (nodes.find(n) != nodes.end() && nodes.find(m) != nodes.end()) {
            // add adjacent
            std::vector<T *> *v = &nodes.at(n);
            v->push_back(m);

            // create an Edge in Edge dict
            std::pair<T *, T *> p = std::make_pair(n, m);
            std::pair<std::pair<T *, T *>, U *> u = std::make_pair(p, e);

            EdgeBase *f = dynamic_cast<EdgeBase *>(e);
            f->index = edgeIndex;
            edges.insert(u);
            edgeIndex++;
            return f->index;
        }
        return -1;
    }

    U *getEdge(T *n, T *m) {
        std::pair<T *, T *> p = std::make_pair(n, m);

        if (edges.find(p) == edges.end()) {
            return NULL;
        }
        return edges.at(p);
    }

    /* T * findNodeByIndex(int index) */
    /* { */
    /*     // Works because of assert ! */
    /*     for (auto i = nodes.begin(); i != nodes.end(); i++) { */
    /*         NodeBase *n = dynamic_cast<NodeBase *>(i->first); */
    /*         if (n->index == index) { */
    /*             return i->first; */
    /*         } */
    /*     } */
    /*     return NULL; */
    /* } */

    T *getNodeByIndex(int index) {
        T *t = nodesPerIndex.at(index);

        return t;
    }

    std::map<T *, std::vector<T *>>::iterator nodesBegin() {
        return nodes.begin();
    }

    std::map<T *, std::vector<T *>>::iterator nodesEnd() { return nodes.end(); }

    std::map<std::pair<T *, T *>, U *>::iterator edgesBegin() {
        return edges.begin();
    }

    std::map<std::pair<T *, T *>, U *>::iterator edgesEnd() {
        return edges.end();
    }

    size_t nodeSize() { return nodes.size(); }

    size_t edgeSize() { return edges.size(); }

    void hamCycle(int hm = 1, std::vector<Path> *hamCycles = NULL) {
        // Initially value of boolean
        // flag is false
        hasCycle = false;
        howMuch = hm;
        countHamCycles = 0;
        stopRecursion = false;

        // Store the resultant path
        Path p;
        p.push_back(getNodeByIndex(0));

        // Keeps the track of the
        // visited vertices
        // bool visited[size];
        visited = new bool[getSize()];

        for (int i = 0; i < getSize(); i++) {
            visited[i] = false;
        }

        visited[0] = true;

        // Function call to find all
        // hamiltonian cycles
        findHamCycle(1, p, visited, getSize(), hamCycles);

        if (!hasCycle) {
            // If no Hamiltonian Cycle
            // is possible for the
            // given graph
            std::cout << "No Hamiltonian Cycle possible " << std::endl;
            return;
        }

        delete[] visited;
    } // hamCycle

    friend std::ostream &operator<<(std::ostream &os, Graph<T, U> &g) {
        for (auto n = g.nodes.begin(); n != g.nodes.end(); n++) {
            os << *(n->first) << std::endl;

            for (auto m = n->second.begin(); m != n->second.end(); m++) {
                U *u = g.getEdge(n->first, *m);
                os << "  └" << *u << **m << std::endl;
            }
        }
        return os;
    }
    void displayPath(Path &p) {
        for (int i = 0; i < p.size(); i++) {
            T *d = p.at(i);
            std::cout << "    " << *d << std::endl;
        }
    }
};
} // namespace rc
