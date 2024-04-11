const int inf = 1e9;
// const real one = 1;

struct request {
    int v, t, s, d, l, u;
    long long taker = -1;
    void read(ifstream *fi) {
        (*fi) >> v >> t >> s >> d >> l >> u;
    }

    void write() {
        cout << v << ' ' << t << ' ' << s << ' ' << d << ' ' << l << ' ' << u << '\n';
    }
};

struct vehicle {
    int loc, curQ, aTime;
    void set(int v_0, int cap, int av) {
        loc = v_0;
        curQ = cap;
        aTime = av;
    }
}stve;

struct action {
    int ty, st, et, ve, req;
    //ty: 0 -> travel, 1 -> serve;
};

struct environment {
    real scailing, scailing1;
    int N, v_0, k, Q, T, longestPath, counter;
    vector<vector<real> > c;
    vector<request> reqList, reqList1;

    //read a test;
    void read(ifstream *fi) {
        longestPath = 0;
        (*fi) >> N >> v_0 >> k >> Q >> T;
        c.resize(N);
        for (int i = 0; i < N; i++) {
            c[i].resize(N);
            for (int j = 0; j < N; j++) {
                (*fi) >> c[i][j];
                if (longestPath < c[i][j]) {
                    longestPath = c[i][j];
                }
            }
        }
        for (int k = 0; k < N; k++) {
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    if (c[i][j] > c[i][k] + c[k][j]) {
                        c[i][j] = c[i][k] + c[k][j];
                    }
                }
            }
        }
        int req;
        (*fi) >> req;
        while (req--) {
            request tmp;
            tmp.read(fi);
            reqList.push_back(tmp);
        }
        sort(reqList.begin(), reqList.end(), [](request x, request y) {
            return x.t < y.t;
        });
    }

    //write a test
    void write() {
        cout << N << ' ' << v_0 << ' ' << k << ' ' << Q << ' ' << T << '\n';
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                cout << c[i][j] << ' ';
            }
            cout << '\n';
        }
        for (request tmp : reqList) {
            tmp.write();
        }
    }

    int currentTime;
    vector<int> acRequest, acRequest1;
    vector<vehicle> stateSet, stateSet1;
    vector<action> jobs, jobs1;
    //initialize the state
    inline void initialise() {
        counter = 0;
        scailing = one * longestPath / 100;
        scailing1 = one * longestPath / 8;
        currentTime = 0;
        acRequest.clear();
        stateSet.clear();
        stve.set(v_0, Q, 0);
        for (int i = 0; i < k; i++) {
            stateSet.push_back(stve);
        }
        for (int i = 0; i < reqList.size(); i++) {
            reqList[i].taker = -1;
        }
        jobs.clear();
    }

    //executes an action
    inline void executes(action &x) {
        if (x.req != -1 && reqList[x.req].taker != -1 && reqList[x.req].taker != x.ve) {
            return;
        }
        if (x.ty == 0) {
            stateSet[x.ve].aTime = x.et;
            if (x.req != -1) {
                stateSet[x.ve].loc = reqList[x.req].v;
                reqList[x.req].taker = x.ve;
                stateSet[x.ve].aTime += reqList[x.req].s;
                stateSet[x.ve].curQ -= reqList[x.req].d;
                for (int i = acRequest.size() - 1; i >= 0; i--) {
                    if (acRequest[i] == x.req) {
                        swap(acRequest[i], acRequest[acRequest.size() - 1]);
                        acRequest.pop_back();
                        break;
                    }
                }
            }
            else {
                stateSet[x.ve].loc = v_0;
            }
        }
        else {
            stateSet[x.ve].aTime = x.et;
        }
    }

    //calculate the terminals
    vector<real> determinal(int pos, request x) {
        vehicle cur = stateSet[pos];
        vector<real> res;
        res.push_back(one * c[x.v][v_0] / T); //TD
        res.push_back(one * c[cur.loc][x.v] / T); //T
        res.push_back(one * max(x.l - currentTime, 0) / T);//OT
        res.push_back(one * (x.u - currentTime) / T);//CT
        res.push_back(one * x.s / T);//ST
        res.push_back(one * cur.curQ / Q);//Q
        res.push_back(one * x.d / Q);//DEM
        real dis = 0;
        for (int i = 0; i < N; i++) {
            if (i == x.v) {
                continue;
            }
            dis += exp(-(1/2) * pow(one * c[i][x.v] / scailing, 2));
        }
        res.push_back(dis);//VTD
        dis = 0;
        for (int i = 0; i < k; i++) {
            if (i == pos) {
                continue;
            }
            dis += exp(-(1/2) * pow(one * c[stateSet[i].loc][x.v] / scailing1, 2));
        }
        res.push_back(dis);//VHD
        dis = inf;
        for (int i = 0; i < k; i++) {
            if (i == pos) {
                continue;
            }
            dis = min(dis, c[stateSet[i].loc][x.v]);
        }
        res.push_back(dis);//TOV
        res.push_back(1);//constant 1
        assert(res.size() == 11);
        return res;
    }

    //generate a Solution with a heuristic
    vector<action> generateSolution(node *h) {
        vector<action> res;
        real fit;
        vector<real>terset;
        int cnt = 0;
        while (true) {
            int i = 0, r = -1, dep, arr, clr = 0;
            if (acRequest.empty()) {
                break;
            }
            if (stateSet[0].loc != v_0) {
                clr = 1;
            }
            //choose the avalible vehicle
            for (int j = 1; j < k; j++) {
                if (stateSet[j].loc != v_0) {
                    clr = 1;
                }
                if (stateSet[i].aTime > stateSet[j].aTime) {
                    i = j;
                }
            }

            currentTime = max(currentTime, stateSet[i].aTime);
            if (currentTime > T) {
                break;
            }
            //choose the request
            for (int j = 0; j < acRequest.size(); j++) {
                request tmp = reqList[acRequest[j]];
                if (tmp.taker != -1 || tmp.d > stateSet[i].curQ || currentTime + c[stateSet[i].loc][tmp.v] > tmp.u) {
                    continue;
                }
                terset = determinal(i, tmp);
                if (r == -1) {
                    fit = h->calc(&terset);
                    r = j;
                }
                else {
                    real pot = h->calc(&terset);
                    if (fit > pot) {
                        fit = pot;
                        r = j;
                    }
                }
            }
            if (r == -1) {
                if (!clr) {
                    break;
                }
                //go back to the depot
                if (stateSet[i].loc == v_0 && stateSet[i].curQ == Q) {
                    stateSet[i].aTime = T + 1;
                }
                else {
                    res.push_back({0, currentTime, currentTime + (int)c[stateSet[i].loc][v_0], i, -1});
                    stateSet[i] = {v_0, Q, currentTime + (int)c[stateSet[i].loc][v_0]};
                }
            }
            else {
                //dep is the departure time
                dep = max(currentTime, reqList[acRequest[r]].l - (int)c[stateSet[i].loc][reqList[acRequest[r]].v]);
                //arr is the arrive time
                arr = dep + c[stateSet[i].loc][reqList[acRequest[r]].v];
                res.push_back({0, dep, arr, i, acRequest[r]});
                res.push_back({1, arr, arr + reqList[acRequest[r]].s, i, acRequest[r]});
                stateSet[i].loc = reqList[acRequest[r]].v;
                stateSet[i].curQ -= reqList[acRequest[r]].d;
                stateSet[i].aTime = arr + reqList[acRequest[r]].s;
                reqList[acRequest[r]].taker = i;
                swap(acRequest[r], acRequest[acRequest.size() - 1]);
                acRequest.pop_back();
            }
        }

        for (int tmp : acRequest) {
            if (reqList[tmp].taker == -1) {
                res.clear();
                return res;
            }
        }
        return res;
    }

    //new request arrives
    inline bool requestArrives(int newReq, node *h) {
        int response = true;
        currentTime = reqList[newReq].t;
        sort(jobs.begin(), jobs.end(), [](action x, action y) {
            return x.st > y.st;
        });
        while (!jobs.empty()) {
            action tmp = jobs.back();
            if (tmp.st <= currentTime) {
                executes(tmp);
                jobs.pop_back();
            }
            else {
                break;
            }
        };
        acRequest1 = acRequest;
        stateSet1 = stateSet;
        jobs1 = jobs;
        acRequest.push_back(newReq);
        reqList1 = reqList;
        jobs = generateSolution(h);
        acRequest = acRequest1;
        stateSet = stateSet1;
        currentTime = reqList[newReq].t;
        reqList = reqList1;
        if (jobs.empty()) {
            response = false;
            jobs = jobs1;
        }
        else {
            acRequest.push_back(newReq);
        }
        return response;
    }

    //executes meta-heuristic algorithm
    int metaHeuristic(node *h) {
        initialise();
        for (int i = 0; i < reqList.size(); i++) {
            if (requestArrives(i, h)) {
                counter++;
            }
        }
        return counter;
    }
};

environment env[numberOfTests];

long long fitness(node *h) {
    long long sum = 0;
    for (int i = 0; i < numberOfTests; i++) {
        sum += env[i].metaHeuristic(h);
    }
    return sum;
}