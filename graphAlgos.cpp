#include <iostream>
#include <fstream>
#include <vector>

struct CSRGraph {
    std::vector<int> vertices;
    std::vector<int> edges;
};

CSRGraph readDIMACSGraph(const std::string& filename) {
    std::ifstream file(filename);
    CSRGraph graph;

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open the file: " << filename << std::endl;
        return graph;
    }

    int numVertices, numEdges;
    char line[256];

    while (file.getline(line, sizeof(line))) {
        if (line[0] == 'c') {
            // Comment line, ignore it
        } else if (line[0] == 'p') {
            // Problem line, extract number of vertices and edges
            sscanf(line, "p edge %d %d", &numVertices, &numEdges);
            graph.vertices.resize(numVertices + 1);
            graph.edges.reserve(numEdges * 2); // Assuming an undirected graph, so double the edges
        } else if (line[0] == 'e') {
            // Edge line, extract edge information
            int source, target;
            sscanf(line, "e %d %d", &source, &target);
            graph.edges.push_back(source);
            graph.edges.push_back(target);
        }
    }

    file.close();

    // Build CSR representation
    int currentEdge = 0;
    for (int i = 0; i < numVertices; ++i) {
        graph.vertices[i] = currentEdge;
        while (graph.edges[currentEdge] == i + 1) {
            ++currentEdge;
        }
    }
    graph.vertices[numVertices] = currentEdge;

    return graph;
}

int main() {
    const std::string filename = "your_dimacs_file.txt";
    CSRGraph graph = readDIMACSGraph(filename);

    // Example: Accessing the CSR data
    for (int i = 0; i < graph.vertices.size() - 1; ++i) {
        int start = graph.vertices[i];
        int end = graph.vertices[i + 1];
        for (int j = start; j < end; ++j) {
            int targetVertex = graph.edges[j];
            // Process the edge between vertex i and targetVertex
            // Edge label can be accessed as graph.edges[j+1]
        }
    }

    return 0;
}
