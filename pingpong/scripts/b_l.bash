#!/bin/bash

set -e

declare -a arr=("tcp" "udp")
readonly DataDir='../data' 



for ProtocolName in "${arr[@]}"
do
    InputFile="${DataDir}/${ProtocolName}_throughput.dat"
    OutputPngFile="${DataDir}/${ProtocolName}_banda_latenza.png"
    OutputDatFile="${DataDir}/${ProtocolName}_delay.dat"
    if [ -e ${OutputDatFile} ]
     then 
     	echo  "*** removing older version *** "
        rm -v ${OutputDatFile} ${OutputPngFile} 
    fi
    #ottengo parametri
    #f2 0 f3 cioè devo usare T come valore mediano o medio?
    N1=$(head -n 1 ${InputFile}| cut -d' ' -f1 )
    T1=$(head -n 1 ${InputFile}| cut -d' ' -f3 )
    N2=$(tail -n 1 ${InputFile}| cut -d' ' -f1 )
    T2=$(tail -n 1 ${InputFile}| cut -d' ' -f3 )
    echo "***Ottengo parametri***   "
    echo "N1:${N1} T1:${T1} N2:${N2} T2:${T2}"
    
    #calcolo le costanti
    echo "***Calcolo costanti*** "
    DelayMin=$(bc <<<"scale=10;${N1}/${T1}")
    DelayMax=$(bc <<<"scale=10;${N2}/${T2}")
    Denominatore=$(bc <<< "scale=10;${DelayMax}-${DelayMin}")
    echo "Delay min:${DelayMin} Delay max:${DelayMax}"
    L=$(bc <<< "scale=10;( ( ${DelayMin} * ${N2} ) - ( ${DelayMax} * ${N1} ) ) / ( ${N2} - ${N1} )")
    B=$(bc <<< "scale=10;(${N2}-${N1})/${Denominatore}")
    echo "Latency:${L} Bandwidth:${B} "

    #stampa i valori Numero_byte e Latenza sul file .dat
    N_LINEE_FILE=$(wc -l ${DataDir}/${ProtocolName}_throughput.dat | cut -d' ' -f1)
    NUMERO_LINEA=1
    #echo "numero linea:${NUMERO_LINEA} numero linee file: ${N_LINEE_FILE}"
    
    #il ciclo arriva  a numero linee file meno 1
    echo "*** Stampo risultati su file *** "
    while [ ${NUMERO_LINEA} -lt ${N_LINEE_FILE} ]
    do 
        #echo ${NUMERO_LINEA}
        N=$(sed "${NUMERO_LINEA}q;d" ${DataDir}/${ProtocolName}_throughput.dat | cut -d' ' -f1)
        D=$(bc <<<"scale=10;( ${L} + ( ${N} / ${B} ) )")
        Latency_Bandwith=$(bc <<<"scale=10;${N} / ( ${D} )")
        echo "${NUMERO_LINEA} N:${N} D:${D} L_B:${Latency_Bandwith} "
        printf "$N ${Latency_Bandwith} \n" >> ${OutputDatFile}
        NUMERO_LINEA=$(bc <<<"${NUMERO_LINEA}+1")
    done
    echo "*** Creazione grafico *** "
gnuplot <<-eNDgNUPLOTcOMMAND
    set term png size 900,700
    
    set logscale x 2
	set logscale y 10
	set xlabel "msg size (B)"
	set ylabel "throughput (KB/s)"
	
	set output "../data/$OutputPngFile"
	plot "../data/${OutputDatFile}" using 1:2 title "Latency-Bandwidth model with L=${L} and B=${B}"\
	    with linespoint, \
	     "../data/${InputFile}" using 1:3 title "${ProtocolName} ping-pong Throughput (average)" \
			with linespoints
			
	clear
eNDgNUPLOTcOMMAND
echo "***done*** "
done
