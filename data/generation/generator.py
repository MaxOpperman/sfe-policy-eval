#!/usr/bin/env python3

import argparse
from random import choice, randint

DEBUG = False

attributes = range(1, 11)
values = [range((x - 1) * 10 + 1, x * 10 + 1) for x in attributes]
conditions = range(1, 5)
decisions = range(0, 2)

number_of_targets = 0
number_of_operations = 0
operators = ['smax', 'smin', 'wmax', 'wmin', 'po', 'do', 'fa', 'wea', 'not']

def generate_leaf():
	value = choice(decisions)
	return "[%d]" % value 

def generate_policy():
    global number_of_targets
    if number_of_targets > 0:
        rand = randint(0, 3)
        if rand == 0:
            return generate_operator()
        else:
            return generate_target()
    else:
        rand = randint(0, 2)
        if rand == 0:
            return 'not((' + generate_leaf() + '))'
        elif rand == 1:
            return 'wea((' + generate_leaf() + '))'
        else:
            return generate_leaf()

def generate_operator():
    operator = choice(operators)
    if operator == 'not':
        subtree = generate_policy()
        return 'not((%s))' % subtree
    elif operator == 'wea':
        subtree = generate_policy()
        return 'wea((%s))' % subtree
    else:
        rand = randint(0, 1)
        subtree1 = generate_policy()
        subtree2 = generate_policy()
        if rand == 0:
            return '%s((%s), (%s))' % (operator, subtree1, subtree2)
        else:
            return '%s((%s), (%s))' % (operator, subtree2, subtree1)

def generate_target():
    global number_of_targets
    attr = choice(attributes)
    val = choice(values[attr - 1])
    cond = choice(conditions)
    number_of_targets -= 1
    subtree = generate_operator()

    return "T(%d, %d, %d, (%s))" % (attr, val, cond, subtree)

def generate_target_experiment1():
    attr = choice(attributes)
    val = choice(values[attr - 1])
    cond = choice(conditions)
    leaf = generate_leaf()

    return "T(%d, %d, %d, (%s))" % (attr, val, cond, leaf)

def generate_operator_experiment2():
    global number_of_operations
    if number_of_operations == 0:
        return generate_dummy_target()
    elif number_of_operations == 1:
        rand = randint(0, 1)
        number_of_operations -= 1
        leaf = generate_operator_experiment2()
        if rand == 0:
            return 'wea((%s))' % leaf
        else:
            return 'not((%s))' % leaf
    else:
        op = choice(operators)
        if op == 'wea':
            number_of_operations -= 1
            left = generate_operator_experiment2()
            return 'wea((%s))' % left
        elif op == 'not':
            number_of_operations -= 1
            left = generate_operator_experiment2()
            return 'not((%s))' % left
        else:
            number_of_operations -= 1
            left = generate_operator_experiment2()
            right = generate_operator_experiment2()
            pos = randint(0, 1)
            if pos == 1:
                return '%s((%s), (%s))' % (op, left, right)
            else: 
                return '%s((%s), (%s))' % (op, right, left)

def generate_query(query_length):
    query = ""
    for x in range(0, query_length):
        attr = choice(attributes)
        val = choice(values[attr - 1])
        query += "{%s, %s}, " % (attr, val)

    return query[:-2]

def generate_dummy_target():
    decisions = [(1, 0, 0), (0, 1, 0), (0, 0, 1)]
    decision = choice(decisions)
    return 'D(%d, %d, %d)' % (decision[0], decision[1], decision[2])

def generate_experiment1(targets, operations, repetitions, query_length):
    for t in targets:
        for q in query_length:
            for x in range(1, repetitions + 1):
                query = generate_query(q)
                policy = generate_target_experiment1()

                print("[%d, %d, %d, %d]===[%s]===[%s]" % (t, operations, q, x, query, policy))

def generate_experiment2(targets, operations, repetitions, query_length):
    global number_of_operations
    for o in operations:
        for x in range(1, repetitions+1):
            number_of_operations = o
            policy = generate_operator_experiment2()
            print("[0, %d, 1, %d]===[{1, 1}]===[%s]" % (o, x, policy))

def generate_experiment3(repetitions, max_targets):
    global number_of_targets
    for t in range(1, max_targets + 1):
        for x in range(1, repetitions + 1):
            number_of_targets = t            
            policy = generate_policy()
            query = generate_query(10)
            print("[%d, x, 10, %d]===[%s]===[%s]" % (t, x, query, policy))

def main():
    targets = [1]
    operations = range(1, 21)
    repetitions = 50
    query_length = range(1,21)

    #generate_experiment3(repetitions, 50)
    generate_experiment2(targets, operations, repetitions, query_length)

if __name__ == "__main__":
    main()
