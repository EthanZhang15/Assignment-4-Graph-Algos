#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <sstream> // Include the <sstream> header


struct CSRGraph {
    int numNodes;
    int numEdges;
    std::vector<int> rp;
    std::vector<int> ci;
    std::vector<int> ai;
};

struct Edge {
    int s;
    int d;
    int w;
};

bool compareEdges(const Edge& edge1, const Edge& edge2) {
    if (edge1.s != edge2.s) {
        return edge1.s < edge2.s;
    } else if (edge1.d != edge2.d) {
        return edge1.d < edge2.d;
    } else {
        return edge1.w < edge2.w;
    }
}

void printCOO(const std::vector<Edge>& edges) {
    for (const Edge& edge : edges) {
        std::cout << "s: " << edge.s << ", d: " << edge.d << ", w: " << edge.w << std::endl;
    }
}

void printCSRG(const CSRGraph& graph) {
    std::cout << "Number of nodes: " << graph.numNodes << std::endl;
    std::cout << "Number of edges: " << graph.numEdges << std::endl;

    std::cout << "rp: ";
    for (int x : graph.rp) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    
    std::cout << "ci: ";
    for (int x : graph.ci) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    std::cout << "ai: ";
    for (int x : graph.ai) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

CSRGraph readDIMACS(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    int numNodes, numEdges;

    //read numNodes and numEdges
    while (std::getline(file, line)) {
        if (line[0] == 'p') {
            break;
        }
    }
    std::string token;
    std::istringstream iss(line);
    iss >> token >> token >> numNodes >> numEdges;

    //populate COO
    std::vector<Edge> COO;
    while (std::getline(file, line)) {
        if (line[0] == 'a') {
            int s, d, w;
            if (sscanf(line.c_str(), "a %d %d %d", &s, &d, &w) == 3) {
                COO.push_back({s, d, w});
            }
        }
    }

    //sort COO by source, destination, then weight
    std::sort(COO.begin(), COO.end(), compareEdges);

    //only keep biggest weights
    for (auto it = COO.begin(); it != COO.end(); ) {
        auto next = std::next(it);
        if (next != COO.end() && it->s == next->s && it->d == next->d) {
            if (it->w < next->w) {
                it = COO.erase(it);
            } else {
                COO.erase(next);
            }
        } else {
            ++it;
        }
    }

    printCOO(COO);

    //convert COO to CSR
    CSRGraph graph;
    graph.numNodes = numNodes;
    graph.numEdges = COO.size();
    graph.rp.push_back(0);
    graph.ci.push_back(0);
    graph.ai.push_back(0);

    for(int i = 0; i < COO.size(); i++){
        graph.ci.push_back(COO[i].d);
        graph.ai.push_back(COO[i].w);
    }
    
    printCSRG(graph);

    return graph;
}

// Function to write a graph in CSR representation to a file in DIMACS format
void writeDIMACS(const std::string& filename, const CSRGraph& graph) {
    /*
    std::ofstream file(filename);
    file << "p sp " << graph.offsets.size() - 1 << " " << graph.edge_labels.size() << "\n";

    for (int u = 1; u < graph.offsets.size(); u++) {
        for (int i = graph.offsets[u - 1]; i < graph.offsets[u]; i++) {
            int v = graph.edges[i * 2];
            int label = graph.edge_labels[i];
            file << "a " << u << " " << v << " " << label << "\n";
        }
    }
    */
}

int main() {
    // Replace with your input and output filenames
    std::string inputFilename = "wiki.dimacs";
    std::string outputFilename = "output.dimacs";

    CSRGraph graph = readDIMACS(inputFilename);


    //writeDIMACS(outputFilename, graph);

    return 0;
}
