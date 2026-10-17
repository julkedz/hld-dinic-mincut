#include <bits/stdc++.h>
using namespace std;

const int MAX_N = 2*1e4+7, MAX_M = 1e4+7;
const int ROOT = 1;

int n, m, u[MAX_N], v[MAX_N], x[MAX_M], y[MAX_M];
vector<pair<int,int>> adj[MAX_N];

int k;
int citizen_counter, guardian_counter;
bool citizen_puppy[MAX_M], guardian_puppy[MAX_N];

bool visited[MAX_N];
pair<int,int> parent[MAX_N];
int depth[MAX_N];

void determine_parents(int v) {
    visited[v] = true;
    for (int i=0; i<adj[v].size(); i++) {
        int neighbour = adj[v][i].first;
        int edge_num = adj[v][i].second;
        if (!visited[neighbour]) {
            parent[neighbour] = {v, edge_num};
            depth[neighbour] = depth[v] + 1;
            determine_parents(neighbour);
        }
    }
}

bool check_path (int x, int y) {
    while (x != y) {
        if (depth[x] > depth[y]) {
            int guardian_num = parent[x].second;
            if (guardian_puppy[guardian_num] == false)
                return false;
            x = parent[x].first;
        }
        else {
            int guardian_num = parent[y].second;
            if (guardian_puppy[guardian_num] == false)
                return false;
            y = parent[y].first;
        }
    }

    return true;
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
    
    // read the output

    int num;
    cin >> k;
    cin >> citizen_counter;
    for (int i=0; i < citizen_counter; i++) {
        cin >> num;
        if (citizen_puppy[num-1] == true) {
            cout << "Doubled citizen\n";
            return 0;
        }
        citizen_puppy[num-1] = true;
    }
    cin >> guardian_counter;
    for (int i=0; i < guardian_counter; i++) {
        cin >> num;
        if (guardian_puppy[num] == true) {
            cout << "Doubled guardian\n";
            return 0;
        }
        guardian_puppy[num] = true;
    }

    if (citizen_counter + guardian_counter != k) {
        cout << "Output inconsistent with specification\n";
        return 0;
    }

    // checking if everyone is happy

    fill(visited, visited+MAX_N, 0);
    fill(depth, depth+MAX_N, 0);
    determine_parents(ROOT);

    vector<int> unhappy;
    for (int i=0; i<m; i++) {
        if (citizen_puppy[i] == true)
            continue;
        
        if (!check_path(x[i], y[i]))
            unhappy.push_back(i+1);
    }

    if (!unhappy.empty()) {
        cout << "Citizen(s) of number(s): ";
        for (int i=0; i<unhappy.size(); i++) {
            if (i != 0)
                cout << ", " << unhappy[i];
            else
                cout << unhappy[i];
        }
        cout << " won't be happy.\n";
    }
    else {
        cout << "OK\n";
    }
    
    return 0;
}