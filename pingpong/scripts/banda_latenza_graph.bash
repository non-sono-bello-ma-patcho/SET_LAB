#!/bin/bash

set -e

declare -a arr=("tcp" "udp")
readonly DataDir='../data' 



for ProtocolName in "${arr[@]}"
do
    readonly InputFile="${DataDir}/${ProtocolName}_throughput.dat"
    readonly OutputPngFile="${DataDir}/${ProtocolName}_banda_latenza.png"
    readonly OutputDatFile="${DataDir}/${ProtocolName}_delay.dat"
    #ottengo parametri
    #f2 0 f3 cioè devo usare T come valore mediano o medio?
    N1=head -n 1 ${ProtocolName}_throughput.dat | cut -d' ' -f1
    T1=head -n 1 ${ProtocolName}_throughput.dat | cut -d' ' -f3
    N2=tail -n 1 ${ProtocolName}_throughput.dat | cut -d' ' -f1
    T2=tail -n 1 ${ProtocolName}_throughput.dat | cut -d' ' -f3
    #calcolo le costanti
    DelayMax=$N1 \ $T1
    DelayMin=$N2 \ $T2 | bc -q
    L0=($DelayMax*$N2-$DelayMin*$N1) \ ($DelayMax-$DelayMin) | bc -q
    B=($N2-$N1) \ ($DelayMax-$DelayMin) | bc -q
    #stampa i valori Numero_byte e Latenza sul file .dat
    N_LINEE_FILE=wc -l ${ProtocolName}_throughput.dat
    
    while [ 
        N=sed "${NUMERO_LINEA}q;d" ${ProtocolName}_throughput.dat | cut -d' ' -f1
        D=$L0 + $N \ $B | bc -q
        printf "$N $D \n" >> ${OutputDatFile}
    


done


gnuplot <<-eNDgNUPLOTcOMMAND
    set term png size 900,700
    
    set logscale x 2
	set logscale y 10
	set xlabel "msg size (B)"
	set ylabel "throughput (KB/s)"
	
	set output "../data/tcp_banda_latenza.png"
	plot "../data/tcp_delay.dat" using 1:2 title "Latency-Bandwidth model with L=$L0 and B=$B"\
	    with linespoint, \
	     "../data/tcp_throughput.dat" using 1:3 title "TCP ping-pong Throughput (average)" \
			with linespoints
			
			
	set output "../data/udp_banda_latenza.png"
	plot "../data/udp_delay.dat" using 1:2 title "Latency-Bandwidth model with L=$L0 and B=$B"\
	    with linespoint, \
	     "../data/udp_throughput.dat" using 1:3 title "UDP ping-pong Throughput (average)" \
			with linespoints
	clear
eNDgNUPLOTcOMMAND


































