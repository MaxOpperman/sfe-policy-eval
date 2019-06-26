#ifndef __PARSER_H_
#define __PARSER_H_

#include "policy.h"

typedef std::string String;
typedef std::vector<String> StringSet;

StringSet split_parsing(const String& line);
Node* leaf_parsing(BooleanCircuit *bc, uint32_t bitlen, const String& line);
Node* target_parsing(BooleanCircuit *bc, uint32_t bitlen, String line);
Node* operation_parsing(BooleanCircuit *bc, uint32_t bitlen, String line);
Node* policy_parsing(BooleanCircuit *bc, uint32_t bitlen, const String& line);
Node* perform_policy_parsing(BooleanCircuit *bc, uint32_t bitlen, const String& line);
StringSet split_query(const String& line);
Query perform_query_parsing(BooleanCircuit *bc, uint32_t bitlen, String line);

#endif /* __PARSER_H_ */
