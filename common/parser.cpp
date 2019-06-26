#include "parser.h"

/**
 *
 * Policy 1: not((T(1, 12, 1, (smin((T(3, 4, 4, (wmax((T(1, 6, 2, ([1]))), ([0]))))), (po((T(1, 5, 3, ([1]))), (T(6, 12, 1, ([1]))))))))))
 * Decision 1: [0, 0, 2]
 *
 * Policy 2: po((T(1, 12, 2, ([1]))), ([0]))
 * Decision 2: [0, 1, 2]
 *
 */

StringSet split_parsing(const String& line)
{
    String part;
    int open = 0;
    StringSet parts;

    for (int i = 0; i < line.length(); i++){
        if (line.substr(i, 1) == "("){
            open += 1;
            part += line.substr(i, 1);
        } else if (line.substr(i, 1) == ")") {
            open -= 1;
            part += line.substr(i, 1);
        } else if (line.substr(i, 1) == " " && open == 0){

        } else if (line.substr(i, 1) == "," && open == 0){
            parts.push_back(part);
            part = "";
        } else {
            part += line.substr(i, 1);
        }
    }

    parts.push_back(part);

    return parts;
}

Node* leaf_parsing(BooleanCircuit *bc, uint32_t bitlen, const String& line) {
    int val = std::stoi(line.substr(1,1));
    share *value = bc->PutINGate((uint32_t) val, bitlen, CLIENT); 
    Node *leaf = new Leaf(value);
    return leaf;
}

Node* target_parsing(BooleanCircuit *bc, uint32_t bitlen, String line) {
    Node *target;

    line = line.substr(2, line.length() - 3);
    StringSet parts = split_parsing(line);

    uint32_t a = std::stoi(parts[0]);
    uint32_t v = std::stoi(parts[1]);
    uint32_t c = std::stoi(parts[2]);
    share *attr = bc->PutINGate(a, bitlen, CLIENT);
    share *value = bc->PutINGate(v, bitlen, CLIENT);
    share *condition = bc->PutINGate(c, bitlen, CLIENT);

    String line1 = parts[3].substr(1, parts[3].length() - 2);
    Node *child1 = policy_parsing(bc, bitlen, line1);
    target = new Target(attr, value, condition, child1);

    return target;
}

Node* operation_parsing(BooleanCircuit *bc, uint32_t bitlen, String line) {
    Node *operation;

    if (line.substr(0, 4) == "not(") {
        line = line.substr(4, line.length() - 5);
        String line1 = line.substr(1, line.length() - 2);
        Node *child1 = policy_parsing(bc, bitlen, line1);

        operation = new Operation(NOT, child1);
    } else if (line.substr(0, 4) == "wea(") {
        line = line.substr(4, line.length() - 5);
        String line1 = line.substr(1, line.length() - 2);
        Node *child1 = policy_parsing(bc, bitlen, line1);

        operation = new Operation(WEA, child1);
    } else if (line.substr(0, 5) == "smax(") {
        line = line.substr(5, line.length() - 6);
        StringSet parts = split_parsing(line);
        String line1 = parts[0].substr(1, parts[0].length() - 2);
        String line2 = parts[1].substr(1, parts[1].length() - 2);
        Node *child1 = policy_parsing(bc, bitlen, line1);
        Node *child2 = policy_parsing(bc, bitlen, line2);

        operation = new Operation(SMAX, child1, child2);
    } else if (line.substr(0, 5) == "smin(") {
        line = line.substr(5, line.length() - 6);
        StringSet parts = split_parsing(line);
        String line1 = parts[0].substr(1, parts[0].length() - 2);
        String line2 = parts[1].substr(1, parts[1].length() - 2);
        Node *child1 = policy_parsing(bc, bitlen, line1);
        Node *child2 = policy_parsing(bc, bitlen, line2);

        operation = new Operation(SMIN, child1, child2);
    } else if (line.substr(0, 5) == "wmax(") {
        line = line.substr(5, line.length() - 6);
        StringSet parts = split_parsing(line);
        String line1 = parts[0].substr(1, parts[0].length() - 2);
        String line2 = parts[1].substr(1, parts[1].length() - 2);
        Node *child1 = policy_parsing(bc, bitlen, line1);
        Node *child2 = policy_parsing(bc, bitlen, line2);

        operation = new Operation(WMAX, child1, child2);
    } else if (line.substr(0, 5) == "wmin(") {
        line = line.substr(5, line.length() - 6);
        StringSet parts = split_parsing(line);
        String line1 = parts[0].substr(1, parts[0].length() - 2);
        String line2 = parts[1].substr(1, parts[1].length() - 2);
        Node *child1 = policy_parsing(bc, bitlen, line1);
        Node *child2 = policy_parsing(bc, bitlen, line2);

        operation = new Operation(WMIN, child1, child2);
    } else if (line.substr(0, 3) == "po(") {
        line = line.substr(3, line.length() - 4);
        StringSet parts = split_parsing(line);
        String line1 = parts[0].substr(1, parts[0].length() - 2);
        String line2 = parts[1].substr(1, parts[1].length() - 2);
        Node *child1 = policy_parsing(bc, bitlen, line1);
        Node *child2 = policy_parsing(bc, bitlen, line2);

        operation = new Operation(PO, child1, child2);
    } else if (line.substr(0, 3) == "do(") {
        line = line.substr(3, line.length() - 4);
        StringSet parts = split_parsing(line);
        String line1 = parts[0].substr(1, parts[0].length() - 2);
        String line2 = parts[1].substr(1, parts[1].length() - 2);
        Node *child1 = policy_parsing(bc, bitlen, line1);
        Node *child2 = policy_parsing(bc, bitlen, line2);

        operation = new Operation(DO, child1, child2);
    } else if (line.substr(0, 3) == "fa(") {
        line = line.substr(3, line.length() - 4);
        StringSet parts = split_parsing(line);
        String line1 = parts[0].substr(1, parts[0].length() - 2);
        String line2 = parts[1].substr(1, parts[1].length() - 2);
        Node *child1 = policy_parsing(bc, bitlen, line1);
        Node *child2 = policy_parsing(bc, bitlen, line2);

        operation = new Operation(FA, child1, child2);
    }

    return operation;
}

Node* policy_parsing(BooleanCircuit *bc, uint32_t bitlen, const String& line) {
    Node *policy;

    if (line.substr(0, 1) == "[") {
        policy = leaf_parsing(bc, bitlen, line);
    } else if (line.substr(0, 1) == "T") {
        policy = target_parsing(bc, bitlen, line);
    } else {
        policy = operation_parsing(bc, bitlen, line);
    }

    return policy;
}

Node* perform_policy_parsing(BooleanCircuit *bc, uint32_t bitlen, const String& line){
    std::cout << line << std::endl;
    String substring = line.substr(1, line.length() - 2);
    Node* policy = policy_parsing(bc, bitlen, substring);
    return policy;
}

StringSet split_query(const String& line)
{
    String part;
    String substring = line;
    StringSet parts;
    size_t pos = 0;

    while ((pos = substring.find("}, {")) != std::string::npos) {
        part = substring.substr(0, pos);
        parts.push_back(part);
        substring.erase(0, pos + 4);
    }

    parts.push_back(substring);
    return parts;
}

Query perform_query_parsing(BooleanCircuit *bc, uint32_t bitlen, String line){
    Query query;
    line = line.substr(2, line.length() - 4);
    StringSet parts = split_query(line);

    for (int i = 0; i < parts.size(); i++){
        StringSet numbers = split_parsing(parts[i]);
        uint32_t a = std::stoi(numbers[0]);
        uint32_t v = std::stoi(numbers[1]);
        Pair pair1 = {bc->PutINGate(a, bitlen, CLIENT),
                      bc->PutINGate(v, bitlen, CLIENT)};
        query.push_back(pair1);
    }

    return query;
}
