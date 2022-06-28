#/bin/bash

if [ $# -eq 0 ]
then
    echo "No arguments given"
    exit
fi

cp "data/experiment$1.txt" "data/current_experiment.txt"

LINES=$(wc -l data/current_experiment.txt |awk '{ print $1 }')

echo "Running experiment $1 consisting of $LINES policies";
start_time=$SECONDS

while [ "$LINES" -ne 0 ]; do
    head -50 data/current_experiment.txt > data/experiment.txt
    sed -i -e "1,50d" data/current_experiment.txt
    ./policy_test -r 2 -d 172.0.0.2 -a 172.0.0.3 -p 4080 -q 4081
    LINES=$(wc -l data/current_experiment.txt | awk '{ print $1 }')
done
elapsed=$((SECONDS - start_time))
eval "echo Elapsed time: $(date -ud "@$elapsed" +'$((%s/3600/24)) days %H hr %M min %S sec')"
