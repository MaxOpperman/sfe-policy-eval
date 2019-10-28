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

    Query query_creation(BooleanCircuit *bc, uint32_t bitlen);
    CipherSet query_attributes(Query& query);
    CipherSet query_values(Query& query);

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

    Triple evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, Query q);
    Triple target_evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, Query q);

};

class DummyTarget : public Node
{
public:
	Triple result;

	DummyTarget(Triple r, Node *c1)
		: Node(c1, NULL), result(r)
	{
	}

	Triple evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, Query q);

};

class Leaf: public Node
{
public:
    share* value;

    Leaf(share* val)
        : Node(NULL, NULL), value(val)
    {
    }

    Triple evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, Query q);

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

};

#endif /* __POLICY_H_ */
