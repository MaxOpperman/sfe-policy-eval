#!/usr/bin/env python3

from random import choice

DEBUG = False

attributes = range(1, 11)
values = [range((x - 1) * 10 + 1, x * 10 + 1) for x in attributes]
conditions = range(1, 5)
decisions = range(0, 2)

def generate_target():
    attr = choice(attributes)
    val = choice(values[attr - 1])
    cond = choice(conditions)
    leaf = choice(decisions)

    return "T(%d, %d, %d, ([%d]))" % (attr, val, cond, leaf)

def generate_query(query_length):
    query = ""
    for x in range(0, query_length):
        attr = choice(attributes)
        val = choice(values[attr - 1])
        query += "{%s, %s}, " % (attr, val)

    return query[:-2]

def generate(targets, operations, repetitions, query_length):
    for t in targets:
        for q in query_length:
            for x in range(1, repetitions + 1):
                query = generate_query(q)
                policy = generate_target()

                print("[%d, %d, %d, %d]===[%s]===[%s]" % (t, operations, q, x, query, policy))

def main():
    targets = [1]
    operations = 0
    repetitions = 50
    query_length = range(1, 21)


    if DEBUG:
        print("Starting generator")
        print("Targets: %d \nOperations: %d \nRepetitions: %d \nQuery length: %d \n" % (len(targets), operations, repetitions, len(query_length)))

    generate(targets, operations, repetitions, query_length)

if __name__ == "__main__":
    main()
