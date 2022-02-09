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

#echo -e "\n\n**********  Test with content length  *************"
#
#{ 
#echo "POST /file_to_upload.txt HTTP/1.1"; 
#echo "Host: localhost:3000"; 
#echo "content-length: 10";
#echo "";
#echo "123";
#echo "456";
#echo; sleep 1;
#} | telnet localhost 3000

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

#echo -e "\n\n**********  Python test  *************"
#
#{ 
#echo "POST /testpy.py HTTP/1.1"; 
#echo "Host: localhost:3000"; 
#echo "content-length: 22";
#echo ""
#echo "lname=coco&fname=choco"
#echo; sleep 1; 
#} | telnet localhost 3000

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
