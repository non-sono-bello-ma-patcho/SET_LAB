#!/bin/bash
echo -e "\e[92mTESTING $2 METHOD\e[0m"
echo -e "\n\e[94mSending Bad request [syntax error in method]\e[0m"

netcat localhost $1 <<-EndTelnetCommand
	$2 /index.html HTTP1.0
	If-Modified-Since: Mon, 26 Nov 2014 12:12:12 GMT
EndTelnetCommand

echo -e "\n\e[94mSending Bad request [syntax error in method]\e[0m"

netcat localhost $1 <<-EndTelnetCommand
	$2 index.html HTTP/1.0
	If-Modified-Since: Mon, 26 Nov 2014 12:12:12 GMT
EndTelnetCommand

echo -e "\n\e[94mSending Bad request [syntax error in method]\e[0m"

netcat localhost $1 <<-EndTelnetCommand
	PIZZA /index.html HTTP1.0
	If-Modified-Since: Mon, 26 Nov 2014 12:12:12 GMT
EndTelnetCommand

echo -e "\n\e[94mSending Bad request [data format]\e[0m"

netcat localhost $1 <<-EndTelnetCommand
	$2 /index.html HTTP/1.0
	If-Modified-Since: Mon Nov 26 2014 12:12:12 GMT
EndTelnetCommand

echo -e "\n\e[94mSending Correct request [IMS 26/11/2014] \e[0m"

netcat localhost $1 <<-EndTelnetCommand
	$2 /index.html HTTP/1.0
	If-Modified-Since: Mon, 26 Nov 2014 12:12:12 GMT
EndTelnetCommand

echo -e "\n\e[94mSending Correct request [IMS 20/01/2018]\e[0m"

netcat localhost $1 <<-EndTelnetCommand
	$2 /index.html HTTP/1.0
	If-Modified-Since: Sat, 20 Jan 2018 12:12:12 GMT
EndTelnetCommand
