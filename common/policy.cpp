#include "policy.h"
#include "../ABY/src/abycore/circuit/booleancircuits.h"
#include "../ABY/src/abycore/sharing/sharing.h"

/*
 * Combination rules
 */

Triple Node::smax(BooleanCircuit *bc, Triple p1, Triple p2)
{
    Triple result;
    share *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    // pi_0(p1 smax p2) = pi_0(p1) AND pi_0(p2)
    result.f = bc->PutANDGate(p1.f, p2.f);

    // pi_u(p1 smax p2) = (pi_NA(p1) AND NOT pi_1(p2)) OR (pi_NA(p2) AND NOT pi_1(p1))
    s_inv_1 = bc->PutINVGate(p2.t);
    s_inv_2 = bc->PutINVGate(p1.t);

    s_op_temp1 = bc->PutANDGate(p1.u, s_inv_1);
    s_op_temp2 = bc->PutANDGate(p2.u, s_inv_2);

    result.u = bc->PutORGate(s_op_temp1, s_op_temp2);
    
    // pi_1(p1 smax p2) = pi_1(p1) OR pi_1(p2)
    result.t = bc->PutORGate(p1.t, p2.t);

    return result;
}

Triple Node::smin(BooleanCircuit *bc, Triple p1, Triple p2)
{
    Triple result;
    share *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    // pi_0(p1 smin p2) = pi_0(p1) OR pi_0(p2)
    result.f = bc->PutORGate(p1.f, p2.f);

    // pi_u(p1 smin p2) = (pi_NA(p1) AND NOT pi_0(p2)) OR (pi_NA(p2) AND NOT pi_0(p1))
    s_inv_1 = bc->PutINVGate(p2.f);
    s_inv_2 = bc->PutINVGate(p1.f);

    s_op_temp1 = bc->PutANDGate(p1.u, s_inv_1);
    s_op_temp2 = bc->PutANDGate(p2.u, s_inv_2);

    result.u = bc->PutORGate(s_op_temp1, s_op_temp2);
    
    // pi_1(p1 smin p2) = pi_1(p1) AND pi_1(p2)
    result.t = bc->PutANDGate(p1.t, p2.t);

    return result;
}

Triple Node::wmax(BooleanCircuit *bc, Triple p1, Triple p2)
{
    Triple result;
    share *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    // pi_0(p1 wmax p2) = pi_0(p1) AND pi_0(p2)
    result.f = bc->PutANDGate(p1.f, p2.f);

    // pi_u(p1 wmax p2) = pi_NA(p1) OR p_NA(p2)
    result.u = bc->PutORGate(p1.u, p2.u);

    // pi_1(p1 wmax p2) = (pi_1(p1) AND NOT pi_u(p2)) OR (pi_1(p2) AND pi_NA(p1)) 
    s_inv_1 = bc->PutINVGate(p2.u);
    s_inv_2 = bc->PutINVGate(p1.u);

    s_op_temp1 = bc->PutANDGate(p1.t, s_inv_1);
    s_op_temp2 = bc->PutANDGate(p2.t, s_inv_2);

    result.t = bc->PutORGate(s_op_temp1, s_op_temp2);

    return result;
}

Triple Node::wmin(BooleanCircuit *bc, Triple p1, Triple p2)
{
    Triple result;

    share *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    // pi_1(p1 wmin p2) = (pi_0(p1) AND NOT pi_u(p2)) OR (pi_0(p2) AND pi_NA(p1)) 
    s_inv_1 = bc->PutINVGate(p2.u);
    s_inv_2 = bc->PutINVGate(p1.u);

    s_op_temp1 = bc->PutANDGate(p1.f, s_inv_1);
    s_op_temp2 = bc->PutANDGate(p2.f, s_inv_2);

    result.t = bc->PutORGate(s_op_temp1, s_op_temp2);

    // pi_u(p1 wmin p2) = pi_NA(p1) OR p_NA(p2)
    result.u = bc->PutORGate(p1.u, p2.u);

    // pi_1(p1 wmin p2) = pi_1(p1) AND pi_1(p2)   
    result.f = bc->PutANDGate(p1.t, p2.t);

    return result;
}

// Complement rule
Triple Node::Not(BooleanCircuit *bc, Triple p1)
{
    Triple result;

    // pi_0(p) = pi_1(p)
    result.f = p1.t;

    // pi_u(p) = pi_NA(p)
    result.u = p1.u;

    //pi_1(p) = pi_0(p)
    result.t = p1.f;

    return result;
}

// Optional rule
Triple Node::wea(BooleanCircuit *bc, Triple p1)
{
    Triple result;

    // pi_0(p) = pi_1(p)
    result.f = bc->PutORGate(p1.t, p1.u);

    // pi_u(p) = pi_NA(p)
    result.u = p1.u;

    //pi_1(p) = pi_0(p)
    result.t = p1.t;

    return result;
}

// permit override rule
Triple Node::po(BooleanCircuit *bc, Triple p1, Triple p2)
{
    Triple result;

    share *s_op, *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    //pi_0(p1 PO p2) = (pi_0(p1) AND pi_1(p2)) OR (pi_0(p2) AND pi_1(p1))
    s_inv_1 = bc->PutINVGate(p2.t);
    s_inv_2 = bc->PutINVGate(p1.t);

    s_op_temp1 = bc->PutANDGate(p1.f, s_inv_1);
    s_op_temp2 = bc->PutANDGate(p2.f, s_inv_2);

    result.f = bc->PutORGate(s_op_temp1, s_op_temp2);

    //pi_u(p1 PO p2) = pi_NA(p1) AND pi_NA(p2)
    result.u = bc->PutANDGate(p1.u, p2.u);

    //pi_1(p1 PO p2) = (pi_1(p1) OR pi_1(p2))
    result.t = bc->PutORGate(p1.t, p2.t);

    return result;
}

// deny override rule
Triple Node::Do(BooleanCircuit *bc, Triple p1, Triple p2)
{
    Triple result;
    share *s_op, *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    //pi_0 (p1 DO p2) = pi_0(p1) OR pi_0(p2)
    result.f = bc->PutORGate(p1.f, p2.f);

    //pi_u(p1 DO p2) = pi_NA(p1) AND pi_NA(p2)
    result.u = bc->PutANDGate(p1.u, p2.u);

    //pi_1(p1 DO p2) = (pi_1(p1) AND pi_0(p2)) OR (pi_1(p2) AND pi_0(p1))
    s_inv_1 = bc->PutINVGate(p2.f);
    s_inv_2 = bc->PutINVGate(p1.f);

    s_op_temp1 = bc->PutANDGate(p1.t, s_inv_1);
    s_op_temp2 = bc->PutANDGate(p2.t, s_inv_2);

    result.t = bc->PutORGate(s_op_temp1, s_op_temp2);

    return result;
}

// First attribute rule
Triple Node::fa(BooleanCircuit *bc, Triple p1, Triple p2)
{
    Triple result;
    share *s_op_temp1, *s_op_temp2;

    //pi_0 (p1 FA p2) = pi_0(p1) OR (pi_u(p1) AND pi_0(p2))
    s_op_temp1 = bc->PutANDGate(p1.u, p2.f);
    result.f = bc->PutORGate(p1.f, s_op_temp1);

    //pi_u(p1 FA p2) = pi_NA(p1) AND pi_NA(p2)
    result.u = bc->PutANDGate(p1.u, p2.u);

    //pi_1 (p1 FA p2) = pi_1(p1) OR (pi_u(p1) AND pi_1(p2))
    s_op_temp2 = bc->PutANDGate(p1.u, p2.t);
    result.t = bc->PutORGate(p1.t, s_op_temp2);

    return result;
}


/*
 *
 * Triple arithmetic
 *
 */

Triple Node::Triple_addition(BooleanCircuit *bc, Triple t1, Triple t2)
    {
        Triple result;
        result.t = bc->PutADDGate(t1.t, t2.t);
        result.f =   bc->PutADDGate(t1.f, t2.f);
        result.u =     bc->PutADDGate(t1.u, t2.u);     

        return result;
    }


Triple Node::Triple_subtraction(BooleanCircuit *bc, Triple t1, Triple t2)
{
    Triple result;
    result.t = bc->PutSUBGate(t1.t, t2.t);
    result.f =   bc->PutSUBGate(t1.f, t2.f);
    result.u =     bc->PutSUBGate(t1.u, t2.u);     

    return result;
}
    
Triple Node::Triple_scaling(BooleanCircuit *bc, Triple t, share *s)
{
    Triple result;
    result.t = bc->PutMULGate(t.t, s);
    result.u = bc->PutMULGate(t.u, s);
    result.f = bc->PutMULGate(t.f, s);

    return result;
}

/*
 *
 *  Inclusion gate
 *
 */ 
share *Node::PutINCGate(BooleanCircuit *bc, share *s_a, share *s_bs[], int size) {

    share *intermediate, *equal_zero, *zero, *one, *out;
    share *result = bc->PutSUBGate(s_bs[0], s_a);

    for(int i=1;i<size; i++) {
        intermediate = bc->PutSUBGate(s_bs[i], s_a);
        result = bc->PutMULGate(result, intermediate);
    }

    uint32_t zero_int = 0;
    zero = bc->PutINGate(zero_int, 1, CLIENT);
    out = bc->PutEQGate(result, zero);

    return out;
}

// Policy evaluation for the target node
Triple Target::evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, Query q) {
    Triple result, t2, t3, F, U;

    Triple r = this->target_evaluate(bc, role, bitlen, q);
    share *one_share = bc->PutINGate((uint32_t) 1, 1, CLIENT);
    share *pi_t = bc->PutEQGate(r.t, one_share);
    share *pi_u = bc->PutEQGate(r.u, one_share);
    share *pi_f = bc->PutEQGate(r.f, one_share);

    Triple pt = this->child1->evaluate(bc, role, bitlen, q);
    result = this->Triple_scaling(bc, pt, pi_t);
    
    F.t = bc->PutINGate((uint32_t) 0, 1, CLIENT);
    F.f =  bc->PutINGate((uint32_t) 1, 1, CLIENT);
    F.u = bc->PutINGate((uint32_t) 0, 1, CLIENT);

    t2 = this->Triple_scaling(bc, F, pi_f);
    result =  this->Triple_addition(bc, result, t2);

    U.t = bc->PutINGate((uint32_t) 0, 1, CLIENT);
    U.f =  bc->PutINGate((uint32_t) 0, 1, CLIENT);
    U.u = bc->PutINGate((uint32_t) 1, 1, CLIENT);

    t3 = this->Triple_scaling(bc, U, pi_u);
    result = this->Triple_addition(bc, result, t3);

    return result;
}

// Setup target evaluation
Triple Target::target_evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, Query q) {
    share* attr_share, *value_share;
    share* Aq[1] = {};
    share* Vq[1] = {};

    attr_share = bc->PutINGate(q[0].attribute, bitlen, CLIENT);
    value_share = bc->PutINGate(q[0].value, bitlen, CLIENT);
    Aq[0] = attr_share;
    Vq[0] = value_share;

    Triple result;
    uint32_t zero = 0;
    uint32_t one = 1;

    // Term 1   ([0], [0], [1]) if 
    Triple U; 
    U.t = bc->PutINGate(zero, 1, CLIENT);
    U.f = bc->PutINGate(zero, 1, CLIENT);
    U.u = bc->PutINGate(one, 1, CLIENT);

    share *one_share = bc->PutINGate(one, 1, CLIENT);
    share *inc_aq = PutINCGate(bc, this->attribute, Aq, sizeof(*Aq) / sizeof(Aq[0]));
    share *int_result = bc->PutSUBGate(one_share, inc_aq);

    result = Triple_scaling(bc, U, int_result);

    // Term 2   ([1], [0], [0]) if [a_1 = a] and [v_1 condition v]
    Triple T, t2_result;
    T.t = bc->PutINGate(one, 1, CLIENT);
    T.f = bc->PutINGate(zero, 1, CLIENT);
    T.u = bc->PutINGate(zero, 1, CLIENT);

    t2_result = Triple_scaling(bc, T, inc_aq);

    // c = 1
    share *c1 = bc->PutEQGate(one_share, this->condition);

    // sum
    share *c1_a_eq = bc->PutEQGate(Aq[0], this->attribute);
    share *c1_v_eq = bc->PutEQGate(Vq[0], this->value);
    share *c1_sum = bc->PutMULGate(c1_a_eq, c1_v_eq);
    for(int i=1;i<(sizeof(*Aq) / sizeof(Aq[0]));i++) {
        c1_a_eq = bc->PutEQGate(Aq[i], this->attribute);
        c1_v_eq = bc->PutEQGate(Vq[i], this->value);
        share *c1_sum_int = bc->PutMULGate(c1_a_eq, c1_v_eq);
        c1_sum = bc->PutADDGate(c1_sum, c1_sum_int);
    }
    share *t2_sum = bc->PutMULGate(c1, c1_sum);

    // c = 2
    share *two_share = bc->PutINGate((uint32_t) 2, 3, CLIENT);
    share *c2 = bc->PutEQGate(two_share, this->condition);

    // sum
    share *c2_a_eq = bc->PutEQGate(Aq[0], this->attribute);
    share *c2_v_eq = bc->PutEQGate(Vq[0], this->value);
    share *c2_v_neq = bc->PutSUBGate(one_share, c2_v_eq);
    share *c2_sum = bc->PutMULGate(c2_a_eq, c2_v_neq);
    for(int i=1;i<(sizeof(*Aq) / sizeof(Aq[0]));i++) {
        c2_a_eq = bc->PutEQGate(Aq[i], this->attribute);
        c2_v_eq = bc->PutEQGate(Vq[i], this->value);
        c2_v_neq = bc->PutSUBGate(one_share, c2_v_eq);
        share *c2_sum_int = bc->PutMULGate(c2_a_eq, c2_v_eq);
        c2_sum = bc->PutADDGate(c2_sum, c2_sum_int);
    }
    c2 = bc->PutMULGate(c2, c2_sum);
    t2_sum = bc->PutADDGate(t2_sum, c2);

    // c = 3
    share *three_share = bc->PutINGate((uint32_t) 3, 3, CLIENT);
    share *c3 = bc->PutEQGate(three_share, this->condition);

    // sum
    share *c3_a_eq = bc->PutEQGate(Aq[0], this->attribute);
    share *c3_v_eq = bc->PutGTGate(this->value, Vq[0]);
    share *c3_sum = bc->PutMULGate(c3_a_eq, c3_v_eq);
    for(int i=1;i<(sizeof(*Aq) / sizeof(Aq[0]));i++) {
        c3_a_eq = bc->PutEQGate(Aq[i], this->attribute);
        c3_v_eq = bc->PutGTGate(this->value, Vq[0]);
        share *c3_sum_int = bc->PutMULGate(c3_a_eq, c3_v_eq);
        c3_sum = bc->PutADDGate(c3_sum, c3_sum_int);
    }
    c3 = bc->PutMULGate(c3, c3_sum);
    t2_sum = bc->PutADDGate(t2_sum, c3);
    
    // c = 4
    share *four_share = bc->PutINGate((uint32_t) 4, 3, CLIENT);
    share *c4 = bc->PutEQGate(four_share, this->condition);
    
    share *c4_a_eq = bc->PutEQGate(Aq[0], this->attribute);
    share *c4_v_eq = bc->PutGTGate(Vq[0], this->value);
    share *c4_sum = bc->PutMULGate(c4_a_eq, c4_v_eq);
    for(int i=1;i<(sizeof(*Aq) / sizeof(Aq[0]));i++) {
        c4_a_eq = bc->PutEQGate(Aq[i], this->attribute);
        c4_v_eq = bc->PutGTGate(Vq[0], this->value);
        share *c4_sum_int = bc->PutMULGate(c4_a_eq, c4_v_eq);
        c4_sum = bc->PutADDGate(c4_sum, c4_sum_int);
    }
    c4 = bc->PutMULGate(c4, c4_sum);
    t2_sum = bc->PutADDGate(t2_sum, c4);

    t2_result = Triple_scaling(bc, t2_result, t2_sum);
    result = Triple_addition(bc, result, t2_result);

    // Term 3   ([0], [1], [0]) if [a_1 != a] and [v_1 != v]
    Triple F, t3_result;
    F.t = bc->PutINGate(zero, 1, CLIENT);
    F.f = bc->PutINGate(one, 1, CLIENT);
    F.u = bc->PutINGate(zero, 1, CLIENT);

    share *not_eq_cmp = bc->PutSUBGate(one_share, t2_sum);
    share *inc_comb = bc->PutMULGate(inc_aq, not_eq_cmp);
    t3_result = Triple_scaling(bc, F, inc_comb);

    result = Triple_addition(bc, result, t3_result);

    return result;

}

// Policy evaluation for leaf nodes
Triple Leaf::evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, Query q)
{
    Triple left, right, result;

    // [x=1] MUL T
    Triple T;
    T.t = bc->PutINGate((uint32_t) 1, 1, CLIENT);
    T.f = bc->PutINGate((uint32_t) 0, 1, CLIENT);
    T.u = bc->PutINGate((uint32_t) 0, 1, CLIENT);
    left = Triple_scaling(bc, T, this->value);

    // [x=0] MUL F
    Triple F;
    F.t = bc->PutINGate((uint32_t) 0, 1, CLIENT);
    F.f = bc->PutINGate((uint32_t) 1, 1, CLIENT);
    F.u = bc->PutINGate((uint32_t) 0, 1, CLIENT);
    share* zero_share = bc->PutINGate((uint32_t) 0, 1, CLIENT);
    share* eq_zero = bc->PutEQGate(this->value, zero_share);
    right = Triple_scaling(bc, F, eq_zero);

    result = Triple_addition(bc, left, right);

    return result;
}

// Policy evaluation for operation nodes
Triple Operation::evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, Query q)
{
    Triple left, right, result;

    if(this->child2 == NULL) {
        Triple r = this->child1->evaluate(bc, role, bitlen, q);
        if(this->rule == NOT) {
            result = this->Not(bc, r);
        }
        else if(this->rule == WEA) {
            result = this->wea(bc, r);
        }
        else {
            std::cout << "Trying to perform operation on one child that requires two children." << std::endl;   
        }
    }
    else {
        Triple r1 = this->child1->evaluate(bc, role, bitlen, q);
        Triple r2 = this->child2->evaluate(bc, role, bitlen, q);

        switch(this->rule) {
            case SMAX:
                result = this->smax(bc, r1, r2);
                break;
            case SMIN:
                result = this->smin(bc, r1, r2);
                break;
            case WMAX:
                result = this->wmax(bc, r1, r2);
                break;
            case WMIN:
                result = this->wmin(bc, r1, r2);
                break;
            case PO:
                result = this->po(bc, r1, r2);
                break;
            case DO:
                result = this->Do(bc, r1, r2);
                break;
            default:
                result = this->fa(bc, r1, r2);
        }
    }

    return result;
}

int32_t perform_target_evaluation(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint32_t nvals, uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg,
		e_sharing sharing, int pi) {

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg);

	std::vector<Sharing*>& sharings = party->GetSharings();
	Circuit* circ = sharings[sharing]->GetCircuitBuildRoutine();

    // Query
    Query query;
    query.push_back({1, 12});

    // Encrypted 0 and 1
    share* one = circ->PutINGate((uint32_t) 1, 1, CLIENT);
    share* zero = circ->PutINGate((uint32_t) 0, 1, CLIENT);

    share* twelve = circ->PutINGate((uint32_t) 12, 4, CLIENT);
    share* two = circ->PutINGate((uint32_t) 2, 4, CLIENT);
    share* three = circ->PutINGate((uint32_t) 3, 4, CLIENT);
    share* four = circ->PutINGate((uint32_t) 4, 4, CLIENT);
    share* five = circ->PutINGate((uint32_t) 5, 4, CLIENT);
    share* six = circ->PutINGate((uint32_t) 6, 4, CLIENT);
    
    // Policies
    // This is figure 2 from the paper.
    Node *policy = new Operation(
        NOT, 
        new Target(
            one, twelve, one, 
            new Operation(
                SMIN, 
                new Target(
                    three, four, four,
                    new Operation(
                        WMAX,
                        new Target(
                            one, six, two,
                            new Leaf(one, 1)
                        ),
                        new Leaf(zero, 0)
                    )
                ),
                new Operation(
                    PO,
                    new Target(
                        one, five, three,
                        new Leaf(one, 1)
                    ),
                    new Target(
                        six, twelve, one,
                        new Leaf(one, 1)
                    )
                )
            )
        )
    );

    std::cout << policy->print() << std::endl;
    Triple t = policy->evaluate((BooleanCircuit *)circ, role, bitlen, query);
    
    share *s_t = circ->PutOUTGate(t.t, ALL);
    share *s_u = circ->PutOUTGate(t.u, ALL);
    share *s_f = circ->PutOUTGate(t.f, ALL);

    std::cout << "Circuit succesfully initialised." << std::endl;

    party->ExecCircuit();

    std::cout << "Circuit executed, collecting results..." << std::endl;

    uint32_t t_output = s_t->get_clear_value<uint32_t>();
    uint32_t u_output = s_u->get_clear_value<uint32_t>();
    uint32_t f_output = s_f->get_clear_value<uint32_t>();

    std::cout << "Output: ([" << t_output << "], [" << f_output << "], [" << u_output << "])" << std::endl;

	delete party;
	return 0;
}
