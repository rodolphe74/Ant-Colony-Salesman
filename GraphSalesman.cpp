#include "GraphSalesman.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <set>
#include <string>
#include <time.h>
#include <vector>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
void getResourcePath(const CFStringRef resourceName, char *resourcePath) {
    CFURLRef appUrlRef;
    appUrlRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), resourceName,
                                        NULL, NULL);
    CFStringRef filePathRef = CFURLCopyPath(appUrlRef);
    const char *filePath =
        CFStringGetCStringPtr(filePathRef, kCFStringEncodingUTF8);

    std::cout << "filePath:" << filePath << std::endl;
    strcpy(resourcePath, filePath);

    CFRelease(filePathRef);
    CFRelease(appUrlRef);
}
#endif

float GraphSalesman::distance(NodeSalesman &n0, NodeSalesman &n1) {
    return (std::sqrtf((n1.east - n0.east) * (n1.east - n0.east) +
                       (float)(n1.south - n0.south) * (n1.south - n0.south)));
}

float GraphSalesman::pathSize(std::vector<NodeSalesman *> &n) {
    float sz = 0;

    for (int i = 0; i < n.size() - 1; i++) {
        NodeSalesman *d = n[i];
        NodeSalesman *e = n[i + 1];
        sz += distance(*d, *e);
    }
    return sz;
}

void GraphSalesman::renderGraph(sf::RenderWindow &window,
                                std::vector<NodeSalesman *> *path, float zoom) {
    int h = window.getSize().y;
    int w = window.getSize().x;

    sf::VertexArray line(sf::LinesStrip, 2);
    /* for (auto n = nodesBegin(); n != nodesEnd(); n++) { */
    /*     std::vector<NodeSalesman *> a = getAdjacents(*(n->first)); */
    /*     for (auto m = a.begin(); m != a.end(); m++) { */
    /*         float x = (**m).east; */
    /*         float y = (**m).south; */

    /*         EdgeSalesman *e = getEdge(n->first, (*m)); */
    /*         float phi = e->scaledPh; */
    /*         /1* float phi = 1.0f; *1/ */

    /*         line[0].position = */
    /*             sf::Vector2f(n->first->east * zoom, n->first->south * zoom);
     */
    /*         line[0].color = sf::Color(64 + phi, 64 + phi, 64 + phi); */
    /*         line[1].position = sf::Vector2f(x * zoom, y * zoom); */
    /*         line[1].color = sf::Color(64 + phi, 64 + phi, 64 + phi); */
    /*         window.draw(line); */
    /*     } */
    /* } */

    // Render pheromon intensity with color shade
    // need to maintain a Z order.
    // edges with more pheromon are drawn last
    std::vector<
        std::pair<std::pair<NodeSalesman *, NodeSalesman *>, EdgeSalesman *>>
        ev;
    for (auto e = edgesBegin(); e != edgesEnd(); e++) {
        ev.push_back(*e);
    }
    std::sort(ev.begin(), ev.end(), EdgeSalesman::cmp);
    for (auto e = ev.begin(); e != ev.end(); e++) {
        NodeSalesman *n = e->first.first;
        NodeSalesman *m = e->first.second;
        float phi = e->second->scaledPh;
        line[0].position = sf::Vector2f(n->east * zoom, n->south * zoom);
        line[0].color = sf::Color(32 + phi * 2, 32 + phi * 2, 32 + phi * 2);
        line[1].position = sf::Vector2f(m->east * zoom, m->south * zoom);
        line[1].color = sf::Color(32 + phi * 2, 32 + phi * 2, 32 + phi * 2);
        window.draw(line);
    }

    // Loading font is a bit different for an apple bundle
    static sf::Font font;
    static bool fontLoaded = false;
#ifdef __APPLE__
    if (!fontLoaded) {
        char p[1024];
        CFStringRef cstrrefFont =
            CFStringCreateWithCString(NULL, "Arial.ttf", kCFStringEncodingUTF8);
        getResourcePath(cstrrefFont, p);
        font.loadFromFile(p);
        CFRelease(cstrrefFont);
        fontLoaded = true;
    }
#else
    if (!fontLoaded) {
        font.loadFromFile("./Arial.ttf");
        fontLoaded = true;
    }
#endif

    for (auto n = nodesBegin(); n != nodesEnd(); n++) {
        sf::CircleShape shape(5 * zoom);
        shape.setFillColor(sf::Color::Cyan);
        shape.move((n->first->east - shape.getRadius()) * zoom,
                   (n->first->south - shape.getRadius()) * zoom);
        window.draw(shape);

        sf::Text text;
        text.setFont(font);
        sf::String sfTmp =
            sf::String::fromUtf8(n->first->name.begin(), n->first->name.end());
        text.setString(sfTmp);
        text.move((n->first->east - 10) * zoom, (n->first->south - 20) * zoom);
        text.setFillColor(sf::Color::Green);
        text.setCharacterSize(12 * zoom);
        window.draw(text);
    }

    if (path) {
        for (int i = 0; i < path->size() - 1; i++) {
            NodeSalesman *n = path->at(i);
            NodeSalesman *m = path->at(i + 1);
            float d = distance(*m, *n);

            sf::VertexArray line(sf::LineStrip, 2);

            line[0].position = sf::Vector2f(n->east * zoom, n->south * zoom);
            line[0].color = sf::Color::Cyan;
            line[1].position = sf::Vector2f(m->east * zoom, m->south * zoom);
            line[1].color = sf::Color::Cyan;
            window.draw(line);
            sf::Text text;
            text.setFont(font);
            text.setString(std::to_string(i));
            text.setCharacterSize(12 * zoom);
            text.setPosition((n->east + 10) * zoom, n->south * zoom);
            window.draw(text);
        }

        float pSize = pathSize(*path);
        sf::Text text;
        text.setFont(font);
        text.setString(std::to_string(pSize) + " km");
        text.setCharacterSize(14 * zoom);
        text.setPosition(10 * zoom, 20 * zoom);
        window.draw(text);
    } // GraphSalesman::renderGraph
}

void GraphSalesman::findNodesInRadius(float dist, NodeSalesman &n,
                                      std::vector<NodeSalesman> &nodes,
                                      std::vector<NodeSalesman> &retNodes) {
    for (auto m = nodes.begin(); m != nodes.end(); m++) {
        if (distance(n, *m) <= dist && n.name != m->name) {
            retNodes.push_back(*m);
        }
    }
}

NodeSalesman *GraphSalesman::findNode(rc::Graph<NodeSalesman, EdgeSalesman> g,
                                      std::string name) {
    for (auto it = g.nodesBegin(); it != g.nodesEnd(); it++) {
        if (it->first->name == name) {
            return it->first;
        }
    }
    return NULL;
}

// Replace edges path[i]->path[i+1] and path[j]->path[j+1]
// with path[i]->path[j] and path[i+1]->path[j+1]
void GraphSalesman::swapEdges(std::vector<NodeSalesman *> &path, int i, int j) {
    i += 1;
    while (i < j) {
        NodeSalesman *temp = path[i];
        path[i] = path[j];
        path[j] = temp;
        i++;
        j--;
    }
}

float GraphSalesman::twoOpt(std::vector<NodeSalesman *> &n) {
    bool better = true;

    while (better == true) {
        better = false;
        for (int i = 0; i < n.size() - 1; i++) {
            for (int j = 0; j < n.size() - 1; j++) {
                if (j != i - 1 && j != i && j != i + 1) {
                    float a = distance(*n[i], *n[i + 1]);
                    float b = distance(*n[j], *n[j + 1]);
                    float c = distance(*n[i], *n[j]);
                    float d = distance(*n[i + 1], *n[j + 1]);
                    if (a + b > c + d) {
                        swapEdges(n, i, j);
                        better = true;
                    }
                }
            }
        }
    }
    return pathSize(n);
}

void GraphSalesman::findProbabilisticPath(NodeSalesman *nodeStart,
                                          std::vector<NodeSalesman *> &path) {
    std::set<NodeSalesman *> visited;
    std::vector<NodeSalesman *> possiblePaths;
    NodeSalesman *currentNode = nodeStart;
    float possibleSum = 0.0f;
    visited.insert(currentNode);
    path.push_back(currentNode);

    int i = 1;
    while (i != getSize()) {

        std::vector<NodeSalesman *> adj = getAdjacents(*currentNode);

        // set probability of each non visited node
        for (auto n = adj.begin(); n != adj.end(); n++) {
            NodeSalesman *nextNode = *n;

            if (!visited.contains(nextNode)) {
                EdgeSalesman *edge = getEdge(currentNode, nextNode);
                float dist = distance(*currentNode, *nextNode);
                dist = Q / dist;
                // nextNode->probability = edge->ph * (1.0f / dist);
                nextNode->probability = std::pow(edge->ph, A) * std::pow(dist, B); 
                possiblePaths.push_back(nextNode);
                possibleSum += nextNode->probability;
            }

       }
        /* std::cout << "psum:" << possibleSum << std::endl; */
        if (possiblePaths.size() > 0) {
            for (auto n = possiblePaths.begin(); n != possiblePaths.end();
                 n++) {
                (*n)->probability /= possibleSum;
                if (possibleSum == 0) {
                    std::cout << possibleSum << std::endl;
                    exit(1);
                }
            } // get a node regarding probabilities
            float sum = 0.0f;
            NodeSalesman *nextNode = NULL;
            std::sort(possiblePaths.begin(), possiblePaths.end(),
                      NodeSalesman::cmp);
            float r = ((float)rand() / (RAND_MAX));
            /* std::cout << std::endl; */
            for (auto n = possiblePaths.begin(); n != possiblePaths.end();
                 n++) {
                sum += (*n)->probability;
                /* std::cout << "r=" << r << " p=" << (*n)->probability */
                /*           << " s=" << sum << std::endl; */
                if (r <= sum) {
                    nextNode = *n;
                    visited.insert(nextNode);
                    path.push_back(nextNode);
                    currentNode = nextNode;
                    i++;
                    break;
                }
            }
        }
        possiblePaths.clear();
        possibleSum = 0.0f;
    }
    path.push_back(nodeStart);
}

void GraphSalesman::doPheromonEvaporation() {
    for (auto e = edgesBegin(); e != edgesEnd(); e++) {
        // if (e->first.first->name != e->first.second->name) {
        e->second->ph *= PHI_E_R;
        if (e->second->ph < PHI_MIN) {
            e->second->ph = PHI_MIN;
        }
        // §}
    }
}

void GraphSalesman::doAntPheromonLeftOnPath(std::vector<NodeSalesman *> &path,
                                            float factor) {
    float distance = pathSize(path);
    float bonus = Q / distance;
    for (int i = 0; i < path.size() - 1; i++) {
        NodeSalesman *d = path[i];
        NodeSalesman *e = path[i + 1];
        EdgeSalesman *f = getEdge(d, e);
        f->ph += (bonus * factor);
    }
}

void GraphSalesman::scalePheromon() {
    float minPh = std::numeric_limits<float>::max(),
          maxPh = std::numeric_limits<float>::min();
    ;

    for (auto e = edgesBegin(); e != edgesEnd(); e++) {
        if (e->second->ph < minPh) {
            minPh = e->second->ph;
        }
        if (e->second->ph > maxPh) {
            maxPh = e->second->ph;
        }
    }

    for (auto e = edgesBegin(); e != edgesEnd(); e++) {
        /* std::cout << "ph=" << e->second->ph; */
        e->second->scaledPh = (e->second->ph * 100) / maxPh;
        /* std::cout << "   scph=" << e->second->scaledPh << std::endl;; */
    }
    /* std::cout << "minph=" << minPh << "  maxph=" << maxPh << std::endl; */
}
