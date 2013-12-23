#!/bin/bash
LANG=C

if [ $# -eq 1 ]; then
		TESTLIB="$1"
else
		TESTLIB=/home/lovat/qdft-lazy/qdftgraph/src/testlib
fi

SCRIPT_HOME=$(cd `dirname $(readlink -e $0)` && pwd)
test_folder="/home/lovat/test_lazy/lazy-simpl-old"
precision_folder="${test_folder}/precision"
slp_folder="${test_folder}/slp"


from=2
to=8192
step=2

block=4096

psrc_list="50"
pnew_list="0 25 50 100"

pest_list="0 50 100"

multiple_tests_num=30

seed="0"
sourcesize=50000
publicsize=10000
type="simplified"


mkdir -p $test_folder
mkdir -p $precision_folder
mkdir -p $slp_folder

real_seed=123

i=$from


time_func() {
    [[ $# -ne 5 ]] && { echo "wrong number of parameters for time_func()"; exit 1; }

    local command="$1" 
    local output_file="$2" 
    local prefix_string="$3"
    local stdout="$4"
    local stderr="$5"

    if [ -f /usr/bin/time ]; 
    then
	/usr/bin/time -a -o $output_file -f "${prefix_string} %U\n" $command > $stdout 2> $stderr
    else
	(TIMEFORMAT="${prefix_string} %3U"; time $command > $stdout 2> $stderr) 2>> $output_file	
    fi
}



graph_generation() {
    [[ $# -ne 10 ]] && { echo "wrong number of parameters for graph_generation()"; exit 1; }

    local TESTLIB="$1" 
    local i="$2" 
    local block="$3"
    local psrc="$4"
    local pnew="$5"
    local real_seed="$6"
    local pest="$7"
    local graph="$8"
    local trace="$9"
    local resfile="${10}"

    time_func "$TESTLIB $i $block $psrc $pnew $real_seed $sourcesize $publicsize 0 $pest 1" "$resfile.${real_seed}" "${i}.${psrc}.${pnew}.pe${pest}.${real_seed}:" "$graph" "$trace"
}

precision() {
    [[ $# -ne 3 ]] && { echo "wrong number of parameters for precision()"; exit 1; }

    local graph="$1" 
    local trace="$2" 
    local precision_output="$3"

    $SCRIPT_HOME/run_trace.sh -p $publicsize -s $sourcesize -g $graph -t $trace >  $precision_output 
}

short_long_path() {
    [[ $# -ne 3 ]] && { echo "wrong number of parameters for short_long_path()"; exit 1; }

    local graph="$1" 
    local slp_output="$2" 
    local precision_output="$3"
    
    declare -A qdftvalues
    while read line; do
	cname=$(echo $line | cut -d '=' -f4 | cut -d']' -f1)
	amount=$(echo $line | cut -d '=' -f2 | cut -d',' -f1)
	qdftvalues[$cname]=$amount
    done < <(grep 'fake=0' $graph)

    declare -A precvalues
    while read line; do
	cname=$(echo $line | cut -d '|' -f1 | sed -e 's/^ *//;s/ *$//')
	size=$(echo $line | cut -d '|' -f2 |  sed -e 's/^ *//;s/ *$//')
	usize=$(echo $line | cut -d '|' -f3 | sed -e 's/^ *//;s/ *$//')
	qdft="${qdftvalues[$cname]}"
        if [ -z "$qdft" ]; then
            qdft=0
        fi
        if [ $size -gt $usize ]; then
          perf=$(echo "($size - $qdft) / ($size - $usize)" | bc -l 2>/dev/null)
        else
          perf="1.00000"
        fi
	precvalues[$cname]=$perf
    done < <(grep 'c' $precision_output)

    tmpfile=$(mktemp)
    cat $graph | grep -v 'transfered=0' > $tmpfile
    # $SCRIPT_HOME/shortest_longest_paths $tmpfile | grep c | grep -v src > $slp_output
    while read c; do 
	cont=$(echo $c | cut -f 1 -d ':'); 
	# echo $c$(cat $precision_output | grep "$cont |" | cut -d '|' -f 5 | cut -d ' ' -f2)
	echo $c$(printf "%.5f" ${precvalues[$cont]})
    done > $slp_output < <($SCRIPT_HOME/shortest_longest_paths $tmpfile | grep c | grep -v src)
    
    rm -f $tmpfile
}

sig_handler() {
    echo
    echo "Cleaning: $trace $graph $precision_output $slp_output"
    rm -f "$trace" "$graph" "$precision_output" "$slp_output"
    exit 1;
}

trap sig_handler SIGHUP SIGTERM SIGINT; 

x=1
pe=0;
while [ $x -le $multiple_tests_num ]; do
    echo "Test nr ${x}"
    i=$from;
    while [ $i -le $to ]; do 
	echo "Length of trace=${i}"
	experiment_folder="length.${i}"
	mkdir -p $test_folder/$experiment_folder
	for pnew in $pnew_list; do
	    echo "  (Exp:${x} - Length:${i}) Pnew = ${pnew}"
	    for psrc in $psrc_list; do
		for pe in $pest_list; do
		    test_path=$test_folder/$experiment_folder/$i.pn$pnew.ps$psrc
		    resfile=$test_path/time.$i.$pnew.$psrc.${type}
		    suff=$i.$psrc.$pnew.pe$pe.$real_seed;
		    graph=$test_path/graph_${type}.$suff
		    trace=$test_path/trace.$suff
		    precision_output="${precision_folder}/precision.$suff"
		    slp_output="${slp_folder}/slp.$suff"
		    echo -n "    (${x}-${i}-${pnew}) Psrc = ${psrc}... "
		    mkdir -p $test_path
		    if [ -f $trace ]; then
			echo "skipped."
		    else
			touch $trace
			echo -n "graph creation..."
			graph_generation "$TESTLIB" "$i" "$block" "$psrc" "$pnew" "$real_seed" "$pe" "$graph" "$trace" "$resfile"
			echo -n "done! Precision computation..."

			#  precision "$graph" "$trace" "$precision_output"
			echo -n -e "done! Slp computation...no longer needed. "

			# short_long_path "$graph" "$slp_output" "$precision_output"
			echo "Done! :-)"
		    fi
		    let real_seed=(real_seed+3)%65536;
		done
	    done
	done
	let i=i*step;
	
	# if [ $i -gt $to ]; 
	# then
	#     let real_seed=(real_seed+81)%65536; 
	# fi

    done  
    let x=x+1;
done
