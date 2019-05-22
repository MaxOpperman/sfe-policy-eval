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

	share *s_alice_money, *s_bob_money, *s_out;

	uint32_t alice_money, bob_money, output;
	srand(time(NULL));
	alice_money = rand() % 100000;
	bob_money = rand() % 100000;

    struct query { int attr; int value; };

    share *s_struct_share;
    query queries[1] = {{5, 3}};

    struct target { int attr; int value; int function; };

    target t1 = {1, 12, 1};


    uint32_t el = 2;
    share *s_el = circ->PutINGate(el, bitlen, CLIENT);
    uint32_t list[3] = {1, 2, 3};
    share *s_tmp;
    share *s_list[3];
    for(int i=0;i<sizeof(list)/sizeof(list[0]);i++) {
        s_tmp = circ->PutINGate(list[i], bitlen, CLIENT);
        s_list[i] = s_tmp;
    }
    std::cout << "List has " << sizeof(list)/sizeof(list[0]) << " elements";
    s_out = PutINCGate((BooleanCircuit*) circ, s_el, s_list, sizeof(list) / sizeof(list[0]));
    s_out = circ->PutOUTGate(s_out, ALL);
    party->ExecCircuit();
    output = s_out->get_clear_value<uint32_t>();

    std::cout << "Output: " << output;

	delete party;
	return 0;
}

share* PutINCGate(BooleanCircuit *bc, share *s_a, share *s_bs[], int size) {

    share *intermediate, *equal_zero, *zero, *one, *out;
    share *result = bc->PutSUBGate(s_bs[0], s_a);
    std::cout << "using putinccircuit" << sizeof(*s_bs) / sizeof(s_bs[0]) << std::endl;
    bc->PutPrintValueGate(s_a, "a value");

    for(int i=1;i<size; i++) {
        std::cout << "test" << std::endl;
        bc->PutPrintValueGate(s_bs[i], "initial vals");
        intermediate = bc->PutSUBGate(s_bs[i], s_a);
        bc->PutPrintValueGate(intermediate, "int vals");
        result = bc->PutMULGate(result, intermediate);
    }

    bc->PutPrintValueGate(result, "mul result");

    uint32_t zero_int = 0;
    zero = bc->PutINGate(zero_int, 1, CLIENT);
    out = bc->PutEQGate(result, zero);

    return out;
}


void atomic_evaluate(BooleanCircuit *bc, share* Aq[], share* Vq[], share *s_target_a, share *s_target_v, share *s_target_c)
{
    // Term shares
    share *s_t1_A, *s_t1_D, *s_t1_NA, *s_t2_A, *s_t2_D, *s_t2_NA, *s_t3_A, *s_t3_D, *s_t3_NA;
    uint32_t zero = 0;
    uint32_t one = 1;

    // Create NA: ([0], [0], [1])
    s_t1_A = bc->PutINGate(zero, 1, CLIENT);
    s_t1_D = bc->PutINGate(zero, 1, CLIENT);
    s_t1_NA = bc->PutINGate(one, 1, CLIENT);

    share *one_share = bc->PutINGate(one, 1, CLIENT);
    share *included = PutINCGate(bc, s_target_a, Aq, sizeof(*Aq) / sizeof(Aq[0]));
    share *int_result = bc->PutSUBGate(one_share, included);

    s_t1_NA = bc->PutANDGate(s_t1_NA, int_result);

    // Term 2   ([1], [0], [0]) if [a_1 = a] and [v_1 = v]
    

    // Term 3   ([0], [1], [0]) if [a_1 != a] and [v_1 != v]
    
}

void smax(BooleanCircuit *bc, share *s_p1_D, share *s_p1_NA, share *s_p1_A, share *s_p2_D, share *s_p2_NA, share *s_p2_A, share* &new_p_D, share* &new_p_NA, share* &new_p_A)
{
    share *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    // pi_0(p1 smax p2) = pi_0(p1) AND pi_0(p2)
    new_p_D = bc->PutANDGate(s_p1_D, s_p2_D);

    // pi_NA(p1 smax p2) = (pi_NA(p1) AND NOT pi_1(p2)) OR (pi_NA(p2) AND NOT pi_1(p1))
    s_inv_1 = bc->PutINVGate(s_p2_A);
    s_inv_2 = bc->PutINVGate(s_p1_A);

    s_op_temp1 = bc->PutANDGate(s_p1_NA, s_inv_1);
    s_op_temp2 = bc->PutANDGate(s_p2_NA, s_inv_2);

    new_p_NA = bc->PutORGate(s_op_temp1, s_op_temp2);
    
    // pi_1(p1 smax p2) = pi_1(p1) OR pi_1(p2)
    new_p_A = bc->PutORGate(s_p1_A, s_p2_A);
}

void smin(BooleanCircuit *bc, share *s_p1_D, share *s_p1_NA, share *s_p1_A, share *s_p2_D, share *s_p2_NA, share *s_p2_A, share* &new_p_D, share* &new_p_NA, share* &new_p_A)
{
    share *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    // pi_0(p1 smin p2) = pi_0(p1) OR pi_0(p2)
    new_p_D = bc->PutORGate(s_p1_D, s_p2_D);

    // pi_NA(p1 smin p2) = (pi_NA(p1) AND NOT pi_0(p2)) OR (pi_NA(p2) AND NOT pi_0(p1))
    s_inv_1 = bc->PutINVGate(s_p2_D);
    s_inv_2 = bc->PutINVGate(s_p1_D);

    s_op_temp1 = bc->PutANDGate(s_p1_NA, s_inv_1);
    s_op_temp2 = bc->PutANDGate(s_p2_NA, s_inv_2);

    new_p_NA = bc->PutORGate(s_op_temp1, s_op_temp2);
    
    // pi_1(p1 smin p2) = pi_1(p1) AND pi_1(p2)
    new_p_A = bc->PutANDGate(s_p1_A, s_p2_A);
}

void wmax(BooleanCircuit *bc, share *s_p1_D, share *s_p1_NA, share *s_p1_A, share *s_p2_D, share *s_p2_NA, share *s_p2_A, share* &new_p_D, share* &new_p_NA, share* &new_p_A)
{
    share *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    // pi_0(p1 wmax p2) = pi_0(p1) AND pi_0(p2)
    new_p_D = bc->PutANDGate(s_p1_D, s_p2_D);

    // pi_NA(p1 wmax p2) = pi_NA(p1) OR p_NA(p2)
    new_p_NA = bc->PutORGate(s_p1_NA, s_p2_NA);

    // pi_1(p1 wmax p2) = (pi_1(p1) AND NOT pi_NA(p2)) OR (pi_1(p2) AND pi_NA(p1)) 
    s_inv_1 = bc->PutINVGate(s_p2_NA);
    s_inv_2 = bc->PutINVGate(s_p1_NA);

    s_op_temp1 = bc->PutANDGate(s_p1_A, s_inv_1);
    s_op_temp2 = bc->PutANDGate(s_p2_A, s_inv_2);

    new_p_A = bc->PutORGate(s_op_temp1, s_op_temp2);
}

void wmin(BooleanCircuit *bc, share *s_p1_D, share *s_p1_NA, share *s_p1_A, share *s_p2_D, share *s_p2_NA, share *s_p2_A, share* &new_p_D, share* &new_p_NA, share* &new_p_A)
{
    share *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    // pi_1(p1 wmin p2) = (pi_0(p1) AND NOT pi_NA(p2)) OR (pi_0(p2) AND pi_NA(p1)) 
    s_inv_1 = bc->PutINVGate(s_p2_NA);
    s_inv_2 = bc->PutINVGate(s_p1_NA);

    s_op_temp1 = bc->PutANDGate(s_p1_D, s_inv_1);
    s_op_temp2 = bc->PutANDGate(s_p2_D, s_inv_2);

    new_p_A = bc->PutORGate(s_op_temp1, s_op_temp2);

    // pi_NA(p1 wmin p2) = pi_NA(p1) OR p_NA(p2)
    new_p_NA = bc->PutORGate(s_p1_NA, s_p2_NA);

    // pi_1(p1 wmin p2) = pi_1(p1) AND pi_1(p2)   
    new_p_A = bc->PutANDGate(s_p1_A, s_p2_A);
}

void Complement(BooleanCircuit *bc, share *s_p1_D, share *s_p1_NA, share *s_p1_A, share* &new_p_D, share* &new_p_NA, share* &new_p_A)
{
    // pi_0(p) = pi_1(p)
    new_p_D = s_p1_A;

    // pi_NA(p) = pi_NA(p)
    new_p_NA = s_p1_NA;

    //pi_1(p) = pi_0(p)
    new_p_A = s_p1_D;
}

void Optional(BooleanCircuit *bc, share *s_p1_D, share *s_p1_NA, share *s_p1_A, share* &new_p_D, share* &new_p_NA, share* &new_p_A)
{
    // pi_0(p) = pi_1(p)
    new_p_D = bc->PutORGate(s_p1_A, s_p1_NA);

    // pi_NA(p) = pi_NA(p)
    new_p_NA = s_p1_NA;

    //pi_1(p) = pi_0(p)
    new_p_A = s_p1_A;
}

void PermitOverride(BooleanCircuit *bc, share *s_p1_D, share *s_p1_NA, share *s_p1_A, share *s_p2_D, share *s_p2_NA, share *s_p2_A, share* &new_p_D, share* &new_p_NA, share* &new_p_A)
{
    share *s_op, *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    //pi_0(p1 PO p2) = (pi_0(p1) AND pi_1(p2)) OR (pi_0(p2) AND pi_1(p1))
    s_inv_1 = bc->PutINVGate(s_p2_A);
    s_inv_2 = bc->PutINVGate(s_p1_A);

    s_op_temp1 = bc->PutANDGate(s_p1_D, s_inv_1);
    s_op_temp2 = bc->PutANDGate(s_p2_D, s_inv_2);

    new_p_D = bc->PutORGate(s_op_temp1, s_op_temp2);

    //pi_NA(p1 PO p2) = pi_NA(p1) AND pi_NA(p2)
    new_p_NA = bc->PutANDGate(s_p1_NA, s_p2_NA);

    //pi_1(p1 PO p2) = (pi_1(p1) OR pi_1(p2))
    new_p_A = bc->PutORGate(s_p1_A, s_p2_A);
}

void DenyOverride(BooleanCircuit *bc, share *s_p1_D, share *s_p1_NA, share *s_p1_A, share *s_p2_D, share *s_p2_NA, share *s_p2_A, share* &new_p_D, share* &new_p_NA, share* &new_p_A)
{
    share *s_op, *s_op_temp1, *s_op_temp2, *s_inv_1, *s_inv_2;

    //pi_0 (p1 DO p2) = pi_0(p1) OR pi_0(p2)
    new_p_D = bc->PutORGate(s_p1_D, s_p2_D);

    //pi_NA(p1 DO p2) = pi_NA(p1) AND pi_NA(p2)
    new_p_NA = bc->PutANDGate(s_p1_NA, s_p2_NA);

    //pi_1(p1 DO p2) = (pi_1(p1) AND pi_0(p2)) OR (pi_1(p2) AND pi_0(p1))
    s_inv_1 = bc->PutINVGate(s_p2_D);
    s_inv_2 = bc->PutINVGate(s_p1_D);

    s_op_temp1 = bc->PutANDGate(s_p1_A, s_inv_1);
    s_op_temp2 = bc->PutANDGate(s_p2_A, s_inv_2);

    new_p_A = bc->PutORGate(s_op_temp1, s_op_temp2);
}

void FirstAttribute(BooleanCircuit *bc, share *s_p1_D, share *s_p1_NA, share *s_p1_A, share *s_p2_D, share *s_p2_NA, share *s_p2_A, share* &new_p_D, share* &new_p_NA, share* &new_p_A)
{
    share *s_op_temp1, *s_op_temp2;

    //pi_0 (p1 FA p2) = pi_0(p1) OR (pi_NA(p1) AND pi_0(p2))
    s_op_temp1 = bc->PutANDGate(s_p1_NA, s_p2_D);
    new_p_D = bc->PutORGate(s_p1_D, s_op_temp1);

    //pi_NA(p1 FA p2) = pi_NA(p1) AND pi_NA(p2)
    new_p_NA = bc->PutANDGate(s_p1_NA, s_p2_NA);

    //pi_1 (p1 FA p2) = pi_1(p1) OR (pi_NA(p1) AND pi_1(p2))
    s_op_temp2 = bc->PutANDGate(s_p1_NA, s_p2_A);
    new_p_A = bc->PutORGate(s_p1_A, s_op_temp2);
}
