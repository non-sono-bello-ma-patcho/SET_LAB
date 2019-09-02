#!/bin/bash

declare -a servers=("incapache" "incapache2.2-debug")
port=$1
mkdir results

for serverName in "${servers[@]}"
do
	# launching students-server:
	./bin/$serverName www-root/ $port 2>&1 1>/dev/null &

	# testing GET Method:
	./request_script.sh $port GET > results/"${serverName}_GET_response.txt"

	# testing HEAD Method:
	./request_script.sh $port HEAD > results/"${serverName}_HEAD_response.txt"

	# killing server:
	echo -e "Stopping ${serverName} sever:"
	lsof -ti:$port | xargs kill -9
	port=$((port+1))
done

# check for differences:
echo -e "\n\e[92m"
diff -s results/*_GET_response.txt
diff -s results/*_HEAD_response.txt
echo -e "\n\e[0m"
rm -r results
