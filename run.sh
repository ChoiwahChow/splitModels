
# single threaded, no random invariants
#./splitModels -d5 -f"../bin/isofilter -ignore_constants" -m100 -oworking/m_ -tworking/semi_statistics.json -i"./outputs/semi5.out"

# 4 multi processes, no random invariants
./splitModels -d5 -f"../bin/isofilter -ignore_constants" -m100 -oworking/m_ -tworking/semi_statistics.json -i"./outputs/semi5.out" -p4
