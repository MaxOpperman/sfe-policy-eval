# Privacy-preserving policy evaluation using secure function evaluation (ABY)

## Installation
First install the packages required by ABY:
```
sudo apt-get install g++ make cmake libgmp-dev libssl-dev libboost-all-dev
```
After which you can clone the repository:
```
git clone https://github.com/IschaStork/sfe-policy-eval.git --recursive
```
The recursive flag makes sure that the ABY submodule is also loaded.

## Structure
- policy_test.cpp - entry point of the application reads CLI arguments.
- common/policy.h - definition of data structures (pair, query, triple, tree nodes)
- common/policy.cpp - implementation of target and policy evaluation, triple arithmetic and combination rules.
- common/parser.h - contains parser data structures (Strings and StringSets).
- common/parser.cpp - parses string representation of policies and transforms them into actual data structures.
- data/generation/generator.py - generates policies for the experiments

## Running the experiments

Make sure that ```PRINT_PERFORMANCE_STATS``` and ```PRINT_COMMUNICATION_STATS``` are set to 1 in ABY/src/abycore/ABY_utils/ABYconstants.h

```
./experiment.sh EXPID
```
where EXPID is the experiment id (1 for target evaluation, 2 for operations, 3 for complex policies). 
The results are automatically written to data/online-output.txt and data/total-output.txt.
