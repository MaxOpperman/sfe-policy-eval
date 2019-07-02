# Secure function evaluation for Policy Evaluation
## Files
- policy_test.cpp - entry point of the application reads CLI arguments.
- common/policy.h - definition of data structures (pair, query, triple, tree nodes)
- common/policy.cpp - implementation of target and policy evaluation, triple arithmetic and combination rules.
- common/parser.h - contains parser data structures (Strings and StringSets).
- common/parser.cpp - parses string represetnation of policiesi to actual datastructures.

## How to run
In one terminal do:
```
./policy_test -r 1
```
In another terminal do
```
./policy_test -r 0
```

If the results need to be formatted like in data/output.txt do:
```
./policy_test -r 0 | ./to_output.py
```
The results are automatically written to data/online-output.txt and data/total-output.txt.