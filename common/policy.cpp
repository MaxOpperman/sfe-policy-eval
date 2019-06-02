#include "policy.h"
#include "../ABY/src/abycore/circuit/booleancircuits.h"
#include "../ABY/src/abycore/sharing/sharing.h"

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
    target t1 = {1, 12, 1};
    
    target t2_1 = {3, 35, 3};
    target t2_2 = {2, 25, 1};
    composite_target t2 = {t2_1, t2_2, 2};
    
    target t3 = {1, 22, 1};

    target t4_1 = {1, 12, 1};
    target t4_2 = {1, 13, 2};
    composite_target t4 = {t4_1, t4_2, 7};

    triple t = evaluate((BooleanCircuit*) circ, role, bitlen, queries[0], t3);

    //share* permit = result.permit;
    //share* na = result.na;
    //share* deny = result.deny;

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

triple evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, query q, composite_target t) {

    triple result, r1, r2;
    r1 = evaluate(bc, role, bitlen, q, t.t1);
    if(&t.t2 != NULL) {
        triple r2 = evaluate(bc, role, bitlen, q, t.t2);
    }

    std::cout << "Combining results using combination rule" << std::endl;

    switch(t.combiner) {
        case 1:
            result = smax(bc, r1, r2);
            break;
        case 2:
            result = smin(bc, r1, r2);
            break;
        case 3:
            result = wmax(bc, r1, r2);
            break;
        case 4:
            result = wmin(bc, r1, r2);
            break;
        case 5:
            result = Complement(bc, r1);
            break;
        case 6:
            result = Optional(bc, r1);
            break;
        case 7:
            result = PermitOverride(bc, r1, r2);
            break;
        case 8:
            result = DenyOverride(bc, r1, r2);
            break;
        default:
            result = FirstAttribute(bc, r1, r2);

    }
    
    return result;
}
/*
 * Setting up atomic evaluation formula
 */
triple evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, query q, target t) {
    share* attr_share, *value_share, *target_attr_share, *target_value_share, *target_condition_share;
    share* Aq[1] = {};
    share* Vq[1] = {};

    std::cout << "Given input query {" << q.attr << ", " << q.value << "}" << std::endl;
    attr_share = bc->PutINGate(q.attr, bitlen, SERVER);
    value_share = bc->PutINGate(q.value, bitlen, CLIENT);
    std::cout << "Target {" << t.attr << ", " << t.value << ", " << t.condition <<"}" << std::endl;
    target_attr_share = bc->PutINGate(t.attr, bitlen, SERVER);
    target_value_share = bc->PutINGate(t.value, bitlen, SERVER);
    target_condition_share = bc->PutINGate(t.condition, bitlen, SERVER);
    Aq[0] = attr_share;
    Vq[0] = value_share;

    triple r = evaluate(bc, Aq, Vq, target_attr_share, target_value_share, target_condition_share); 
    
    return r;
}

/* This is atomic evaluation
 *
 */ 
triple evaluate(BooleanCircuit *bc, share* Aq[], share* Vq[], share *s_target_a, share *s_target_v, share *s_target_c)
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

triple triple_addition(BooleanCircuit *bc, triple t1, triple t2)
{
    triple result;
    result.permit = bc->PutADDGate(t1.permit, t2.permit);
    result.deny =   bc->PutADDGate(t1.deny, t2.deny);
    result.na =     bc->PutADDGate(t1.na, t2.na);     

    return result;
}

triple triple_subtraction(BooleanCircuit *bc, triple t1, triple t2)
{
    triple result;
    result.permit = bc->PutSUBGate(t1.permit, t2.permit);
    result.deny =   bc->PutSUBGate(t1.deny, t2.deny);
    result.na =     bc->PutSUBGate(t1.na, t2.na);     

    return result;
}

triple triple_scaling(BooleanCircuit *bc, triple t, share *s)
{
    triple result;
    result.permit = bc->PutMULGate(t.permit, s);
    result.na = bc->PutMULGate(t.na, s);
    result.deny = bc->PutMULGate(t.deny, s);

    return result;
}

void triple_equality()
{

}

share* PutINCGate(BooleanCircuit *bc, share *s_a, share *s_bs[], int size) {

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

triple smax(BooleanCircuit *bc, triple p1, triple p2)
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

triple smin(BooleanCircuit *bc, triple p1, triple p2)
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

triple wmax(BooleanCircuit *bc, triple p1, triple p2)
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

triple wmin(BooleanCircuit *bc, triple p1, triple p2)
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

triple Complement(BooleanCircuit *bc, triple p1)
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

triple Optional(BooleanCircuit *bc, triple p1)
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

triple PermitOverride(BooleanCircuit *bc, triple p1, triple p2)
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

triple DenyOverride(BooleanCircuit *bc, triple p1, triple p2)
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

triple FirstAttribute(BooleanCircuit *bc, triple p1, triple p2)
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
