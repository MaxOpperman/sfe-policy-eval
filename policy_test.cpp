#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
#include "ABY/src/abycore/aby/abyparty.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "common/policy.h"
#include "common/parser.h"
#include <chrono>

int32_t read_test_options(int32_t* argcp, char*** argvp, e_role* role,
		uint32_t* bitlen, uint32_t* nvals, uint32_t* secparam, std::string* address,
		uint16_t* port, int32_t* test_op) {

	uint32_t int_role = 0, int_port = 0;
	bool useffc = false;

	parsing_ctx options[] =
			{ { (void*) &int_role, T_NUM, "r", "Role: 0/1", true, false }, {
					(void*) nvals, T_NUM, "n",
					"Number of parallel operation elements", false, false }, {
					(void*) bitlen, T_NUM, "b", "Bit-length, default 32", false,
					false }, { (void*) secparam, T_NUM, "s",
					"Symmetric Security Bits, default: 128", false, false }, {
					(void*) address, T_STR, "a",
					"IP-address, default: localhost", false, false }, {
					(void*) &int_port, T_NUM, "p", "Port, default: 7766", false,
					false }, { (void*) test_op, T_NUM, "t",
					"Single test (leave out for all operations), default: off",
					false, false }};

	if (!parse_options(argcp, argvp, options,
			sizeof(options) / sizeof(parsing_ctx))) {
		print_usage(*argvp[0], options, sizeof(options) / sizeof(parsing_ctx));
		std::cout << "Exiting" << std::endl;
		exit(0);
	}

	assert(int_role < 2);
	*role = (e_role) int_role;

	if (int_port != 0) {
		assert(int_port < 1 << (sizeof(uint16_t) * 8));
		*port = (uint16_t) int_port;
	}

	//delete options;

	return 1;
}

StringSet split_line(const String& line)
{
    String part;
    String substring = line;
    StringSet parts;
    size_t pos = 0;

    while ((pos = substring.find("===")) != std::string::npos) {
        part = substring.substr(0, pos);
        parts.push_back(part);
        substring.erase(0, pos + 3);
    }

    parts.push_back(substring);
    return parts;
}

int32_t simulate(e_role role, const std::string& address, uint16_t port, seclvl seclvl,	uint32_t nvals, uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing) {

    std::ifstream input("data/input.txt");
    std::ofstream output("data/output.txt");
    int number = 0;

    String line;

    while(std::getline(input, line)) {
        if(!line.empty()) {
            if (line.substr(0, 1) != "#") {
                number += 1;

                ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
                        mt_alg);

                std::vector<Sharing*>& sharings = party->GetSharings();
                Circuit* circ = sharings[sharing]->GetCircuitBuildRoutine();

                StringSet parts = split_line(line);
                String index = parts[0];
                Query query = perform_query_parsing((BooleanCircuit *) circ, bitlen, parts[1]);
                Node* policy = perform_policy_parsing((BooleanCircuit *) circ, bitlen, parts[2]);

                std::cout << "Evaluation started on policy " << number << "..." << std::endl;

                const clock_t begin_time = clock();
                Triple t = policy->evaluate((BooleanCircuit *)circ, role, bitlen, query);
                share *s_t = circ->PutOUTGate(t.t, ALL);
                share *s_u = circ->PutOUTGate(t.u, ALL);
                share *s_f = circ->PutOUTGate(t.f, ALL);
                party->ExecCircuit();
                float difference = float( clock() - begin_time ) / CLOCKS_PER_SEC;

                uint32_t t_output = s_t->get_clear_value<uint32_t>();
                uint32_t f_output = s_f->get_clear_value<uint32_t>();
                uint32_t u_output = s_u->get_clear_value<uint32_t>();

                output << index <<"===" <<"[" << t_output << ", " << f_output << ", " << u_output << "]===[" << difference << "]===[0]" << std::endl;

                delete party;
            }
            else {
                output << "# [0, 0, 0]===[0, 0, 0]===[0.000000]===[0]" << std::endl;
            }
        }
    }

    input.close();
    output.close();

	return 0;
}

int main(int argc, char** argv) {

	e_role role;
	uint32_t bitlen = 32, nvals = 31, secparam = 128, nthreads = 1;
	uint16_t port = 7766;
	std::string address = "127.0.0.1";
	int32_t test_op = -1;
	e_mt_gen_alg mt_alg = MT_OT;

	read_test_options(&argc, &argv, &role, &bitlen, &nvals, &secparam, &address,
			&port, &test_op);

	seclvl seclvl = get_sec_lvl(secparam);

    std::cout << "Starting the simulation!" << std::endl;

    simulate(role, address, port, seclvl, 1, 32, nthreads, mt_alg, S_BOOL);

    std::cout << "End of the simulation" << std::endl;

	return 0;
}

