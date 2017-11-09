#!/bin/bash

set -e

declare -a arr=("tcp" "udp")
readonly DataDir='../data' 
declare -a L
declare -a B
declare -a N1
declare -a N2
declare -a T1
declare -a T2
declare -a Denominatore


for ProtocolName in "${arr[@]}"
do
    declare InputFile="${DataDir}/${ProtocolName}_throughput.dat"
    declare OutputPngFile="${DataDir}/${ProtocolName}_banda_latenza.png"
    declare OutputDatFile="${DataDir}/${ProtocolName}_delay.dat"
    
    if [ -e $OutputDatFile ]
    then
    	echo removing older version:
    	rm $OutputPngFile $OutputDatFile -f
    fi

    #ottengo parametri
    #f2 0 f3 cioè devo usare T come valore mediano o medio?
    # it's fun to find out you actually don't know what to do with these variables;
    # even funnier is the fact you don't even know which variable to use. I want to cry rn.
    
	# estraggo no byte del minimo:
    N1=$(head -n 1 ${DataDir}/${ProtocolName}_throughput.dat | cut -d' ' -f1)
    # estraggo delay del minimo:
    T1=$(head -n 1 ${DataDir}/${ProtocolName}_throughput.dat | cut -d' ' -f2)
    
	# estraggo no byte del massimo:
    N2=$(tail -n 1 ${DataDir}/${ProtocolName}_throughput.dat | cut -d' ' -f1)
    # estraggo delay del massimo:
    T2=$(tail -n 1 ${DataDir}/${ProtocolName}_throughput.dat | cut -d' ' -f2)
    
    #calcolo le costanti
    echo 'calcolo costanti'
    DelayMin=$(bc <<<"scale=20;var1=${N1};var2=${T1};var1 / var2")
    DelayMax=$(bc <<<"scale=20;var1=${N2};var2=${T2};var1 / var2")
    Denominatore=$(bc <<< "scale=20;${DelayMax}-${DelayMin}")
    
    L=$(bc <<< "scale=20;var1=${DelayMax}*${N2};var2=${DelayMin}*${N1};var3=var1-var2;var3/${Denominatore}")
    B=$(bc <<< "scale=20;var1=${N2}-${N1};var1/${Denominatore}")

    echo bandwiwidth: $B
    echo latency: $L
    #stampa i valori Numero_byte e Latenza sul file .dat
    echo 'stampo i valori Numero_byte e Latenza sul file .dat'
    N_LINEE_FILE=$(wc -l "${DataDir}/${ProtocolName}_throughput.dat" | cut -d ' ' -f1)
    NUMERO_LINEA=1

    while [ $NUMERO_LINEA -lt $N_LINEE_FILE ]
    do 
        N=$(sed "${NUMERO_LINEA}q;d" ${DataDir}/${ProtocolName}_throughput.dat | cut -d' ' -f1)
        D=$(bc <<<"scale=20;var1=${L};var2=${N};var3=${B};var1 + var2 / var3")
        printf "$N $D \n" >> ${OutputDatFile}
        ((NUMERO_LINEA++))
    done

gnuplot <<-eNDgNUPLOTcOMMAND
    set term png size 900,700
    
    set logscale x 2
	set logscale y 10
	set xlabel "msg size (B)"
	set ylabel "throughput (KB/s)"
	
	set output "../data/$OutputPngFile"
	plot "../data/${OutputDatFile}" using 1:2 title "Latency-Bandwidth model with L=${L0} and B=${B}"\
	    with linespoint, \
	     "../data/${InputFile}" using 1:3 title "${ProtocolName} ping-pong Throughput (average)" \
			with linespoints
			
	clear
eNDgNUPLOTcOMMAND


done































