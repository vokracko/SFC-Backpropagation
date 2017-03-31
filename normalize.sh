#!/bin/bash 

MAX=""
MIN=""
LINES=`wc -l $1 | cut -d ' ' -f1`

> $1.normalized

for COL in `seq 1 3` ; do
	MAX+=`cut -d ' ' -f$COL $1 | cut -d '	' -f1 | sort -nr | head -n 1`,
	MIN+=`cut -d ' ' -f$COL $1 | cut -d '	' -f1 | sort -n | head -n 1`,
done

for i in `seq 0 100 $LINES` ; do
	head -n $i $1 | tail -n 100 > /tmp/sfc.$i
	while read LINE ; do
		L=""
		for COL in `seq 1 3`; do
			MAX_=`echo $MAX | cut -d ',' -f$COL`
			MIN_=`echo $MIN | cut -d ',' -f$COL`
			VAL_ORIG=`echo $LINE | cut -d ' ' -f$COL | cut -d '	' -f1`
			VAL=`echo "( $VAL_ORIG - $MIN_ ) / ( $MAX_ - $MIN_  + 0.00001) + 0.000001" | bc -l`
			# echo "( $VAL_ORIG - $MIN_ ) / ( $MAX_ - $MIN_  + 1)"
			L+=`printf "%f "  $VAL`
		done
		V=`echo $LINE | cut -d ' ' -f4`
		printf "%s %s\n" "${L:0:-1}" $V
	done < /tmp/sfc.$i > /tmp/sfc.$i.out &
done

echo "Waiting"

for job in `jobs -p` ; do
    wait $job 
	echo Done $job
done

for i in `seq 0 100 $LINES` ; do
	cat /tmp/sfc.$i.out >> $1.normalized
done
