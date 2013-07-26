#! /usr/bin/env bash
LANG=C
XP_PATH="/tmp/xp"
CACHE_PATH="/tmp/ex.pu$PUBLIC_LINES.se$SENSITIVE_LINES" #useless here, variables not assigned
NS=6
NC=9

# print log information w.r.t the verbosity level
log() {
    [[ $# -ge 2 ]] || die "log INFO|ERROR|DEBUG msg"
    local level="$1"
    shift
    local msg="$*"
    case $level in
	INFO ) [[ $VERBOSE -ge 1 ]] && echo "[INFO] $msg" 1>&2 ;;
	ERROR ) [[ $VERBOSE -ge 1 ]] && echo "[ERROR] $msg" 1>&2 ;;
	DEBUG ) [[ $VERBOSE -ge 2 ]] && echo "[DEBUG] $msg" 1>&2 ;;
    esac
    mode=$level
}

# Print error message and exit
die() {
		echo $*
		exit 1
}

reset() {
    CACHE_PATH="/tmp/ex.pu$PUBLIC_LINES.se$SENSITIVE_LINES"
		rm -rf "$XP_PATH"
		if [ $CACHE -eq 1 -a -d "$CACHE_PATH" ]; then
				cp -r "$CACHE_PATH" "$XP_PATH"
		else
				log INFO "Initializing the structure for $XP_PATH..."
				sum=0
				rm -rf "$CACHE_PATH"
				mkdir -p "$XP_PATH"
    		# Create sensitive specifications
				for f in $(seq 0 $NS); do
						fname="$XP_PATH/s$f"
						rm -f "$fname";
						touch "$fname";
						for l in $(seq $SENSITIVE_LINES); do
								echo sensitive_spec${f}_${l} >> "$fname";
						done
				done
				for f in $(seq 0 $NC); do
						fname="$XP_PATH/c$f"
						rm -f "$fname";
						touch "$fname";
						for l in $(seq $PUBLIC_LINES); do
								echo public_$l >> "$fname";
						done
				done
				cp -r "$XP_PATH" "$CACHE_PATH"
		fi
}

run() {
		trace="$1"
    log INFO "Executing the traces from $trace..."
    [[ -f "$trace" ]] || die "$trace must be a regular file"
		while read line; do
				log DEBUG "Line [$line]"
				local n=$(echo $line | cut -d' ' -f3)
				local c1="$XP_PATH/$(echo $line | cut -d' ' -f5)"
				local c2="$XP_PATH/$(echo $line | cut -d' ' -f7)"
        local size=$(wc -l $c1 | cut -d' ' -f1)
				if [ $size -ne 0 ]; then
						let r=n/size+1
						cat $(for i in $(seq $r); do echo -n "$c1 "; done) | shuf | head -n$n >> $c2
				else
						touch $c2
				fi
		done < $trace
}

show_results() {
    [[ -f "$GRAPH" ]] || die "$GRAPH must be a regular file"
    echo "========================================"
    printf '%10s | %6s | %6s | %6s | %5s\n' "filename" "lines" "qdft" "ulines" "PERF"
    local dests=($XP_PATH/c*)
    for f in ${dests[@]}; do
        local size=$(wc -l $f | cut -d' ' -f1)
        # grep sensitive $f | sort | uniq > /tmp/f.uniq
        local usize=$(grep sensitive $f | sort | uniq | wc -l)
				local cname=${f##*/}
        pdp=$(grep "name=$cname]" $GRAPH | grep "fake=0" | cut -d '=' -f2 | cut -d ',' -f1)
        if [ -z "$pdp" ]; then
            pdp=0
        fi
        if [ $size -gt $usize ]; then
          perf=$(echo "($size - $pdp) / ($size - $usize)" | bc -l 2>/dev/null)
        else
          perf=1
        fi
        printf '%10s | %6d | %6d | %6d | %.3f\n' "${f##*/}" $size $pdp $usize $perf
    done
}

# Usage
usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Run transfers described the trace file"
    echo "Options are:"
    echo "  -c, --no-cache    do not use the cache folder"
    echo "  -g, --graph FILE  set the file that contains the graph (default is $GRAPH)"
    echo "  -h, --help        display this help and exit"
    echo "  -p, --plines N    set the initial number of public lines (default is $PUBLIC_LINES)"
    echo "  -s, --slines N    set the initial number of sensitive lines (default is $SENSITIVE_LINES)"
    echo "  -t, --trace FILE  set the file which contains the trace (default is $TRACEFILE)"
    echo "  -v, --verbose     be verbose, use several times to be even more verbose"
}

# SCRIPT STARTS HERE
INTERACTIVE=0
TRACEFILE=transfers.log
GRAPH=graph.dot
VERBOSE=0
PUBLIC_LINES=1000
SENSITIVE_LINES=50000
CACHE=1
while [ "$1" != "" ]; do
    case $1 in
				-c | --no-cache )
						CACHE=0
						;;
				-g | --graph )
						shift
						GRAPH="$1"
						;;
				-h | --help )
						usage
						exit
						;;
				-p | --plines )
						shift
						PUBLIC_LINES="$1"
						;;
				-s | --slines )
						shift
						SENSITIVE_LINES="$1"
						;;
				-t | --trace )
						shift
						TRACEFILE="$1"
						;;
				-v | --verbose )
						VERBOSE=$((VERBOSE + 1))
						;;
	# Special cases
				--)
						break
						;;
				--*)
						echo "Error unknown (long) option $1"
						usage
						exit
						;;
				-?)
						echo "Error unknown (short) option $1"
						usage
						exit
						;;
	# Split apart combined short options
				-*)
						split=$1
						shift
						set -- $(echo "$split" | cut -c 2- | sed 's/./-& /g') "$@"
						continue
						;;
	# Done with options
				*)
						break
						;;
		esac
    shift
done

reset
run $TRACEFILE
show_results
