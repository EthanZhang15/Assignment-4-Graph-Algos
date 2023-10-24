#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <functional>  // for std::hash

struct PairHash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

struct Graph {
    std::vector<int> edges;
    std::vector<int> edge_labels;
    std::vector<int> offsets;
};

// Function to read a graph in DIMACS format from a file and construct CSR representation
Graph readDIMACS(const std::string& filename) {
    Graph graph;
    std::ifstream file(filename);
    std::string line;
    int numNodes, numEdges;

    std::unordered_map<std::pair<int, int>, int, PairHash> edgeWeights;

    while (std::getline(file, line)) {

        std::cout << line << std::endl;

        if (line.empty()) {
            continue;
        }

        if (line[0] == 'c') {
            continue; // Ignore comments
        } else if (line[0] == 'p') {
            sscanf(line.c_str(), "p %*s %d %d", &numNodes, &numEdges);
            graph.edges.resize(numEdges * 2);
            graph.edge_labels.resize(numEdges);
            graph.offsets.resize(numNodes + 1, 0);
        } else if (line[0] == 'a') {
            int u, v, label, weight;
            sscanf(line.c_str(), "a %d %d %d %d", &u, &v, &label, &weight);
            
            // Keep the edge with the largest weight
            std::pair<int, int> edge = std::make_pair(u, v);
            if (edgeWeights.find(edge) == edgeWeights.end() || weight > edgeWeights[edge]) {
                graph.edges[graph.offsets[u] * 2] = v;
                graph.edge_labels[graph.offsets[u]] = label;
                edgeWeights[edge] = weight;
                graph.offsets[u]++;
            }
        }
    }

    // Update offsets to represent prefix sum
    for (int i = 1; i <= numNodes; i++) {
        graph.offsets[i] += graph.offsets[i - 1];
    }

    return graph;
}

// Function to write a graph in CSR representation to a file in DIMACS format
void writeDIMACS(const std::string& filename, const Graph& graph) {
    std::ofstream file(filename);
    file << "p sp " << graph.offsets.size() - 1 << " " << graph.edge_labels.size() << "\n";

    for (int u = 1; u < graph.offsets.size(); u++) {
        for (int i = graph.offsets[u - 1]; i < graph.offsets[u]; i++) {
            int v = graph.edges[i * 2];
            int label = graph.edge_labels[i];
            file << "a " << u << " " << v << " " << label << "\n";
        }
    }
}

int main() {
    // Replace with your input and output filenames
    std::string inputFilename = "wiki.dimacs";
    std::string outputFilename = "output.dimacs";

    Graph graph = readDIMACS(inputFilename);

    writeDIMACS(outputFilename, graph);

    return 0;
}
