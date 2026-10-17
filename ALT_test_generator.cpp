#include <bits/stdc++.h>
using namespace std;

const int MAX_N = 29, MAX_M = 30;

int leader[MAX_N], siz[MAX_N];

int find(int v) {
    if (leader[v] == v)
        return v;
    leader[v] = find(leader[v]);
    return leader[v];
}

void unio(int u, int v) {
    int leader_u = find(u);
    int leader_v = find(v);

    if (siz[leader_u] > siz[leader_v]) {
        leader[leader_v] = leader_u;
        siz[leader_u] += siz[leader_v];
    }
    else {
        leader[leader_u] = leader_v;
        siz[leader_v] += siz[leader_u];
    }
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    
    srand(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());

    int n = rand() % MAX_N + 2;
    int m = rand() % MAX_M + 1;
    cout << n << ' ' << m << '\n';

    for (int i=1; i<=n; i++)
        leader[i] = i;
    fill(siz, siz+MAX_N, 1);

    for (int i=0; i<(n-1); i++) {
        bool success = false;
        while (!success) {
            int u = rand() % n + 1;
            int v = rand() % n + 1;

            if (find(u) != find(v)) {
                cout << u << ' ' << v << '\n';
                unio(u, v);
                success = true;
            }
        }
    }

    for (int i=0; i<m; i++) {
        bool success = false;
        while(!success) {
            int x = rand() % n + 1;
            int y = rand() % n + 1;

            if (x != y) {
                cout << x << ' ' << y << '\n';
                success = true;
            }
        }
    }
    return 0;
}
