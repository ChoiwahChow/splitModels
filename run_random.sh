
mkdir -p working parallel 
# single threaded, no random invariants
#./splitModels -d5 -f"../bin/isofilter -ignore_constants" -m100 -oworking/m_ -tworking/semi_statistics.json -i"./outputs/semi5.out" s1000 -r50 -l20 -x3000

# 4 multi processes, no random invariants
./splitModels -d5 -f"../bin/isofilter -ignore_constants" -m100 -oworking/m_ -tworking/semi_statistics.json -i"./outputs/semi5.out" -p4 -s1000 -r50 -l20 -x3000
