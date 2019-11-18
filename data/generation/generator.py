#!/usr/bin/env python3

import argparse
from random import choice, randint

DEBUG = False

attributes = range(1, 11)
values = [range((x - 1) * 10 + 1, x * 10 + 1) for x in attributes]
conditions = range(1, 5)
decisions = range(0, 2)

number_of_targets = 0
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

def generate_query(query_length):
    query = ""
    for x in range(0, query_length):
        attr = choice(attributes)
        val = choice(values[attr - 1])
        query += "{%s, %s}, " % (attr, val)

    return query[:-2]

def generate_dummy_target(decision):
	return 'D(%d, %d, %d)' % (decision[0], decision[1], decision[2])

def generate_experiment1(targets, operations, repetitions, query_length):
    for t in targets:
        for q in query_length:
            for x in range(1, repetitions + 1):
                query = generate_query(q)
                policy = generate_target_experiment1()

                print("[%d, %d, %d, %d]===[%s]===[%s]" % (t, operations, q, x, query, policy))

def generate_experiment2(targets, operations, repetitions, query_length):
    decisions = [(1, 0, 0), (0, 1, 0), (0, 0, 1)]
    ops = ['smax', 'smin', 'wmax', 'wmin', 'po', 'do', 'fa']
    for k in range(1, repetitions+1):	
            for x in decisions:
                print("[0, 1, 1, %d]===[{1, 1}]===[wea((%s))]" % (k, generate_dummy_target(x)))
                print("[0, 1, 1, %d]===[{1, 1}]===[not((%s))]" % (k, generate_dummy_target(x)))
                for y in decisions:
                        for o in ops:
                                print("[0, 1, 1, %d]===[{1, 1}]===[%s((%s), (%s))]" % (k, o, generate_dummy_target(x), generate_dummy_target(y)))

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
    operations = 0
    repetitions = 50
    query_length = range(1, 21)

    #generate_experiment3(repetitions, 50)
    generate_experiment1(targets, operations, repetitions, query_length)

if __name__ == "__main__":
    main()
