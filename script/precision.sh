#!/bin/bash
[ $# -eq 1 ] || { echo "wrong number of parameters"; exit 1; }
TRACE_PATH=$(pwd)
SCRIPT_HOME=$(cd `dirname $(readlink -e $0)` && pwd)

mkdir -p precision
# rm -f precision/*
for i in `find $TRACE_PATH | grep trace | shuf`;
do
    echo $i;
    suff=`echo ${i##*/} | cut -d '.' -f 2-`;
    path=`dirname $i`;
    echo "$TRACE_PATH/precision/res.$suff $(test -f ./precision/res.$suff && echo OK)"
    if [ ! -e $TRACE_PATH/precision/res.$suff ]; then 
	touch $TRACE_PATH/precision/res.$suff
	if [ -f $path/graph_simplified.$suff ]; then 
	    $SCRIPT_HOME/run_trace.sh -p 10000 -s 50000 -g $path/graph_simplified.$suff -t $i -v >  $TRACE_PATH/precision/res.$suff
	else
	    $SCRIPT_HOME/run_trace.sh -p 10000 -s 50000 -g $path/graph_not_simplified.$suff -t $i -v >  $TRACE_PATH/precision/res.$suff
	fi
    fi
done
