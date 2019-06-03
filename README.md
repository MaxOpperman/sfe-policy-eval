# Secure function evaluation for Policy Evaluation
## Files
- policy_test.cpp - entry point of the application reads CLI arguments.
- common/policy.h - definition of data structures (query, triple, tree nodes)
- common/policy.cpp - implementation of target and policy evaluation, triple arithmetic and combination rules.

## TODO
- Add composite target evaluation (if required)
- Test policy evaluation thoroughly
- Make tree randomizer

## Possible errors in the paper
- Line 19 of Algorithm 1, U should be F?
