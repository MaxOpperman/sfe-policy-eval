#ifndef __POLICY_H_
#define __POLICY_H_

#include "../ABY/src/abycore/circuit/booleancircuits.h"
#include "../ABY/src/abycore/circuit/arithmeticcircuits.h"
#include "../ABY/src/abycore/circuit/circuit.h"
#include "../ABY/src/abycore/sharing/sharing.h"
#include "../ABY/src/abycore/aby/abyparty.h"
#include <math.h>
#include <cassert>
#include "policy.h"

struct query { uint32_t attr; uint32_t value; };
struct triple { share* permit; share* deny; share* na; };
enum CombinationRule { NOT, WEA, SMAX, SMIN, WMAX, WMIN, PO, DO, FA };

class Node {
public:
    Node *child1;
    Node *child2;

    Node(Node *c1, Node *c2)
        : child1(c1), child2(c2)
    {
    }

    virtual triple evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, query q)
    {
        triple t;

        return t;
    }

    triple triple_addition(BooleanCircuit *bc, triple t1, triple t2);
    triple triple_subtraction(BooleanCircuit *bc, triple t1, triple t2);
    triple triple_scaling(BooleanCircuit *bc, triple t, share *s);
    triple triple_equality();

    triple smax(BooleanCircuit *bc, triple t1, triple t2);
    triple smin(BooleanCircuit *bc, triple t1, triple t2);
    triple wmax(BooleanCircuit *bc, triple t1, triple t2);
    triple wmin(BooleanCircuit *bc, triple t1, triple t2);
    triple Not(BooleanCircuit *bc, triple t1);
    triple wea(BooleanCircuit *bc, triple t1);
    triple po(BooleanCircuit *bc, triple t1, triple t2);
    triple Do(BooleanCircuit *bc, triple t1, triple t2);
    triple fa(BooleanCircuit *bc, triple t1, triple t2);

    share* PutINCGate(BooleanCircuit *bc, share *s_a, share *s_bs[], int size);

};

class Target : public Node
{
public: 
    uint32_t attr;
    uint32_t value;
    uint32_t condition;

    Target(uint32_t a, uint32_t v, uint32_t c, Node *c1, Node *c2)
        : Node(c1, c2), attr(a), value(v), condition(c)
    {
    }
    Target(uint32_t a, uint32_t v, uint32_t c, Node *c1)
        : Node(c1, NULL), attr(a), value(v), condition(c)
    {
    }

    triple evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, query q) ;

    triple target_evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, query q);
    triple target_evaluate(BooleanCircuit *bc, share* Aq[], share* Vq[], share *s_target_a, share *s_target_v, share *s_target_c);

};

class Leaf: public Node
{
public:
    share* value;

    Leaf(share* val)
        : Node(NULL, NULL), value(val)
    {
    }

    triple evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, query q);

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

    triple evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, query q);

};

int32_t perform_target_evaluation(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint32_t nvals, uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg,
		e_sharing sharing);

#endif /* __POLICY_H_ */
