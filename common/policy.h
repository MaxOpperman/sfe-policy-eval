/**
 \file 		policy.h
 \author 	sreeram.sadasivam@cased.de
 \copyright	ABY - A Framework for Efficient Mixed-protocol Secure Two-party Computation
			Copyright (C) 2015 Engineering Cryptographic Protocols Group, TU Darmstadt
			This program is free software: you can redistribute it and/or modify
			it under the terms of the GNU Affero General Public License as published
			by the Free Software Foundation, either version 3 of the License, or
			(at your option) any later version.
			This program is distributed in the hope that it will be useful,
			but WITHOUT ANY WARRANTY; without even the implied warranty of
			MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
			GNU Affero General Public License for more details.
			You should have received a copy of the GNU Affero General Public License
			along with this program. If not, see <http://www.gnu.org/licenses/>.
 \brief		Implementation of the millionaire problem using ABY Framework.
 */
#ifndef __POLICY_H_
#define __POLICY_H_

#include "../ABY/src/abycore/circuit/booleancircuits.h"
#include "../ABY/src/abycore/circuit/arithmeticcircuits.h"
#include "../ABY/src/abycore/circuit/circuit.h"
#include "../ABY/src/abycore/aby/abyparty.h"
#include <math.h>
#include <cassert>


struct query { uint32_t attr; uint32_t value; };
struct target { uint32_t attr; uint32_t value; uint32_t condition; };
struct composed_target { target t1; target t2; int combiner; };
struct triple { share* permit; share* deny; share* na; };

int32_t perform_target_evaluation(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint32_t nvals, uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg,
		e_sharing sharing);

// Inclusion gate
share* PutINCGate(BooleanCircuit *bc, share *s_el, share *s_list[], int size);

// Atomic evaluation
triple evaluate(BooleanCircuit *bc, e_role role, uint32_t bitlen, query q, target t);
triple evaluate(BooleanCircuit *bc, share* Aq[], share* Vq[], share *s_target_a, share *s_target_v, share *s_target_c);


// Triple arithmetic
triple triple_addition(BooleanCircuit *bc, triple t1, triple t2);
triple triple_subtraction(BooleanCircuit *bc, triple t1, triple t2);
triple triple_scaling(BooleanCircuit *bc, triple t1, share *s);
triple triple_equality(BooleanCircuit *bc, triple t1, triple t2);


#endif /* __POLICY_H_ */
