/*
    time complexity: O(m * 2^m)
*/

#include <bits/stdc++.h>
using namespace std;

const int MAX_M = 1e4+7, MAX_N = 2*1e4+7;
const int ROOT = 1;

int n, m, u[MAX_N], v[MAX_N], x[MAX_M], y[MAX_M];
vector<pair<int,int>> adj[MAX_N];
bool visited[MAX_N];
pair<int,int> parent[MAX_N];
int path[MAX_M];


void determine_parents(int v) {
    visited[v] = true;
    for (int i=0; i<adj[v].size(); i++) {
        int neighbour = adj[v][i].first;
        int edge_num = adj[v][i].second;
        if (!visited[neighbour]) {
            parent[neighbour] = {v, edge_num};
            determine_parents(neighbour);
        }
    }
}

int get_path (int x, int y) {
    int path = 0;
    while (x != ROOT) {
        path ^= (1<<parent[x].second);
        x = parent[x].first;
    }
    while (y != ROOT) {
        path ^= (1<<parent[y].second);
        y = parent[y].first;
    }
    return path;
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    cin >> n >> m;
    for (int i=0; i<(n-1); i++) {
        cin >> u[i] >> v[i];
        adj[u[i]].push_back({v[i], i});
        adj[v[i]].push_back({u[i], i});

    }
    fill(visited, visited+MAX_N, 0);
    determine_parents(ROOT);
    for (int i=0; i<m; i++) {
        cin >> x[i] >> y[i];
        path[i] = get_path(x[i], y[i]);
    }

    int min_k = m+n-1, citizens = (1<<m)-1, guardians = (1<<(n-1))-1;

    for (int i=0; i<(1<<m); i++) {
        int guardians_with_puppy = 0;
        for (int j=0; j<m; j++) {
            if (!(i & (1<<j)))
                guardians_with_puppy |= path[j];
        }

        int curr = __builtin_popcount(i)+__builtin_popcount(guardians_with_puppy);
        if (curr < min_k) {
            min_k = curr;
            citizens = i;
            guardians = guardians_with_puppy;
        }
    }

    int how_many_citizens =  __builtin_popcount(citizens);
    int how_many_guardians = __builtin_popcount(guardians);

    cout << min_k << '\n';
    cout << how_many_citizens;
    for (int j=0; j<m; j++) {
        if (citizens & (1<<j))
            cout << ' ' << (j+1);
    }
    cout << '\n';
    cout << how_many_guardians;
    for (int j=0; j<n; j++) {
        if (guardians & (1 << j))
            cout << ' ' << (j+1);
    }
    cout << '\n';

    return 0;
}