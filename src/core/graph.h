#pragma once
#include <vector>
using namespace std;

class Edge{
public:
    int v1, v2, capacitance, flow, residual ;
    Edge(int _v1, int _v2, int cap, int f, int r):v1(_v1), v2(_v2), capacitance(cap), flow(f), residual(r){}
};

class Graph{
    int n ; //number of vertexes
    int e ; //number of edges
    vector< vector<int> > edges;  //graph adjacency list
    vector< vector<Edge> > weightedEdges ; //weighted graph adjacency list for maximum-flow algorithm
    bool directed;
    
    //BFS variables:
    vector<bool> discovered;
    vector<int> parent;
    vector<bool> process;
    
    //Ford-Fulkerson variables:
    int cap[1000][1000];
    // flow network
    int fnet[1000][1000];    
    // BFS
    int q[1000], qf, qb, prev[1000];

public:
    Graph(int,bool);
    void addEdge(int,int);
    vector< vector<int> > getAdjacencyList();
    bool isDirected();
    void printGraph();
    
    void bfs(int);
    void dfs(int);
    void dfsRecursive(int);
    vector<int> findPath(int, int);
    int distance(int , int);
    void connectedComponents();
    void topologicalSorting();
    int networkFlow(int,int);
};

