/*
 * smax combination rule
 * */
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

triple Not(BooleanCircuit *bc, triple p1)
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

triple wea(BooleanCircuit *bc, triple p1)
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

triple po(BooleanCircuit *bc, triple p1, triple p2)
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

triple Do(BooleanCircuit *bc, triple p1, triple p2)
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

triple fa(BooleanCircuit *bc, triple p1, triple p2)
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
