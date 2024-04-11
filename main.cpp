#include<bits/stdc++.h>
#define fi first
#define se second
#define real long double

const int numberOfTests = 1;

using namespace std;

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

ofstream fgp;

#include "func.cpp"
#include "fit.cpp"
#include "gp.cpp"

node *tree, *cay;

void inra() {
    fgp << "Tree 1: \n";
    fgp << "Size: " << tree->sz << '\n';
    tree->print(&fgp);
    fgp << "\n\n";
    fgp << "Tree 2: \n";
    fgp << "Size: " << cay->sz << '\n';
    cay->print(&fgp);
    fgp << "\n\n";
}

int main() {
    ifstream fi, inptree;
    ofstream fo;
    fgp.open("gp.out", ios::out);
    inptree.open("tree.inp", ios::in);
    // fo.open("file.out", ios::out);
    // freopen("file.inp", "r", stdin);
    // freopen("file.out", "w", stdout);
    for (int i = 0; i < numberOfTests; i++) {
        fi.open("./traintests/test" + to_string(i) + ".inp", ios::in);
        env[i].read(&fi);
        fi.close();
    }
    // tree = new node;
    // cay = new node;
    // tree->randomize();
    GP.dontForgetYourself();
    return 0;
}