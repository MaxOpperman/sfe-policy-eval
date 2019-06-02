#include "../ABY/src/abycore/circuit/booleancircuits.h"
#include "../ABY/src/abycore/circuit/circuit.h"

triple smax(BooleanCircuit *bc, triple t1, triple t2);
triple smin(BooleanCircuit *bc, triple t1, triple t2);
triple wmax(BooleanCircuit *bc, triple t1, triple t2);
triple wmin(BooleanCircuit *bc, triple t1, triple t2);
triple Not(BooleanCircuit *bc, triple t1);
triple wea(BooleanCircuit *bc, triple t1);
triple po(BooleanCircuit *bc, triple t1, triple t2);
triple Do(BooleanCircuit *bc, triple t1, triple t2);
triple fa(BooleanCircuit *bc, triple t1, triple t2);
