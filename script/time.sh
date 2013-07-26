
#!/bin/bash
T_SEP=","
H_SEP=","

PS="0 20 40 60 80 100"  #remember to change the field in the last command if you change the size of PS. f=PS+2
PN="20 60 100"

#no dots in the output name
OUTPUT="time_pn20-60_ps0-100"

TYPE="not_simplified"


for l in 2 4 8 16 32 64 128 256 512 1024; do 
    TITLE="LENGTH $l\n" 
    TABLE=""
    for ps in $PS; do 
	TABLE=$TABLE"${H_SEP}$ps"
    done
    TABLE=$TABLE"${H_SEP}AV${H_SEP}\n";
    AVCOLAV=""
    for pn in $PN; do 
	 ROW="$pn${T_SEP}";
	 ROWAV="";
	 for ps in $PS; do 
	     VAL="`cat ./length.$l/$l.pn$pn.ps$ps/results.$l.$pn.$ps.$TYPE | grep . | cut -f 2 -d ':' | awk '{sum+=$1} END {printf "%5.4f", sum/NR}'`"; 
	     ROWAV=$ROWAV"\n"$VAL
	     ROW=$ROW$VAL"${H_SEP}" 
	 done; 
	 ROWAV=`echo -e $ROWAV | sed '/^$/d' | awk '{sum+=$1} END {printf "%5.4f", sum/NR}'`; 
	 AVCOLAV=$AVCOLAV"\n"$ROWAV
	 ROW=$ROW$ROWAV"\n"
	 TABLE=$TABLE$ROW
    done 
    ROW="AV${H_SEP}"
    for ps in $PS; do
	COLAV=""
	for pn in $PN; do
	    VAL="`cat ./length.$l/$l.pn$pn.ps$ps/results.$l.$pn.$ps.$TYPE | grep . | cut -f 2 -d ':' | awk '{sum+=$1} END {printf "%5.4f", sum/NR}'`"; 
 	    COLAV=$COLAV"\n"$VAL
	done
	COLAV=`echo -e $COLAV | sed '/^$/d' | awk '{sum+=$1} END {printf "%5.4f", sum/NR}'`; 
	ROW=$ROW$COLAV"${H_SEP}"
    done
    AVCOLAV=`echo -e $AVCOLAV | sed '/^$/d' | awk '{sum+=$1} END {printf "%5.4f", sum/NR}'`; 
    TABLE=$TABLE$ROW$AVCOLAV
    echo -e "$TITLE$TABLE" | tee $OUTPUT.$l
done

for i in `ls ${OUTPUT}*`; do 
    NUM=`echo -n $i | cut -d '.' -f 2`;
    echo -n $NUM$H_SEP`tail -n 1 $i | cut -d ',' -f 2-` ;
    echo;  
done > total.ps$OUTPUT


# for i in `ls ${OUTPUT}*`; do 
#     IND=2
#     echo -n $i | cut -d '.' -f 2;
#     for ps in $PS; do
# 	echo -n " `tail -n 1 $i | cut -d ',' -f $IND` " ; 
# 	let IND=$IND+1;
#     done
#     echo;  
# done > total.ps$OUTPUT