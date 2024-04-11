#include <bits/stdc++.h>

using namespace std;

int N = 20; //number of th vertex
int root = 0; //index of the depot
int K = 5;//number of vehicles
int Q = 1000;//maximum capacity
int T = 1000;//amount of time, a day has 86400 seconds
int minT = 100, maxT = 200; //bounds of service time
int req = 100;//number of requests
int numberOfTests = 20;
vector<vector<int> > a;//the edges set

mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());

double rand01()
{
    static const uint64_t MAX = 1ULL << 45;
    uint64_t a = (uint64_t)rng() % MAX;
    return (long double)a / MAX;
}

int randlr(int l, int r = -1) {
    if (r == -1) {
        return rng() % l;
    }
    return rng() % (r - l + 1) + l;
}

int make(string path) {
    ofstream f;
    f.open(path, ios::out);
    f << N << ' ' << root << ' ' << K << ' ' << Q << ' ' << T << '\n';
    a.resize(N);
    for (int i = 0; i < N; i++) {
        a[i].resize(N);
        for (int j = 0; j < N; j++) {
            if (i == j) {
                a[i][j] = 0;
            }
            else {
                if (j > i) {
                    a[i][j] = randlr(1, 100);
                }
                else {
                    a[i][j] = a[j][i];
                }
            }
            f << a[i][j] << ' ';
        }
        f << '\n';
    }
    f << req << '\n';
    for (int i = 1; i <= req; i++) {
        int u, v, t;
        v = randlr(1, T);
        u = randlr(0, v - 1);
        t = randlr(0, u);
        f << randlr(N) << ' ' << t << ' ';
        f << randlr(minT, maxT) << ' ' << randlr(0, Q) << ' ';
        f << u << ' ' << v << '\n';
    }
    return 0;
}

int main() {
    for (int i = 0; i < numberOfTests; i++) {
        make("./traintests/test" + to_string(i) + ".inp");
    }
}