#!/bin/bash
 
echo -e "**************  Test GET  *****************"

{ 
echo "GET / HTTP/1.1"; 
echo "Host: localhost:3000"; 
echo; sleep 1; 
} | telnet localhost 3000

echo -e "\n\n**************  Test DELETE  *****************"

touch ../www/delete/to_del.txt
{ 
echo "DELETE /delete/to_del.txt HTTP/1.1"; 
echo "Host: localhost:3000"; 
echo; sleep 1; 
} | telnet localhost 3000

echo -e "\n\n**************  Test POST  *****************"

echo -e "\n\n**********  Test with content length  *************"

{ 
echo "POST /file_to_upload.txt HTTP/1.1"; 
echo "Host: localhost:3000"; 
echo "content-length: 10";
echo "";
echo "123";
echo "456"; sleep 1;
} | telnet localhost 3000

echo -e "\n\n**********  Test with chunked  *************"
{ 
echo "POST /file_to_upload_2.txt HTTP/1.1"; 
echo "Host: localhost:3000"; 
echo "transfer-encoding: chunked";
echo "";
echo "5";
echo "abc";
echo "5";
echo "def";
echo "0";
echo; sleep 1; 
} | telnet localhost 3000

echo -e "\n\n**********  Test with cgi  *************"

echo -e "\n\n**********  Python test  *************"
{ 
echo "POST /cgi-bin/testpy.py HTTP/1.1"; 
echo "Host: 127.0.0.1:3000"; 
echo "Content-Length: 24";
echo "Content-Type: application/x-www-form-urlencoded";
echo;
echo "fname=anne&lname=barot";
sleep 1; 
} | telnet localhost 3000

echo -e "\n\n**********  Perl test  *************"

{ 
echo "POST /testpl.pl HTTP/1.1"; 
echo "Host: localhost:3000"; 
echo "content-length: 0";
echo; sleep 1; 
} | telnet localhost 3000

echo -e "\n\n**********  Bash test  *************"

{ 
echo "POST /testbash.sh HTTP/1.1"; 
echo "Host: localhost:3000"; 
echo "content-length: 0";
echo; sleep 1; 
} | telnet localhost 3000

echo -e "\n\n**********  redirect test  *************"

{ 
echo "GET /redirect HTTP/1.1"; 
echo "Host: localhost:3000"; 
echo; sleep 1; 
} | telnet localhost 3000

echo -e "\n\n**********  Max upload test  *************"

{
echo "POST /err_max.txt HTTP/1.1"; 
echo "Host: localhost:3000"; 
echo "content-length: 3";
echo "";
echo "0"
echo; sleep 1; 
} | telnet 127.0.0.2 3000

echo -e "\n\n**********  Method not implemented test  *************"

{
echo "NOPE / HTTP/1.1"; 
echo "Host: localhost:3000"; 
echo; sleep 1; 
} | telnet localhost 3000

echo -e "\n\n**********  Method not allowed test  *************"

{
echo "DELETE / HTTP/1.1"; 
echo "Host: localhost:3000"; 
echo; sleep 1; 
} | telnet localhost 3000

echo -e "\n\n**********  Length required test  *************"

{
echo "POST / HTTP/1.1"; 
echo "Host: localhost:3000";
echo; sleep 1; 
} | telnet localhost 3000

echo -e "\n\n**********  HTTP version not supported test  *************"

{
echo "POST / HTTP/2.1231"; 
echo "Host: localhost:3000";
echo; sleep 1; 
} | telnet localhost 3000

echo -e "\n\n**********  URI too long test  *************"

{
echo "POST /2i08e74Ms3vbE0AXBT4RXkTegtV4tygsW12y12hKAmwVRzPOnV26aF9AFDvAA0oWSgmjcFFP1CZVuZuV3g6hG31qOlINztCHzlH21wLglTmRiessRqGcU8odI6iMcDJVE9gz8elVWycFUq56Nc2trdmeVQLweF4T5LAQNnCdXGg3Eegx9RStMAdQ61aSKKnNig4a9PRueykeMLZEtvjwVwoL3M43pYJiTTicLOQcdZSOMRQL2AzQsgYDgfghyutujhkj HTTP/1.1"; 
echo "Host: localhost:3000";
echo; sleep 1; 
} | telnet localhost 3000

