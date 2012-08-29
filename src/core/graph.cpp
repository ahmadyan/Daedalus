#include "graph.h"
#include <iostream>
#include <cstring>	//for memset on windows
#include <deque>
using namespace std;

//Stupid graph class. instead of this garbage use vector. so there is no need for maxEdges or whatevers
Graph::Graph(int _n, bool _directed){
    n=_n;
    directed=_directed;
    e=0;
    edges = vector< vector<int> >(n);
}

void Graph::addEdge(int x, int y){
    edges[x].push_back(y);
    if(directed==false) edges[y].push_back(x);
    e++;
}

void Graph::printGraph(){
    for(int i=0;i<edges.size();i++){
        cout << i << "> " ;
        for(int j=0;j<edges[i].size();j++){
            cout << edges[i][j] << " " ;
        }
        cout << endl ;
    }
}


vector< vector<int> > Graph::getAdjacencyList(){
    return edges ;
}

bool Graph::isDirected(){
    return directed;
}


void Graph::bfs(int start){
    int c=0;
    //if(discovered!=NULL) delete discovered;
    //if(parent!=NULL) delete parent;
    //if(process!=NULL) delete process;
    
    discovered = vector<bool>(n);
    parent = vector<int>(n);
    process = vector<bool>(n) ;
  
    for(int i=0;i<n;i++){
        discovered[i]=false;
        process[i]=false;
        parent[i]=-1;
    }
  
    deque<int> q ;
    
    q.push_back(start);
    discovered[start]=true ;
    while(q.size()>0){
        int v = q.front();
        q.pop_front();
        process[v]=true ;
        //cout << v << " " ;
        c++ ;
        //process-the-vertex
        for(int i=0;i<edges[v].size();i++){
            if(discovered[edges[v][i]]==false){
                q.push_back(edges[v][i]);
                discovered[edges[v][i]]=true;
                parent[edges[v][i]]=v ;
            }
            if(process[edges[v][i]]==false){
                //process-the-edge
            }
        }
    }
 }


vector<int> Graph::findPath(int start, int end){
    bfs(start); //if we've note performed BFS, do it now.
    vector<int> nodes ;
    if( (start==end) || (end==-1) ){
        nodes.push_back(start);
        return nodes;
    }else{
        nodes = findPath(start, parent[end]);
        nodes.push_back(end);
        return nodes ;
    }
}

int Graph::distance(int start, int end){
    vector<int> v = findPath(start, end);
    return v.size();
}

void Graph::dfs(int start){
    discovered = vector<bool>(n);
    parent = vector<int>(n);
    process = vector<bool>(n) ;
    
    for(int i=0;i<n;i++){
        discovered[i]=false;
        process[i]=false;
        parent[i]=-1;
    }
    dfsRecursive(start);
}

void Graph::dfsRecursive(int v){
    //if(finished) return ; //in case we need a termination rule.
    discovered[v]=true;
    //processVertex(v);
    for(int i=0;i<edges[v].size();i++){
        if(discovered[edges[v][i]]==false){
            parent[edges[v][i]]=v;
            dfsRecursive(edges[v][i]);
        }else{
            if(process[edges[v][i]]==false){
                //processEdge(v,y);
            }
        }
    }
    process[v]=true;
}

void Graph::connectedComponents(){
    int c=0;
    discovered = vector<bool>(n);
    parent = vector<int>(n);
    process = vector<bool>(n) ;
    
    for(int i=0;i<n;i++){
        discovered[i]=false;
        process[i]=false;
        parent[i]=-1;
    }

    for(int i=0;i<n;i++){
        if(discovered[i]==false){
            c++;
            dfsRecursive(i);
        }
    }
}

int Graph::networkFlow(int s, int t){
    if(s==t) return 0;
    memset( cap, 0, sizeof( cap ) );
    for(int i=0;i<n;i++){
        for(int j=0;j<edges[i].size();j++){
            cap[i][edges[i][j]]=1;
        }
    }
    
    // init the flow network
    memset( fnet, 0, sizeof( fnet ) );
    
    int flow = 0;
    
    while( true )
    {
        // find an augmenting path
        memset( prev, -1, sizeof( prev ) );
        qf = qb = 0;
        prev[q[qb++] = s] = -2;
        while( qb > qf && prev[t] == -1 )
            for( int u = q[qf++], v = 0; v < n; v++ )
                if( prev[v] == -1 && fnet[u][v] - fnet[v][u] < cap[u][v] )
                    prev[q[qb++] = v] = u;
        
        // see if we're done
        if( prev[t] == -1 ) break;
        
        // get the bottleneck capacity
        int bot = 0x7FFFFFFF;
        for( int v = t, u = prev[v]; u >= 0; v = u, u = prev[v] )
            if(bot>cap[u][v] - fnet[u][v] + fnet[v][u]) bot=cap[u][v] - fnet[u][v] + fnet[v][u];
        
        // update the flow network
        for( int v = t, u = prev[v]; u >= 0; v = u, u = prev[v] )
            fnet[u][v] += bot;
        
        flow += bot;
    }
    
    return flow;
}