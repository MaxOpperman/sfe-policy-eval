#/bin/bash

if [ $# -eq 0 ]
then
    echo "No arguments given"
    exit
fi

cp "data/experiment$1.txt" "data/current_experiment.txt"

LINES=$(wc -l data/current_experiment.txt |awk '{ print $1 }')

echo "Running experiment $1 consisting of $LINES policies";

while [ "$LINES" -ne 0 ]; do
    echo $LINES
    head -50 data/current_experiment.txt > data/experiment.txt
    sed -i -e "1,50d" data/current_experiment.txt
    LINES=$(wc -l data/current_experiment.txt | awk '{ print $1 }')
    ./policy_test -r 1 & ./policy_test -r 0 | ./to_output.py
done
