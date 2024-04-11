vector<string> funcSet{"exp", "max", "+", "-", "*", "/", " "};
vector<string> terminalSet{"TD", "T", "OT", "CT", "ST", "Q", "DEM", "VTD", "VHD", "TOV", "1"};
//function and terminal set
const int funcSize = funcSet.size() - 1;
const int terminalSize = terminalSet.size();
const int maximalDepth = 8;
const real one = 1;

struct node{
    int type, val, sz;
    node *l, *r, *parent;
    node(int ctype = -1, int cval = 1) {
        type = ctype;
        val = cval;
        sz = 1;
        l = r = parent = nullptr;
    }

    //randomize a node with 
    //Pfunc is probability of funcion node
    void randomize(real Pfunc = 1) {
        sz = 1;
        if (rand01() <= Pfunc) {
            type = randlr(funcSize);
            l = new node();
            l->parent = this;
            l->randomize(Pfunc - one / maximalDepth);
            sz += l->sz;
            if (type > 0) {
                r = new node();
                r->parent = this;
                r->randomize(Pfunc - one / maximalDepth);
                sz += r->sz;
            }
            else {
                r = nullptr;
            }
        }
        else {
            type = funcSize;
            val = randlr(terminalSize);
            l = r = nullptr;
        }
    }

    //print the following subtree
    void print(ofstream *fo) {
        if (type == funcSize) {
            (*fo) << terminalSet[val];
        }
        else {
            if (type < 2) {
                (*fo) << funcSet[type];
            }
            (*fo) << "(";
            l->print(fo);
            if (type >= 2) {
                (*fo) << funcSet[type];
            }
            else {
                if (type == 1) {
                    (*fo) << ",";
                }
            }
            if (type) {
                r->print(fo);
            }
            (*fo) << ")";
        }
    }

    //read a subtree
    void read(ifstream *fi) {
        sz = 1;
        (*fi) >> type;
        if (type == funcSize) {
            (*fi) >> val;
        }
        else {
            l = new node;
            l->read(fi);
            sz += l->sz;
            l->parent = this;
            if (type) {
                r = new node;
                r->read(fi);
                sz += r->sz;
                r->parent = this;
            }
        }
    }

    //print the tree in raw data
    void rawprint(ofstream *fo) {
        (*fo) << type << ' ';
        if (type == funcSize) {
            (*fo) << val << '\n';
        }
        else {
            (*fo) << '\n';
        }
        if (l != nullptr) {
            l->rawprint(fo);
        }
        if (r != nullptr) {
            r->rawprint(fo);
        }
    }

    //remove a subtree
    void remove() {
        if (l != nullptr) {
            l->remove();
        }
        if (r != nullptr) {
            r->remove();
        }
        delete this;
    }

    //calc value of the function with given input
    real calc(vector<real> *input) {
        static real u, v;
        switch(type) {
            case 0: //exp
                return exp(l->calc(input));
            case 1: //max
                return max(l->calc(input), r->calc(input));
            case 2: //+
                return l->calc(input) + r->calc(input);
            case 3: //-
                return l->calc(input) - r->calc(input);
            case 4: //*
                return l->calc(input) * r->calc(input);
            case 5: ///
                u = l->calc(input);
                v = r->calc(input);
                return (!v) ? 1 : (u / v);
            default: //leaf
                return input->at(val);
        }
        return 0;
    }

    //copy from an existing tree
    void copy(node *s) {
        type = s->type;
        val = s->val;
        sz = s->sz;
        if (s->l != nullptr) {
            l = new node;
            l->copy(s->l);
            l->parent = this;
        }
        if (s->r != nullptr) {
            r = new node;
            r->copy(s->r);
            r->parent = this;
        }
    }

    node *goesto(int pot) {
        assert(pot <= sz);
        if (pot == 1) {
            return this;
        }
        else {
            if (pot - 1 <= l->sz) {
                assert(l != nullptr);
                return l->goesto(pot - 1);
            }
            else {
                assert(r != nullptr);
                return r->goesto(pot - 1 - l->sz);
            }
        }
        return nullptr;
    }
};

//random a number in [0, bound), exclude pre
int randEx(int pre, int bound) {
    for (int tmp = randlr(bound); true; tmp = randlr(bound)) {
        if (tmp != pre) {
            return tmp;
        }
    }
    return -1;
}

inline void mutateType1(node *rt) {
    static int chose, oldsz, newsz;
    chose = randlr(1, rt->sz);
    assert(chose <= rt->sz);
    node *k = rt->goesto(chose);
    oldsz = k->sz;
    if (k->type == funcSize) {
        k->val = randEx(k->val, terminalSize);
    }
    else {
        k->type = randEx(k->type, funcSize);
        if (k->type == 0) {
            if (rand01() < 0.5) {
                swap(k->l, k->r);
            }
            k->sz -= k->r->sz;
            k->r->remove();
            k->r = nullptr;
        }
        else {
            if (k->r == nullptr) {
                k->r = new node;
                k->r->parent = k;
                k->r->randomize(0.5);
                k->sz += k->r->sz;
            }
        }
    }
    newsz = k->sz;
    if (oldsz != newsz && k != rt) {
        while (true) {
            k = k->parent;
            k->sz += newsz - oldsz;
            if (k == rt) {
                break;
            }
        }
    }
}

inline void mutateType2(node *rt) {
    node *k;
    static int chose, oldsz, newsz;
    chose = randlr(2, rt->sz);
    k = rt->goesto(chose);
    oldsz = k->sz;
    if (k->l != nullptr) {
        k->l->remove();
    }
    if (k->r != nullptr) {
        k->r->remove();
    }
    assert(k != nullptr);
    k->randomize(0.8);
    assert(k != nullptr);
    newsz = k->sz;
    node *pre = k;
    if (oldsz != newsz) {
        while (true) {
            k = k->parent;
            k->sz += newsz - oldsz;
            if (k == rt) {
                break;
            }
        }
    }
}

inline void mutate(node *rt) {
    if (rand01() < 0.5) {
        mutateType1(rt);
    }
    else {
        mutateType2(rt);
    }
}

inline void cross(node *u, node *v) {
    static int chooseu, choosev;
    chooseu = randlr(2, u->sz);
    choosev = randlr(2, v->sz);
    static node *ku, *kv, *pu, *pv;
    ku = u->goesto(chooseu);
    kv = v->goesto(choosev);
    static int oldu, oldv;
    oldu = ku->sz;
    oldv = kv->sz;
    static int newu, newv;
    newu = kv->sz;
    newv = ku->sz;
    pu = ku->parent;
    pv = kv->parent;
    if (pu->l == ku) {
        pu->l = kv;
    }
    else {
        pu->r = kv;
    }
    if (pv->l == kv) {
        pv->l = ku;
    }
    else {
        pv->r = ku;
    }
    ku->parent = pv;
    kv->parent = pu;
    if (oldu != newu) {
        while (true) {
            kv = kv->parent;
            kv->sz += newu - oldu;
            if (kv == u) {
                break;
            }
        }
    }
    if (oldv != newv) {
        while (true) {
            ku = ku->parent;
            ku->sz += newv - oldv;
            if (ku == v) {
                break;
            }
        }
    }
}