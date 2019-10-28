# Secure function evaluation for Policy Evaluation

## Installation
First install the packages required by ABY:
```
sudo apt-get install g++ make cmake libgmp-dev libssl-dev libboost-all-dev
```
After which you can clone the repository:
```
git clone https://gitlab.com/Restored/sfe-policy-eval.git --recursive
```
The recursive flag makes sure that the ABY submodule is also loaded.

## Files
- policy_test.cpp - entry point of the application reads CLI arguments.
- common/policy.h - definition of data structures (pair, query, triple, tree nodes)
- common/policy.cpp - implementation of target and policy evaluation, triple arithmetic and combination rules.
- common/parser.h - contains parser data structures (Strings and StringSets).
- common/parser.cpp - parses string represetnation of policiesi to actual datastructures.

## Running the experiments
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
