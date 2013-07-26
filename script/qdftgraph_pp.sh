#!/usr/bin/env bash
n=96
echo "digraph G {"
echo "rankdir=LR"
grep -v "transfered=0" |
sed -e "s/^0 \[amount=\([0-9]*\),.*name=\"*\([^\"]*\)\"*\]/0 [style=dotted, label=\"\2\\\\\\\n\1\"]/g" \
		-e "s/\([0-9]*\) \[amount=\([0-9]*\), fake=0, name=src_\]/\1 [style=filled, label=\"src_\1\\\\\\\n\2\"]/g" \
		-e "s/\([0-9]*\) \[amount=\([0-9]*\), fake=1, name=src_\]/\1 [label=\"src_\1\\\\\\\n\2\"]/g" \
		-e "s/\([0-9]*\) \[amount=\([0-9]*\), fake=0, name=\"*\([^\"]*\)\"*\]/\1 [shape=box, style=filled, label=\"\3\\\\\\\n\2\"]/g" \
		-e "s/\([0-9]*\) \[amount=\([0-9]*\), fake=1, name=\"*\([^\"]*\)\"*\]/\1 [shape=box, label=\"\3\\\\\\\n\2\"]/g" \
		-e "s/transfered/label/g" | \
while read line; do
		echo $line | grep -q "digraph"
		if [ $? -eq 0 ]; then
				let ++n
				c=`printf "\x$(printf %x $n)"`
				echo $line | sed -e "s/digraph \(.*\) {/subgraph \1$c {/"
		else
				c=`printf "\x$(printf %x $n)"`
				echo $line | sed -e "s/\([0-9][0-9]*\)/$c\1/" -e "s/->\([0-9]*\)/->$c\1/"
		fi
done
echo "}"
