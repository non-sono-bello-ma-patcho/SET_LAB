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
    readonly InputFile="${DataDir}/${ProtocolName}_throughput.dat"
    readonly OutputPngFile="${DataDir}/${ProtocolName}_banda_latenza.png"
    readonly OutputDatFile="${DataDir}/${ProtocolName}_delay.dat"
    #ottengo parametri
    #f2 0 f3 cioè devo usare T come valore mediano o medio?
    N1=$(head -n 1 ${DataDir}/${ProtocolName}_throughput.dat | cut -d' ' -f1)
    T1=$(head -n 1 ${DataDir}/${ProtocolName}_throughput.dat | cut -d' ' -f3)
    N2=$(tail -n 1 ${DataDir}/${ProtocolName}_throughput.dat | cut -d' ' -f1)
    T2=$(tail -n 1 ${DataDir}/${ProtocolName}_throughput.dat | cut -d' ' -f3)
    
    #calcolo le costanti
    echo 'calcolo costanti'
    DelayMin=$(bc <<<"scale=20;var1=${N1};var2=${T1};var1 / var2")
    DelayMax=$(bc <<<"scale=20;var1=${N2};var2=${T2};var1 / var2")
    Denominatore=$(bc <<< "scale=20;${DelayMax}-${DelayMin}")
    
    L=$(bc <<< "scale=20;var1=${DelayMax}*${N2};var2=${DelayMin}*${N1};var3=var1-var2;var3/${Denominatore}")
    B=$(bc <<< "scale=20;var1=${N2}-${N1};var1/${Denominatore}")

    echo $B

    #stampa i valori Numero_byte e Latenza sul file .dat
    echo 'stampo i valori Numero_byte e Latenza sul file .dat'
    N_LINEE_FILE=$(wc -l "${DataDir}/${ProtocolName}_throughput.dat" | cut -d ' ' -f1)
    NUMERO_LINEA=1
    
    echo "numero linee: $N_LINEE_FILE"
    echo $NUMERO_LINEA
    while [ $NUMERO_LINEA -lt $N_LINEE_FILE ]
    do 
        N=$(sed "${NUMERO_LINEA}q;d" ${DataDir}/${ProtocolName}_throughput.dat | cut -d' ' -f1)
        D=$(bc <<<"scale=20;var1=${L};var2=${N};var=${B};var1 + var2 / var3")
        printf "$N $D \n" >> ${OutputDatFile}
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































