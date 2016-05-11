#!/bin/bash
var1=2
var2=3
sum=0
declare -A ass
ass[1]=4
ass[2]=5
let sum+=ass[var1]*var2;
echo ${!ass[*]}
echo ${ass[*]}
#for index in ${!ass[*]}
#do
#echo ${ass[$index]}
#echo $index
#printf("index:%s,value:%d", index, ass[$index])
#echo "index:$index,value:${ass[$index]}"
#done
 
for (ind in ass)
{
#printf("%s,%d\n", ind,ass[ind])
echo $ind
}
