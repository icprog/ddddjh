#!/bin/bash
if [ $# -ne 3 ];
then
echo "Usage : $0 filename start_line end_line";
exit -1
fi
declare -A count

filename=$1
start_line=$2
end_line=$3
sed -n "$start_line,$end_line p" $filename | \
egrep -o "\b[[:digit:]]+\.[[:digit:]]+\b"  | \
		  awk 'BEGIN{sum=0;} {count[$0]++ }
END{ printf("$-14s%s\n", "Word", "Count");
for (ind in count)
{	printf("%-14s%d\n", ind, count[ind]);
	let sum+=count[(ind+0)]*(ind+0);
}
printf("sum=%10.3f", sum);
}'

