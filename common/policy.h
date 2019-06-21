#ifndef __POLICY_H_
#define __POLICY_H_

#include "../ABY/src/abycore/circuit/booleancircuits.h"
#include "../ABY/src/abycore/circuit/arithmeticcircuits.h"
#include "../ABY/src/abycore/circuit/circuit.h"
#include "../ABY/src/abycore/sharing/sharing.h"
#include "../ABY/src/abycore/aby/abyparty.h"
#include <math.h>
#include <cassert>

typedef std::vector<share *> CipherSet;

struct Pair
{
    share *attribute;
    share *value;
};

typedef std::vector<Pair> Query;

struct Triple 
{ 
    share* t; 
    share* f; 
    share* u; 
};

enum CombinationRule { NOT, WEA, SMAX, SMIN, WMAX, WMIN, PO, DO, FA };

class Node {
public:
    Node *child1;
    Node *child2;

    Node(Node *c1, Node *c2)
        : child1(c1), child2(c2)
    {
    }

    virtual Triple evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, Query q)
    {
        Triple t;

        return t;
    }

    virtual std::string print()
    {
        std::string s = "test";

        return s;
    }


    virtual int type()
    {
        return 0;
    }                                

    Query query_creation(BooleanCircuit *bc, uint32_t bitlen);
    CipherSet query_attributes(Query& query);
    CipherSet query_values(Query& query);

    Triple Triple_addition(BooleanCircuit *bc, Triple t1, Triple t2);
    Triple Triple_subtraction(BooleanCircuit *bc, Triple t1, Triple t2);
    Triple Triple_scaling(BooleanCircuit *bc, Triple t, share *s);
    Triple Triple_equality();

    Triple smax(BooleanCircuit *bc, Triple t1, Triple t2);
    Triple smin(BooleanCircuit *bc, Triple t1, Triple t2);
    Triple wmax(BooleanCircuit *bc, Triple t1, Triple t2);
    Triple wmin(BooleanCircuit *bc, Triple t1, Triple t2);
    Triple Not(BooleanCircuit *bc, Triple t1);
    Triple wea(BooleanCircuit *bc, Triple t1);
    Triple po(BooleanCircuit *bc, Triple t1, Triple t2);
    Triple Do(BooleanCircuit *bc, Triple t1, Triple t2);
    Triple fa(BooleanCircuit *bc, Triple t1, Triple t2);

    share* PutINCGate(BooleanCircuit *bc, share *s_a, CipherSet s_bs);

};

class Target : public Node
{
public: 
    share *attribute;
    share *value;
    share *condition;

    Target(share *a, share *v, share *c, Node *c1)
        : Node(c1, NULL), attribute(a), value(v), condition(c)
    {
    }

    Triple evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, Query q) ;
    Triple target_evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, Query q);

    std::string print()
    {
        std::string output = "";
        if(this->child1->type() == 1) {
            output = output + "(t, " + this->child1->print() + ")";
        }
        else {
            output = output + "t, " + this->child1->print();
        }
        if(this->child2 != NULL) {
            output = output + this->child2->print();
        }
        return output;
    }

    int type()
    {
        return 2;
    }

};

class Leaf: public Node
{
public:
    share* value;
    int clear_value;

    Leaf(share* val, int cval)
        : Node(NULL, NULL), value(val), clear_value(cval)
    {
    }

    Triple evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, Query q);

    std::string print()
    {
        std::string output = "[" + std::to_string(clear_value) + "]";
        return output;
    }

    int type()
    {
        return 1;
    }

};

class Operation: public Node
{
public:
    CombinationRule rule;
                               
    Operation(CombinationRule r, Node *c1, Node *c2)
        : Node(c1, c2), rule(r)
    {
    }
    Operation(CombinationRule r, Node *c1)
        : Node(c1, NULL), rule(r)
    {
    }

    Triple evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, Query q);

    std::string print()
    {
        std::string output = "";
        switch(this->rule) {
            case NOT:
                output = output + "not(";
                break;
            case WEA:
                output = output + "wea(";
                break;
            case SMAX:
                output = output + "smax(";
                break;
            case SMIN:
                output = output + "smin(";
                break;
            case WMAX:
                output = output + "wmax(";
                break;
            case WMIN:
                output = output + "wmin(";
                break;
            case PO:
                output = output + "po(";
                break;
            case DO:
                output = output + "do(";
                break;
            default:
                output = output + "fa(";
        }
        output = output + this->child1->print();
        if(this->child2 != NULL) {
            output = output + "," + this->child2->print();
        }
        output = output + ")";
        return output;
    }

    int type()
    {
        return 3;
    }

};

int32_t perform_target_evaluation(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint32_t nvals, uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg,
		e_sharing sharing, int pi);

#endif /* __POLICY_H_ */
