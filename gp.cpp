const int populationSize = 100;
const int numberOfGenerations = 1000;
const int numberOfElite = 20;
const real selectionRate = 0.8;
const int selectionSize = 5;
const int crossOverSize = 80;
const int mutationRate = 0.6;
const int mutationSize = 15;
const int eliteMutateSize = 2;
const int eliteMutateRate = 0.3;
const bool mutateWhileCrossOverMode = true;
const string space = "                ";

int cmp(pair<node*, long long> x, pair<node*, long long> y) {
    return x.se > y.se;
}

const pair<node*, long long> basis = {nullptr, -1};
pair<node*, long long> XX, XY;

struct geneticProgramming {
    int genidx = 0;
    vector<pair<node*, long long> > gen, ds;
    pair<node*, long long> best = {nullptr, -1};
    vector<pair<int, int> >couple;

    void terminate(vector<pair<node*, long long> > *x) {
        for (int i = 0; i < x->size(); i++) {
            x->at(i).fi->remove();
            x->at(i).fi = nullptr;
        }
        x->clear();
    }

    //calculate the fitness value
    void calculateFitness() {
        cerr << "Generation " << genidx << ":" << endl;
        for (int i = 0; i < populationSize; i++) {
            if (gen[i].se == -1) {
                cerr << "calculating fitness value of number " << i << '\r';
                gen[i].se = fitness(gen[i].fi);
                if (gen[i].se > best.se) {
                    if (best.fi != nullptr) {
                        best.fi->remove();
                        best.fi = new node;
                        best.fi->copy(gen[i].fi);
                        best.se = gen[i].se;
                    }
                    else {
                        best.fi = new node;
                        best.fi->copy(gen[i].fi);
                        best.se = gen[i].se;
                    }
                }
            }
            
        }
        sort(gen.begin(), gen.end(), cmp);
        cerr << "best finness value: " << gen[0].se << space << endl;
        fgp << "Generation " << genidx << ":\n";
        gen[0].fi->print(&fgp);
        fgp << '\n';
        // gen[0].fi->rawprint();
        // fgp << '\n';
        fgp << "Fitness value: " << gen[0].se << '\n';
    }

    //initialize the first generations
    void initailPopulation() {
        for (int i = 0; i < populationSize; i++) {
            for (int j = i + 1; j < populationSize; j++) {
                couple.push_back({i, j});
            }
        }
        gen.clear();
        gen.resize(populationSize, make_pair(nullptr, -1));
        for (int i = 0; i < populationSize; i++) {
            gen[i].fi = new node;
            gen[i].fi->randomize();
        }
        calculateFitness();
    }

    //selection process
    void selection() {
        for (int i = 0, lft = numberOfElite, need = selectionSize; i < numberOfElite && need; i++, lft--) {
            if (lft == need || rand01() < selectionRate) {
                gen.push_back(basis);
                gen[gen.size() - 1].fi = new node;
                gen[gen.size() - 1].fi->copy(ds[i].fi);
                gen[gen.size() - 1].se = ds[i].se;
                ds[i].se = -1;
                need--;
            }
        }
        static int copied;
        copied = eliteMutateSize;
        for (int i = 0; i < (int)gen.size(); i++) {
            if (copied > 0 && rand01() < eliteMutateRate) {
                copied--;
                mutate(gen[i].fi);
                gen[i].se = -1;
            }
        }
        assert(gen.size() == selectionSize);
    }

    //mutation process
    void mutation() {
        for (int i = 0, need = mutationSize, left = populationSize - selectionSize; i < populationSize && need; i++) {
            if (ds[i].se == -1) {
                continue;
            }
            if (need == left || rand01() < mutationRate) {
                gen.push_back(basis);
                gen[gen.size() - 1].fi = new node;
                gen[gen.size() - 1].fi->copy(ds[i].fi);
                mutate(gen[gen.size() - 1].fi);
                need--;
            }
            left--;
        }
        assert(gen.size() == selectionSize + mutationSize);
    }

    //crossover process
    void crossOver() {
        shuffle(couple.begin(), couple.end(), rng);
        for (int i = 0; i + i + 2 <= crossOverSize; i++) {
            XX = XY = {nullptr, -1};
            XX.fi = new node;
            XY.fi = new node;
            XX.fi->copy(ds[couple[i].fi].fi);
            XY.fi->copy(ds[couple[i].se].fi);
            cross(XX.fi, XY.fi);
            if (mutateWhileCrossOverMode) {
                mutate(XX.fi);
                mutate(XY.fi);
            }
            gen.push_back(XX);
            gen.push_back(XY);
        }
        assert(gen.size() == populationSize);
    }

    void dontForgetYourself() {
        best = {nullptr, -1};
        initailPopulation();
        for (genidx = 1; genidx <= numberOfGenerations; genidx++) {
            ds = gen;
            gen.clear();
            selection();
            mutation();
            crossOver();
            terminate(&ds);
            calculateFitness();
        }    
        terminate(&gen);
        cerr << "evolutionary computation completed \n";
        cerr << "best fitness value: " << best.se << '\n';
        fgp << "best individual:\n";
        best.fi->print(&fgp);
        fgp << "\nfitness value: ";
        fgp << best.se << '\n';
        fgp << "raw:\n";
        best.fi->rawprint(&fgp);
    }
}GP;