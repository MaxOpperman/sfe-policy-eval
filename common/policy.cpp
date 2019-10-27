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

    result.f = bc->PutORGate(s_op_temp1, s_op_temp2);

    // pi_u(p1 wmin p2) = pi_NA(p1) OR p_NA(p2)
    result.u = bc->PutORGate(p1.u, p2.u);

    // pi_1(p1 wmin p2) = pi_1(p1) AND pi_1(p2)   
    result.t = bc->PutANDGate(p1.t, p2.t);

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
    result.f = bc->PutORGate(p1.f, p1.u);

    uint32_t zero = 0;
    share *zero_share = bc->PutCONSGate(zero, (uint32_t) 1);

    // pi_u(p) = pi_NA(p)
    result.u = zero_share;

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
    result.f = bc->PutADDGate(t1.f, t2.f);
    result.u = bc->PutADDGate(t1.u, t2.u);     

    return result;
}


Triple Node::Triple_subtraction(BooleanCircuit *bc, Triple t1, Triple t2)
{
    Triple result;
    result.t = bc->PutSUBGate(t1.t, t2.t);
    result.f = bc->PutSUBGate(t1.f, t2.f);
    result.u = bc->PutSUBGate(t1.u, t2.u);     

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
share *Node::PutINCGate(BooleanCircuit *bc, share *s_a, CipherSet s_bs) {

    share *intermediate, *equal_zero, *zero_share, *out;
    share *result = bc->PutSUBGate(s_bs[0], s_a);

    for(int i = 1 ; i < s_bs.size() ; i++) {
        intermediate = bc->PutSUBGate(s_bs[i], s_a);
        result = bc->PutMULGate(result, intermediate);
    }

    uint32_t zero = 0;
    zero_share = bc->PutCONSGate(zero, 1);
    out = bc->PutEQGate(result, zero_share);

    return out;
}

// Policy evaluation for the target node
Triple Target::evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, Query q) {
    Triple result, t2, t3, F, U;
    uint32_t zero = 0;
    uint32_t one = 1;

    Triple r = this->target_evaluate(bc, role, bitlen, q);

    share *one_share = bc->PutCONSGate(one, 1);
    share *pi_t = bc->PutEQGate(r.t, one_share);
    share *pi_u = bc->PutEQGate(r.u, one_share);
    share *pi_f = bc->PutEQGate(r.f, one_share);

    Triple pt = this->child1->evaluate(bc, role, bitlen, q);
    result = this->Triple_scaling(bc, pt, pi_t);

    U.t = bc->PutCONSGate(zero, 1);
    U.f = bc->PutCONSGate(zero, 1);
    U.u = bc->PutCONSGate(one, 1);

    t2 = this->Triple_scaling(bc, U, pi_f);
    result =  this->Triple_addition(bc, result, t2);

    t3 = this->Triple_scaling(bc, U, pi_u);
    result = this->Triple_addition(bc, result, t3);

    return result;
}

CipherSet Node::query_attributes(Query& query)
{
    CipherSet attributes;
    for(int i = 0; i < query.size(); ++i) {
        attributes.push_back(query[i].attribute);
    }
    
    return attributes;
}

CipherSet Node::query_values(Query& query)
{
    CipherSet values;
    for(int i = 0; i < query.size(); ++i) {
        values.push_back(query[i].value);
    }
    
    return values;
}

// Perform target evaluation
Triple Target::target_evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, Query q) {
    CipherSet Aq = this->query_attributes(q);
    CipherSet Vq = this->query_values(q);

    Triple result;

	uint32_t zero = 0;
	share *matching = bc->PutCONSGate(zero, bitlen);
	
	for(int i = 0; i < q.size(); i++) {
		share *attribute_match = bc->PutEQGate(this->attribute, q[i].attribute);

		// Equal condition
		share *p1 = bc->PutCONSGate((uint32_t) 1, bitlen);
		share *value_equal = bc->PutEQGate(this->value, q[i].value);
		share *value_match = bc->PutANDGate(p1, value_equal);
		
		// Not equal condition
		share *p2 = bc->PutCONSGate((uint32_t) 2, bitlen);
		share *value_not_equal = bc->PutINVGate(value_equal);
		share *non_match_result = bc->PutANDGate(p2, value_not_equal);
		value_match = bc->PutORGate(value_match, non_match_result);

		// Greater than condition
		share *p3 = bc->PutCONSGate((uint32_t) 3, bitlen);
		share *value_greater_than = bc->PutGTGate(this->value, q[i].value);
		share *greater_than_result = bc->PutANDGate(p3, value_greater_than);
		value_match = bc->PutORGate(value_match, greater_than_result);

		// Lesser than condition
		share *p4 = bc->PutCONSGate((uint32_t) 4, 3);
		share *value_lesser_than = bc->PutINVGate(value_greater_than);
		share *value_lesser_than_equal = bc->PutORGate(value_equal, value_lesser_than);
		share *lesser_than_result = bc->PutANDGate(p4, value_lesser_than_equal);
		value_match = bc->PutORGate(value_match, lesser_than_result);

		// Combine results
		share *tmp = bc->PutANDGate(attribute_match, value_match);
		matching = bc->PutORGate(matching, tmp);
	}

	share *non_matching = bc->PutINVGate(matching);
	share *included = PutINCGate(bc, this->attribute, Aq);


	result.t = bc->PutANDGate(included, matching);
	result.f = bc->PutANDGate(included, non_matching);
	result.u = bc->PutINVGate(included);

    return result;

}

// Policy evaluation for leaf nodes
Triple Leaf::evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, Query q)
{
    Triple left, right, result;
    uint32_t zero = 0;
    uint32_t one = 1;

    // [x=1] MUL T
    Triple T;
    T.t = bc->PutCONSGate(one, 1);
    T.f = bc->PutCONSGate(zero, 1);
    T.u = bc->PutCONSGate(zero, 1);
    left = Triple_scaling(bc, T, this->value);

    // [x=0] MUL F
    Triple F;
    F.t = bc->PutCONSGate(zero, 1);
    F.f = bc->PutCONSGate(one, 1);
    F.u = bc->PutCONSGate(zero, 1);
    share* zero_share = bc->PutCONSGate(zero, 1);
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
