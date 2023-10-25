#include <iostream>
#include <fstream>
#include <vector>
#include <functional>
#include <algorithm>
#include <sstream>
#include <math.h>

struct CSRGraph {
    int numNodes;
    int numEdges;
    std::string format;
    std::vector<int> rp;
    std::vector<int> ci;
    std::vector<int> ai;
    std::vector<long double> labels;
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
    std::cout << std::endl << "ci: ";
    for (int x : graph.ci) {
        std::cout << x << " ";
    }
    std::cout << std::endl << "ai: ";
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
    std::string format;

    std::istringstream iss(line);
    iss >> token >> format >> numNodes >> numEdges;

    //populate COO
    std::vector<Edge> COO;
    while (std::getline(file, line)) {
        if (line[0] == 'a') {
            int s, d, w;
            if (sscanf(line.c_str(), "a %d %d %d", &s, &d, &w) == 3 && w != 0) {
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

    //convert COO to CSR
    CSRGraph graph;
    graph.numNodes = numNodes;
    graph.numEdges = COO.size();
    graph.format = format;
    graph.rp.push_back(0);
    graph.ci.push_back(0);
    graph.ai.push_back(0);
    int cur = 0;
    for(int i = 0; i < COO.size(); i++){
        while(COO[i].s > cur) {
            cur++;
            graph.rp.push_back(graph.ci.size());
        }
        graph.ci.push_back(COO[i].d);
        graph.ai.push_back(COO[i].w);
    }
    while(cur <= graph.numNodes) {
        graph.rp.push_back(graph.ci.size());
        cur++;
    }

    return graph;
}

// Function to write a graph in CSR representation to a file in DIMACS format
void writeDIMACS(const std::string& filename, const CSRGraph& graph) {
    std::ofstream file(filename);
    file << "p " << graph.format << " " << graph.numNodes << " " << graph.numEdges << "\n";
    int cur = 1;
    for(int i = 2; i < graph.rp.size(); i++) {
        int index = graph.rp[i];
        for(int j = cur; j < index; j++) {
            file << "a " << i - 1<< " " << graph.ci[j] << " " << graph.ai[j] << "\n";
        }
        cur = index;
    }
}

// Function to print the number and label of all nodes
void printNodeNumbersLabels(const std::string& filename, const CSRGraph& graph){
    std::ofstream file(filename);
    for(int i = 1; i < graph.numNodes + 1; i++) {
        file << i << " " << graph.labels[i] << "\n";
    }
}

// Function to compute the page rank of a graph in CSR representation
void pageRank(CSRGraph& graph) {
    long double ranks[graph.numNodes + 1];
    const double d = .85;
    for(int i = 1; i < graph.numNodes + 1; i++) {
        ranks[i] = (double)1/graph.numNodes;
    }
    
    //count weights
    long long outWeight[graph.numNodes + 1];
    for(int i = 1; i < graph.numNodes + 1; i++) {
        outWeight[i] = 0;
        for(int j = graph.rp[i]; j < graph.rp[i+1]; j++) {
            outWeight[i] += graph.ai[j];
        }
    }

    //run until precision reached
    bool finished = false;
    while(!finished) {
        float tempRanks[graph.numNodes + 1];
        for(int i = 0; i < graph.numNodes + 1; i++) {
            tempRanks[i] = (1-d)/graph.numNodes;
        }

        int cur = 1;
        for(int i = 2; i < graph.rp.size(); i++) {
            int index = graph.rp[i];
            for(int j = cur; j < index; j++) {
                if(outWeight[i - 1] == 0){
                    std::cout << "divide 0 " << i - 1 << " " << graph.ci[j] << std::endl;
                }
                tempRanks[graph.ci[j]] += d * ranks[i - 1] * ((double) graph.ai[j] / outWeight[i - 1]);
                //tempRanks[graph.ci[j]] += d * ranks[i - 1]/(index - cur);
            }
            cur = index;
        }
        
        finished = true;
        for(int i = 1; i < graph.numNodes + 1; i++){
            if(fabs(tempRanks[i] - ranks[i]) > .0001){
                finished = false;
            }
            ranks[i] = tempRanks[i];
        }
    }


    //normalize
    long double sum = 0;
    for(int i = 1; i < graph.numNodes + 1; i++){
        sum += ranks[i];
    }
    for(int i = 1; i < graph.numNodes + 1; i++) {
        ranks[i] = ranks[i]/sum;
    }

    //store ranks
    graph.labels.push_back(0.0);
    for(int i = 1; i < graph.numNodes + 1; i++){
        graph.labels.push_back(ranks[i]);
    }
}

//writes a file with the outdegrees of every node
void nodeDegreeHistogram(const std::string& filename, CSRGraph& graph) {
    std::ofstream file(filename);
    
    int degrees[graph.numNodes + 1] = {0};
    int cur = 1;
    for(int i = 2; i < graph.rp.size(); i++) {
        int index = graph.rp[i];
        for(int j = cur; j < index; j++) {
            degrees[graph.ci[j]]++;
        }
        cur = index;
    }
    
    for(int i = 1; i < graph.numNodes + 1; i++) {
        file << i << " " << degrees[i] << "\n";
    }

}

int main(int argc, char** argv) {
    std::string name(argv[1]);
    // Replace with input and output filenames
    std::string inputFilename = name + ".dimacs";
    std::string outputDimacsFilename = name + "Output.dimacs";
    std::string outputNodeLabelFilename = name + "NodeLabel.txt";

    CSRGraph graph = readDIMACS(inputFilename);
    pageRank(graph);
    //nodeDegreeHistogram(name + "Outgoing.txt", graph);
    printNodeNumbersLabels(outputNodeLabelFilename, graph);
    writeDIMACS(outputDimacsFilename, graph);

    return 0;
}