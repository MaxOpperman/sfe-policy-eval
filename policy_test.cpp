#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
#include "ABY/src/abycore/aby/abyparty.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

// Server side C/C++ program to demonstrate Socket programming
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common/policy.h"
#include "common/parser.h"
#include <chrono>

int32_t read_test_options(int32_t* argcp, char*** argvp, e_role* role,
		uint32_t* bitlen, uint32_t* nvals, uint32_t* secparam, std::string* server_address,
                          uint16_t* server_port, std::string* stp_address,
                          uint16_t* stp_port, int32_t* test_op) {

	uint32_t int_role = 0, int_server_port = 0, int_stp_port = 0;
	bool useffc = false;

	parsing_ctx options[] =
			{ { (void*) &int_role, T_NUM, "r", "Role: 0/1/2 for STP/Data Server/Access Requester", true, false }, {
					(void*) nvals, T_NUM, "n",
					"Number of parallel operation elements", false, false }, {
					(void*) bitlen, T_NUM, "b", "Bit-length, default 32", false,
					false }, { (void*) secparam, T_NUM, "s",
					"Symmetric Security Bits, default: 128", false, false }, {
                    (void*) server_address, T_STR, "d",
                    "Data Server IP-address, default: localhost", false, false }, {
                    (void*) stp_address, T_STR, "a",
                    "STP IP-address, default: localhost", false, false }, {
                    (void*) &int_server_port, T_NUM, "p", "Data Server Port, default: 7766", false,
                    false }, {
                    (void*) &int_stp_port, T_NUM, "q", "STP Port, default: 7766", false,
                    false }, { (void*) test_op, T_NUM, "t",
					"Single test (leave out for all operations), default: off",
					false, false }};

	if (!parse_options(argcp, argvp, options,
			sizeof(options) / sizeof(parsing_ctx))) {
		print_usage(*argvp[0], options, sizeof(options) / sizeof(parsing_ctx));
		std::cout << "Exiting" << std::endl;
		exit(0);
	}

	*role = (e_role) int_role;

    // set requester and server ports
    if (int_server_port != 0) {
        assert(int_server_port < 1 << (sizeof(uint16_t) * 8));
        *server_port = (uint16_t) int_server_port;
    }
    if (int_stp_port != 0) {
        assert(int_stp_port < 1 << (sizeof(uint16_t) * 8));
        *stp_port = (uint16_t) int_stp_port;
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

String simulate(e_role role, const std::string& address, uint16_t port, seclvl seclvl, uint32_t nvals,
                 uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing) {
    std::ifstream input("data/experiment.txt");
    int number = 0;

    String line;

    while(std::getline(input, line)) {
        if(!line.empty()) {
            if (line.substr(0, 1) != "#") {
                number += 1;

                std::cout << address << ":" << port << " with role (0-Server/1-Client) " << role << std::endl;

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

                if (role == SERVER) {
                    uint32_t t_output = s_t->get_clear_value<uint32_t>();
                    uint32_t f_output = s_f->get_clear_value<uint32_t>();
                    uint32_t u_output = s_u->get_clear_value<uint32_t>();

                    std::cout << index <<"===" <<"[" << t_output << ", " << f_output << ", " << u_output << "]===" << std::endl;
                }

                delete party;
            }
            else {
                if(role == SERVER) {
                    std::cout << "# [0, 0, 0]===[0, 0, 0]===[0.000000]===[0]" << std::endl;
                }
            }
        }
    }

    input.close();

    return "";
}


String send_request(const std::string& address_server, uint16_t port_server) {
    int sock = 0, read_value, client_fd;
    struct sockaddr_in serv_addr;
    char* hello = "Hello from client";
    char buffer[1024] = { 0 };

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return "ERROR CREATING SOCKET";
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_server);

    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, address_server.c_str(), &serv_addr.sin_addr)
        <= 0) {
        printf(
                "\nInvalid address/ Address not supported \n");
        return "ERROR INVALID ADDRESS";
    }

    if ((client_fd
                 = connect(sock, (struct sockaddr*)&serv_addr,
                           sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return "ERROR CONNECTION FAILED";
    }
    send(sock, hello, strlen(hello), 0);
    std::cout << "Hello message sent from Access Requester!" << std::endl;
    read_value = read(sock, buffer, 1024);
    std::cout << "Received on Access Requester: " << buffer << std::endl;

    // closing the connected socket
    close(client_fd);
    return buffer;
}

int32_t receive_request(const std::string& address_ds, uint16_t port_ds, const std::string& stp_address,
                        uint16_t stp_port, seclvl seclvl, uint32_t nvals, uint32_t bitlen, uint32_t nthreads,
                        e_mt_gen_alg mt_alg) {
    int server_fd, new_socket, valread;
    int opt = 1;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    char* hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_ds);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket
                 = accept(server_fd, (struct sockaddr*)&address,
                          (socklen_t*)&addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read(new_socket, buffer, 1024);

    std::cout << "Received on Data Server: " << buffer << std::endl;
    simulate(CLIENT, stp_address, stp_port, seclvl, 1, 32, nthreads, mt_alg, S_BOOL);

    send(new_socket, hello, strlen(hello), 0);
    std::cout << "Hello message sent from Data Server!" << std::endl;

    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}

int32_t simulate_request(e_role role, const std::string& ds_address, uint16_t ds_port, const std::string& stp_address,
                         uint16_t stp_port, seclvl seclvl, uint32_t nvals, uint32_t bitlen, uint32_t nthreads,
                         e_mt_gen_alg mt_alg) {

    std::cout << "Creating access request role " << role << " (0 Data Server / 1 - Access Requester)" << std::endl;

    if (role == SERVER) {
        receive_request(ds_address, ds_port, stp_address, stp_port, seclvl, nvals, bitlen, nthreads, mt_alg);
    } else {
        const clock_t begin_time = clock();
        std::string decision = send_request(ds_address, ds_port);
        float difference = float( clock() - begin_time ) / CLOCKS_PER_SEC;
        std::cout << "Receiving decision " << decision << " from Data Server took " << difference << " seconds." << std::endl;
    }

    return 0;
}

int main(int argc, char** argv) {

	e_role role;
	uint32_t bitlen = 32, nvals = 31, secparam = 114, nthreads = 1;
    uint16_t port_ds = 7766;
    std::string address_ds = "127.0.0.1";
    uint16_t port_stp = 7767;
    std::string address_stp = "127.0.0.1";
	int32_t test_op = -1;
	e_mt_gen_alg mt_alg = MT_PAILLIER;

	read_test_options(&argc, &argv, &role, &bitlen, &nvals, &secparam,
                      &address_ds, &port_ds, &address_stp, &port_stp, &test_op);

	seclvl seclvl = get_sec_lvl(secparam);

    std::cout << "Starting the simulation! Role " << role << std::endl;

    // if the program is running the access requester; send the request to the data server
    if (role == ALL) {
        simulate_request(CLIENT, address_ds, port_ds, address_stp, port_stp, seclvl, 1, 32, nthreads, mt_alg);
    }
    // simulate the client; first receive the request, then simulate the data server, and send back the result
    else if (role == CLIENT) {
        simulate_request(SERVER, address_ds, port_ds, address_stp, port_stp, seclvl, 1, 32, nthreads, mt_alg);
    }
    // otherwise the role is the server (STP)
    else {
        simulate(role, address_stp, port_stp, seclvl, 1, 32, nthreads, mt_alg, S_BOOL);
    }

    std::cout << "End of the simulation for " << role << std::endl;

	return 0;
}

