#include "policy.h"
#include "../ABY/src/abycore/circuit/booleancircuits.h"
#include "../ABY/src/abycore/sharing/sharing.h"

/*
 * Combination rules
 *
 */


triple Node::smax(BooleanCircuit *bc, triple p1, triple p2)
{
    triple result;
    share *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    // pi_0(p1 smax p2) = pi_0(p1) AND pi_0(p2)
    result.deny = bc->PutANDGate(p1.deny, p2.deny);

    // pi_NA(p1 smax p2) = (pi_NA(p1) AND NOT pi_1(p2)) OR (pi_NA(p2) AND NOT pi_1(p1))
    s_inv_1 = bc->PutINVGate(p2.permit);
    s_inv_2 = bc->PutINVGate(p1.permit);

    s_op_temp1 = bc->PutANDGate(p1.na, s_inv_1);
    s_op_temp2 = bc->PutANDGate(p2.na, s_inv_2);

    result.na = bc->PutORGate(s_op_temp1, s_op_temp2);
    
    // pi_1(p1 smax p2) = pi_1(p1) OR pi_1(p2)
    result.permit = bc->PutORGate(p1.permit, p2.permit);

    return result;
}

triple Node::smin(BooleanCircuit *bc, triple p1, triple p2)
{
    triple result;
    share *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    // pi_0(p1 smin p2) = pi_0(p1) OR pi_0(p2)
    result.deny = bc->PutORGate(p1.deny, p2.deny);

    // pi_NA(p1 smin p2) = (pi_NA(p1) AND NOT pi_0(p2)) OR (pi_NA(p2) AND NOT pi_0(p1))
    s_inv_1 = bc->PutINVGate(p2.deny);
    s_inv_2 = bc->PutINVGate(p1.deny);

    s_op_temp1 = bc->PutANDGate(p1.na, s_inv_1);
    s_op_temp2 = bc->PutANDGate(p2.na, s_inv_2);

    result.na = bc->PutORGate(s_op_temp1, s_op_temp2);
    
    // pi_1(p1 smin p2) = pi_1(p1) AND pi_1(p2)
    result.permit = bc->PutANDGate(p1.permit, p2.permit);

    return result;
}

triple Node::wmax(BooleanCircuit *bc, triple p1, triple p2)
{
    triple result;
    share *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    // pi_0(p1 wmax p2) = pi_0(p1) AND pi_0(p2)
    result.deny = bc->PutANDGate(p1.deny, p2.deny);

    // pi_NA(p1 wmax p2) = pi_NA(p1) OR p_NA(p2)
    result.na = bc->PutORGate(p1.na, p2.na);

    // pi_1(p1 wmax p2) = (pi_1(p1) AND NOT pi_NA(p2)) OR (pi_1(p2) AND pi_NA(p1)) 
    s_inv_1 = bc->PutINVGate(p2.na);
    s_inv_2 = bc->PutINVGate(p1.na);

    s_op_temp1 = bc->PutANDGate(p1.permit, s_inv_1);
    s_op_temp2 = bc->PutANDGate(p2.permit, s_inv_2);

    result.permit = bc->PutORGate(s_op_temp1, s_op_temp2);

    return result;
}

triple Node::wmin(BooleanCircuit *bc, triple p1, triple p2)
{
    triple result;

    share *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    // pi_1(p1 wmin p2) = (pi_0(p1) AND NOT pi_NA(p2)) OR (pi_0(p2) AND pi_NA(p1)) 
    s_inv_1 = bc->PutINVGate(p2.na);
    s_inv_2 = bc->PutINVGate(p1.na);

    s_op_temp1 = bc->PutANDGate(p1.deny, s_inv_1);
    s_op_temp2 = bc->PutANDGate(p2.deny, s_inv_2);

    result.permit = bc->PutORGate(s_op_temp1, s_op_temp2);

    // pi_NA(p1 wmin p2) = pi_NA(p1) OR p_NA(p2)
    result.na = bc->PutORGate(p1.na, p2.na);

    // pi_1(p1 wmin p2) = pi_1(p1) AND pi_1(p2)   
    result.deny = bc->PutANDGate(p1.permit, p2.permit);

    return result;
}

// Complement rule
triple Node::Not(BooleanCircuit *bc, triple p1)
{
    triple result;

    // pi_0(p) = pi_1(p)
    result.deny = p1.permit;

    // pi_NA(p) = pi_NA(p)
    result.na = p1.na;

    //pi_1(p) = pi_0(p)
    result.permit = p1.deny;

    return result;
}

// Optional rule
triple Node::wea(BooleanCircuit *bc, triple p1)
{
    triple result;

    // pi_0(p) = pi_1(p)
    result.deny = bc->PutORGate(p1.permit, p1.na);

    // pi_NA(p) = pi_NA(p)
    result.na = p1.na;

    //pi_1(p) = pi_0(p)
    result.permit = p1.permit;

    return result;
}

// Permit override rule
triple Node::po(BooleanCircuit *bc, triple p1, triple p2)
{
    triple result;

    share *s_op, *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    //pi_0(p1 PO p2) = (pi_0(p1) AND pi_1(p2)) OR (pi_0(p2) AND pi_1(p1))
    s_inv_1 = bc->PutINVGate(p2.permit);
    s_inv_2 = bc->PutINVGate(p1.permit);

    s_op_temp1 = bc->PutANDGate(p1.deny, s_inv_1);
    s_op_temp2 = bc->PutANDGate(p2.deny, s_inv_2);

    result.deny = bc->PutORGate(s_op_temp1, s_op_temp2);

    //pi_NA(p1 PO p2) = pi_NA(p1) AND pi_NA(p2)
    result.na = bc->PutANDGate(p1.na, p2.na);

    //pi_1(p1 PO p2) = (pi_1(p1) OR pi_1(p2))
    result.permit = bc->PutORGate(p1.permit, p2.permit);

    return result;
}

// Deny override rule
triple Node::Do(BooleanCircuit *bc, triple p1, triple p2)
{
    triple result;
    share *s_op, *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    //pi_0 (p1 DO p2) = pi_0(p1) OR pi_0(p2)
    result.deny = bc->PutORGate(p1.deny, p2.deny);

    //pi_NA(p1 DO p2) = pi_NA(p1) AND pi_NA(p2)
    result.na = bc->PutANDGate(p1.na, p2.na);

    //pi_1(p1 DO p2) = (pi_1(p1) AND pi_0(p2)) OR (pi_1(p2) AND pi_0(p1))
    s_inv_1 = bc->PutINVGate(p2.deny);
    s_inv_2 = bc->PutINVGate(p1.deny);

    s_op_temp1 = bc->PutANDGate(p1.permit, s_inv_1);
    s_op_temp2 = bc->PutANDGate(p2.permit, s_inv_2);

    result.permit = bc->PutORGate(s_op_temp1, s_op_temp2);

    return result;
}

// First attribute rule
triple Node::fa(BooleanCircuit *bc, triple p1, triple p2)
{
    triple result;
    share *s_op_temp1, *s_op_temp2;

    //pi_0 (p1 FA p2) = pi_0(p1) OR (pi_NA(p1) AND pi_0(p2))
    s_op_temp1 = bc->PutANDGate(p1.na, p2.deny);
    result.deny = bc->PutORGate(p1.deny, s_op_temp1);

    //pi_NA(p1 FA p2) = pi_NA(p1) AND pi_NA(p2)
    result.na = bc->PutANDGate(p1.na, p2.na);

    //pi_1 (p1 FA p2) = pi_1(p1) OR (pi_NA(p1) AND pi_1(p2))
    s_op_temp2 = bc->PutANDGate(p1.na, p2.permit);
    result.permit = bc->PutORGate(p1.permit, s_op_temp2);

    return result;
}


/*
 *
 * Triple arithmetic
 *
 */

triple Node::triple_addition(BooleanCircuit *bc, triple t1, triple t2)
    {
        triple result;
        result.permit = bc->PutADDGate(t1.permit, t2.permit);
        result.deny =   bc->PutADDGate(t1.deny, t2.deny);
        result.na =     bc->PutADDGate(t1.na, t2.na);     

        return result;
    }


triple Node::triple_subtraction(BooleanCircuit *bc, triple t1, triple t2)
{
    triple result;
    result.permit = bc->PutSUBGate(t1.permit, t2.permit);
    result.deny =   bc->PutSUBGate(t1.deny, t2.deny);
    result.na =     bc->PutSUBGate(t1.na, t2.na);     

    return result;
}
    
triple Node::triple_scaling(BooleanCircuit *bc, triple t, share *s)
{
    triple result;
    result.permit = bc->PutMULGate(t.permit, s);
    result.na = bc->PutMULGate(t.na, s);
    result.deny = bc->PutMULGate(t.deny, s);

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
triple Target::evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, query q) {
    triple result, t2, t3, deny_triple, na_triple;

    triple r = this->target_evaluate(bc, role, bitlen, q);
    share *one_share = bc->PutINGate((uint32_t) 1, 1, CLIENT);
    share *pi_t = bc->PutEQGate(r.permit, one_share);
    share *pi_u = bc->PutEQGate(r.na, one_share);
    share *pi_f = bc->PutEQGate(r.deny, one_share);

    triple pt = this->child1->evaluate(bc, role, bitlen, q);
    result = this->triple_scaling(bc, pt, pi_t);
    
    deny_triple.permit = bc->PutINGate((uint32_t) 0, 1, CLIENT);
    deny_triple.deny =  bc->PutINGate((uint32_t) 1, 1, CLIENT);
    deny_triple.na = bc->PutINGate((uint32_t) 0, 1, CLIENT);

    t2 = this->triple_scaling(bc, deny_triple, pi_f);
    result =  this->triple_addition(bc, result, t2);

    na_triple.permit = bc->PutINGate((uint32_t) 0, 1, CLIENT);
    na_triple.deny =  bc->PutINGate((uint32_t) 0, 1, CLIENT);
    na_triple.na = bc->PutINGate((uint32_t) 1, 1, CLIENT);

    t3 = this->triple_scaling(bc, na_triple, pi_u);
    result = this->triple_addition(bc, result, t3);

    return result;
}

// Setup target evaluation
triple Target::target_evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, query q) {
    share* attr_share, *value_share, *target_attr_share, *target_value_share, *target_condition_share;
    share* Aq[1] = {};
    share* Vq[1] = {};

    std::cout << "Given input query {" << q.attr << ", " << q.value << "}" << std::endl;
    attr_share = bc->PutINGate(q.attr, bitlen, SERVER);
    value_share = bc->PutINGate(q.value, bitlen, CLIENT);
    std::cout << "Target {" << this->attr << ", " << this->value << ", " << this->condition <<"}" << std::endl;
    target_attr_share = bc->PutINGate(this->attr, bitlen, SERVER);
    target_value_share = bc->PutINGate(this->value, bitlen, SERVER);
    target_condition_share = bc->PutINGate(this->condition, bitlen, SERVER);
    Aq[0] = attr_share;
    Vq[0] = value_share;

    triple r = target_evaluate(bc, Aq, Vq, target_attr_share, target_value_share, target_condition_share); 
    
    return r;
}

// Atomic target evaluation
triple Target::target_evaluate(BooleanCircuit *bc, share* Aq[], share* Vq[], share *s_target_a, share *s_target_v, share *s_target_c)
{
    std::cout << "Starting the actual evaluation..." << std::endl;
    // Some initial values
    triple result;
    uint32_t zero = 0;
    uint32_t one = 1;

    // Term 1   ([0], [0], [1]) if 
    triple na_triple; 
    na_triple.permit =  bc->PutINGate(zero, 1, CLIENT);
    na_triple.deny =    bc->PutINGate(zero, 1, CLIENT);
    na_triple.na =      bc->PutINGate(one, 1, CLIENT);

    share *one_share = bc->PutINGate(one, 1, CLIENT);
    share *inc_aq = PutINCGate(bc, s_target_a, Aq, sizeof(*Aq) / sizeof(Aq[0]));
    share *int_result = bc->PutSUBGate(one_share, inc_aq);

    result = triple_scaling(bc, na_triple, int_result);

    // Term 2   ([1], [0], [0]) if [a_1 = a] and [v_1 condition v]
    triple permit_triple, t2_result;
    permit_triple.permit =  bc->PutINGate(one, 1, CLIENT);
    permit_triple.deny =    bc->PutINGate(zero, 1, CLIENT);
    permit_triple.na =      bc->PutINGate(zero, 1, CLIENT);

    t2_result = triple_scaling(bc, permit_triple, inc_aq);

    // c = 1
    share *c1 = bc->PutEQGate(one_share, s_target_c);

    // sum
    share *c1_a_eq = bc->PutEQGate(Aq[0], s_target_a);
    share *c1_v_eq = bc->PutEQGate(Vq[0], s_target_v);
    share *c1_sum = bc->PutMULGate(c1_a_eq, c1_v_eq);
    for(int i=1;i<(sizeof(*Aq) / sizeof(Aq[0]));i++) {
        c1_a_eq = bc->PutEQGate(Aq[i], s_target_a);
        c1_v_eq = bc->PutEQGate(Vq[i], s_target_v);
        share *c1_sum_int = bc->PutMULGate(c1_a_eq, c1_v_eq);
        c1_sum = bc->PutADDGate(c1_sum, c1_sum_int);
    }
    share *t2_sum = bc->PutMULGate(c1, c1_sum);

    // c = 2
    share *two_share = bc->PutINGate((uint32_t) 2, 3, CLIENT);
    share *c2 = bc->PutEQGate(two_share, s_target_c);

    // sum
    share *c2_a_eq = bc->PutEQGate(Aq[0], s_target_a);
    share *c2_v_eq = bc->PutEQGate(Vq[0], s_target_v);
    share *c2_v_neq = bc->PutSUBGate(one_share, c2_v_eq);
    share *c2_sum = bc->PutMULGate(c2_a_eq, c2_v_neq);
    for(int i=1;i<(sizeof(*Aq) / sizeof(Aq[0]));i++) {
        c2_a_eq = bc->PutEQGate(Aq[i], s_target_a);
        c2_v_eq = bc->PutEQGate(Vq[i], s_target_v);
        c2_v_neq = bc->PutSUBGate(one_share, c2_v_eq);
        share *c2_sum_int = bc->PutMULGate(c2_a_eq, c2_v_eq);
        c2_sum = bc->PutADDGate(c2_sum, c2_sum_int);
    }
    c2 = bc->PutMULGate(c2, c2_sum);
    t2_sum = bc->PutADDGate(t2_sum, c2);

    // c = 3
    share *three_share = bc->PutINGate((uint32_t) 3, 3, CLIENT);
    share *c3 = bc->PutEQGate(three_share, s_target_c);

    // sum
    share *c3_a_eq = bc->PutEQGate(Aq[0], s_target_a);
    share *c3_v_eq = bc->PutGTGate(s_target_v, Vq[0]);
    share *c3_sum = bc->PutMULGate(c3_a_eq, c3_v_eq);
    for(int i=1;i<(sizeof(*Aq) / sizeof(Aq[0]));i++) {
        c3_a_eq = bc->PutEQGate(Aq[i], s_target_a);
        c3_v_eq = bc->PutGTGate(s_target_v, Vq[0]);
        share *c3_sum_int = bc->PutMULGate(c3_a_eq, c3_v_eq);
        c3_sum = bc->PutADDGate(c3_sum, c3_sum_int);
    }
    c3 = bc->PutMULGate(c3, c3_sum);
    t2_sum = bc->PutADDGate(t2_sum, c3);
    
    // c = 4
    share *four_share = bc->PutINGate((uint32_t) 4, 3, CLIENT);
    share *c4 = bc->PutEQGate(four_share, s_target_c);
    
    share *c4_a_eq = bc->PutEQGate(Aq[0], s_target_a);
    share *c4_v_eq = bc->PutGTGate(Vq[0], s_target_v);
    share *c4_sum = bc->PutMULGate(c4_a_eq, c4_v_eq);
    for(int i=1;i<(sizeof(*Aq) / sizeof(Aq[0]));i++) {
        c4_a_eq = bc->PutEQGate(Aq[i], s_target_a);
        c4_v_eq = bc->PutGTGate(Vq[0], s_target_v);
        share *c4_sum_int = bc->PutMULGate(c4_a_eq, c4_v_eq);
        c4_sum = bc->PutADDGate(c4_sum, c4_sum_int);
    }
    c4 = bc->PutMULGate(c4, c4_sum);
    t2_sum = bc->PutADDGate(t2_sum, c4);

    t2_result = triple_scaling(bc, t2_result, t2_sum);
    result = triple_addition(bc, result, t2_result);

    // Term 3   ([0], [1], [0]) if [a_1 != a] and [v_1 != v]
    triple deny_triple, t3_result;
    deny_triple.permit =  bc->PutINGate(zero, 1, CLIENT);
    deny_triple.deny =    bc->PutINGate(one, 1, CLIENT);
    deny_triple.na =      bc->PutINGate(zero, 1, CLIENT);

    share *not_eq_cmp = bc->PutSUBGate(one_share, t2_sum);
    share *inc_comb = bc->PutMULGate(inc_aq, not_eq_cmp);
    t3_result = triple_scaling(bc, deny_triple, inc_comb);

    result = triple_addition(bc, result, t3_result);

    return result;

}

// Policy evaluation for leaf nodes
triple Leaf::evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, query q)
{
    triple left, right, result;

    // [x=1] MUL T
    triple T;
    T.permit = bc->PutINGate((uint32_t) 1, 1, CLIENT);
    T.deny = bc->PutINGate((uint32_t) 0, 1, CLIENT);
    T.na = bc->PutINGate((uint32_t) 0, 1, CLIENT);
    left = triple_scaling(bc, T, this->value);

    // [x=0] MUL F
    triple F;
    F.permit = bc->PutINGate((uint32_t) 0, 1, CLIENT);
    F.deny = bc->PutINGate((uint32_t) 1, 1, CLIENT);
    F.na = bc->PutINGate((uint32_t) 0, 1, CLIENT);
    share* zero_share = bc->PutINGate((uint32_t) 0, 1, CLIENT);
    share* eq_zero = bc->PutEQGate(this->value, zero_share);
    right = triple_scaling(bc, F, eq_zero);

    result = triple_addition(bc, left, right);

    return result;
}

// Policy evaluation for operation nodes
triple Operation::evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, query q)
{
    triple left, right, result;

    if(this->child2 == NULL) {
        triple r = this->child1->evaluate(bc, role, bitlen, q);
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
        triple r1 = this->child1->evaluate(bc, role, bitlen, q);
        triple r2 = this->child2->evaluate(bc, role, bitlen, q);

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
		e_sharing sharing) {

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg);

	std::vector<Sharing*>& sharings = party->GetSharings();
	Circuit* circ = sharings[sharing]->GetCircuitBuildRoutine();

    // Query
    query queries[1] = {{1, 12}};

    // Targets
    Target *t1 = new Target(1, 12, 1, NULL, NULL);
    Target *t3 = new Target(1, 22, 1, NULL, NULL);

    // Encrypted 0 and 1
    share* one = circ->PutINGate((uint32_t) 1, 1, CLIENT);
    share* zero = circ->PutINGate((uint32_t) 0, 1, CLIENT);
    
    // Policies
    // This is figure 2 from the paper.
    Node *policy = new Operation(
        NOT, 
        new Target(
            1, 12, 1, 
            new Operation(
                SMIN, 
                new Target(
                    3, 4, 4,
                    new Operation(
                        WMAX,
                        new Target(
                            1, 6, 2,
                            new Leaf(one)
                        ),
                        new Leaf(zero)
                    )
                ),
                new Operation(
                    PO,
                    new Target(
                        1, 5, 3,
                        new Leaf(one)
                    ),
                    new Target(
                        6, 12, 1,
                        new Leaf(one)
                    )
                )
            )
        )
    );

    // Test policy
    Node *policy2 = new Operation(
        PO,
        new Target(
            1, 12, 2,
            new Leaf(one)
        ),
        new Leaf(zero)
    );

    triple t = policy->evaluate((BooleanCircuit *)circ, role, bitlen, queries[0]);
    
    share *s_permit = circ->PutOUTGate(t.permit, ALL);
    share *s_na = circ->PutOUTGate(t.na, ALL);
    share *s_deny = circ->PutOUTGate(t.deny, ALL);

    std::cout << "Circuit succesfully initialised." << std::endl;

    party->ExecCircuit();

    std::cout << "Circuit executed, collecting results..." << std::endl;

    uint32_t permit_output = s_permit->get_clear_value<uint32_t>();
    uint32_t na_output = s_na->get_clear_value<uint32_t>();
    uint32_t deny_output = s_deny->get_clear_value<uint32_t>();

    std::cout << "Output: ([" << permit_output << "], [" << deny_output << "], [" << na_output << "])" << std::endl;

	delete party;
	return 0;
}
