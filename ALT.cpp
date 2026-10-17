#include <bits/stdc++.h>
using namespace std;

const int MAX_N = 2*1e4+7, MAX_M = 1e4+7;
const int INF = MAX_N + MAX_M;
const int ROOT = 1;

int n, m, u[MAX_N], v[MAX_N], x[MAX_M], y[MAX_M];
vector<pair<int,int>> adj[MAX_N];
bool visited[MAX_N];
int parent[MAX_N], guardian_num[MAX_N], depth[MAX_N], subtree_size[MAX_N], heavy[MAX_N];
int dfn[MAX_N], chain_head[MAX_N], counter = 0;

struct FlowEdge {
    int dest;
    int idx_rev;
    int capacity;
};

const int MAX_V = MAX_M + 4*MAX_N, S = 0, T = MAX_V-1;
const int CITIZEN_SHIFT = MAX_V-1-MAX_M;

const int ST_ROOT= 1; // segment tree root

vector<FlowEdge> flow_graph[MAX_V];
int guardian_to_leaf[MAX_N];

int levels[MAX_V], start[MAX_V];

bool reachable[MAX_V];

// compute the parameters required for HLD: parent, guardian_num, depth, subtree_size, heavy
void preparation_dfs_hld(int v) {
    visited[v] = true;
    int heavy_child = -1, heavy_size = -1;
    
    for (int i=0; i<adj[v].size(); i++) {
        int neighbour = adj[v][i].first;
        int edge_num = adj[v][i].second;
        
        if (!visited[neighbour]) {
            parent[neighbour] = v;
            guardian_num[neighbour] = edge_num;
            depth[neighbour] = depth[v] + 1;
            
            preparation_dfs_hld(neighbour);
            
            subtree_size[v] += subtree_size[neighbour];
            if (subtree_size[neighbour] > heavy_size) {
                heavy_child = neighbour;
                heavy_size = subtree_size[neighbour];
            }
        }
    }
    subtree_size[v]++;
    heavy[v] = heavy_child;
}

// assign HLD numeration so that every heavy chain holds a contiguous range
void numeration_dfs_hld(int v, int head) {
    visited[v] = true;
    dfn[v] = counter++;
    chain_head[v] = head;

    // heavy child has priority over light children
    if (heavy[v] != -1)
        numeration_dfs_hld(heavy[v], head);

    for (int i=0; i<adj[v].size(); i++) {
        int neighbour = adj[v][i].first;

        // every light child is a beginning of a new chain in HLD
        if (!visited[neighbour])
            numeration_dfs_hld(neighbour, neighbour);
    }
}

void add_segment_tree_to_flow_graph(int v, int left, int right) {
    if (left == right) { // connect every leaf [i,i] with T (sink) except [1,1]
        guardian_to_leaf[left] = v;
        if (left != 1) {
            flow_graph[v].push_back(FlowEdge{T, flow_graph[T].size(), 1});
            flow_graph[T].push_back(FlowEdge{v, flow_graph[v].size()-1, 0});
        }
        return;
    }
    
    int left_child = v*2, right_child = v*2+1;
    flow_graph[v].push_back(FlowEdge{left_child, flow_graph[left_child].size(), INF});
    flow_graph[left_child].push_back(FlowEdge{v, flow_graph[v].size()-1, 0});
    flow_graph[v].push_back(FlowEdge{right_child, flow_graph[right_child].size(), INF});
    flow_graph[right_child].push_back(FlowEdge{v, flow_graph[v].size()-1, 0});
    
    int mid = (left+right) / 2;
    add_segment_tree_to_flow_graph(left_child, left, mid);
    add_segment_tree_to_flow_graph(right_child, mid+1, right);
}

vector<pair<int,int>> division_into_hld_intervals (int beg, int end) {
    vector<pair<int,int>> intervals;

    while(chain_head[beg] != chain_head[end]) {
        if (depth[chain_head[beg]] > depth[chain_head[end]]) {
            intervals.push_back({dfn[chain_head[beg]], dfn[beg]});
            beg = parent[chain_head[beg]];
        }
        else {
            intervals.push_back({dfn[chain_head[end]], dfn[end]});
            end = parent[chain_head[end]];
        }
    }
    if (beg != end)
        intervals.push_back({min(dfn[beg], dfn[end])+1, max(dfn[beg], dfn[end])});

    return intervals;
}

void add_interval_to_flow_graph(int citizen_idx, int beg, int end, int v, int left, int right) {
    if (beg <= left && right <= end) {
        flow_graph[citizen_idx].push_back(FlowEdge{v, flow_graph[v].size(), INF});
        flow_graph[v].push_back(FlowEdge{citizen_idx, flow_graph[citizen_idx].size()-1, 0});
        return;
    }

    int left_child = v*2, right_child = v*2+1;
    int mid = (left + right) / 2;
    if (beg <= mid)
        add_interval_to_flow_graph(citizen_idx, beg, end, left_child, left, mid);
    if (end > mid)
        add_interval_to_flow_graph(citizen_idx, beg, end, right_child, mid+1, right);
}

// adding citizens' paths to the flow graph using the HLD ordering
void add_citizens_to_flow_graph() {
    for (int i=0; i<m; i++) {
        int citizen_idx = CITIZEN_SHIFT+i;

        flow_graph[S].push_back(FlowEdge{citizen_idx, flow_graph[citizen_idx].size(), 1});
        flow_graph[citizen_idx].push_back(FlowEdge{S, flow_graph[S].size()-1, 0});

        vector<pair<int,int>> intervals = division_into_hld_intervals(x[i], y[i]);
        while (!intervals.empty()) {
            pair<int, int> interval = intervals.back();
            intervals.pop_back();

            add_interval_to_flow_graph(citizen_idx, interval.first, interval.second, ST_ROOT, 1, n);
        }
    }
}

// build the level graph for one round of Dinic's algorithm
bool determine_levels_bfs(int source) {
    levels[source] = 1;
    queue<int> ver;
    ver.push(source);

    while (!ver.empty()) {
        int v = ver.front();
        ver.pop();

        for (int i=0; i<flow_graph[v].size(); i++) {
            int neighbour = flow_graph[v][i].dest;
            int capacity = flow_graph[v][i].capacity;

            if (capacity > 0 && levels[neighbour] == 0) {
                levels[neighbour] = levels[v] + 1;
                ver.push(neighbour);
            }
        }
    }

    return (levels[T] != 0);
}

int dinic_flow_dfs(int v, int flow) {
    if (v == T)
        return flow;

    for (int i=start[v]; i<flow_graph[v].size(); i++) {
        int neighbour = flow_graph[v][i].dest;
        int idx_rev = flow_graph[v][i].idx_rev;
        int max_cap = flow_graph[v][i].capacity;

        if (max_cap > 0 && levels[neighbour] == levels[v] + 1) {
            int ret = dinic_flow_dfs(neighbour, min(flow, max_cap));
            if (ret > 0) {
                flow_graph[v][i].capacity -= ret;
                flow_graph[neighbour][idx_rev].capacity += ret;
                return ret;
            }
        }
        
        start[v]++;
    }
    
    return 0;
}

void find_reachable_vertices_from_S(int v) {
    reachable[v] = true;

    for(int i=0; i<flow_graph[v].size(); i++) {
        int neighbour = flow_graph[v][i].dest;
        int capacity = flow_graph[v][i].capacity;

        if (capacity > 0 && reachable[neighbour] == false)
            find_reachable_vertices_from_S(neighbour);
    }
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    // read the input
    
    cin >> n >> m;
    for (int i=0; i<(n-1); i++) {
        cin >> u[i] >> v[i];
        adj[u[i]].push_back({v[i], i+1});
        adj[v[i]].push_back({u[i], i+1});
    }
    for (int i=0; i<m; i++)
        cin >> x[i] >> y[i];

    // HLD part

    depth[ROOT] = 0;
    fill(visited, visited+MAX_N, 0);
    preparation_dfs_hld(ROOT);
    fill(visited, visited+MAX_N, 0);
    counter = 1;
    numeration_dfs_hld(ROOT, ROOT);

    // creation of flow graph

    add_segment_tree_to_flow_graph(ST_ROOT, 1, n);
    add_citizens_to_flow_graph();

    // compute the maximum flow - Dinic's algorithm

    fill(levels, levels+MAX_V, 0);
    while (determine_levels_bfs(S)) {
        int ret = 0;
        fill(start, start+MAX_V, 0);

        do {
            ret = dinic_flow_dfs(S, INF);
        } while (ret != 0);

        fill(levels, levels+MAX_V, 0);
    }

    // find the minimum cut

    fill(reachable, reachable+MAX_V, false);
    find_reachable_vertices_from_S(S);

    vector<int> citizens_with_puppy, guardians_with_puppy;

    for (int i=0; i<m; i++) {
        int citizen_idx = i + CITIZEN_SHIFT;
        
        if (reachable[citizen_idx] == false)
            citizens_with_puppy.push_back(i+1);
    }

    for (int i=2; i<=n; i++) {
        int leaf = guardian_to_leaf[dfn[i]];

        if (reachable[leaf] == true)
            guardians_with_puppy.push_back(guardian_num[i]);
    }

    // print the output

    cout << citizens_with_puppy.size() + guardians_with_puppy.size() << '\n';
    cout << citizens_with_puppy.size();
    for (int i=0; i<citizens_with_puppy.size(); i++)
        cout << ' ' << citizens_with_puppy[i];
    cout << '\n';
    cout << guardians_with_puppy.size();
    for (int i=0; i<guardians_with_puppy.size(); i++)
        cout << ' ' << guardians_with_puppy[i];
    cout << '\n';

    return 0;
}