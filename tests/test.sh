#!/bin/bash
while [ 1 ]
do
	curl 127.0.0.1:3000
	curl 127.0.0.1:3000/site
	curl 127.0.0.1:3000/empty
	curl 127.0.0.1:3000/images/arabidopsis_thaliana.jpg
	curl 127.0.0.1:3000/images/bash.png
	curl 127.0.0.1:3000/images/cydonia-warhol.jpg
	curl 127.0.0.1:3000/images/cydonia.jpg
	curl 127.0.0.1:3000/images/perl.png
	curl 127.0.0.1:3000/images/pythong.png
	curl 127.0.0.1:3000/method_not_allowed
	curl 127.0.0.1:3000/redirect
	curl 127.0.0.1:3000/google
	curl 127.0.0.1:3000/random
	curl 127.0.0.1:3001
	curl --resolve webserv_other:3000:127.0.0.1 webserv_other:3000
	curl 127.0.0.1:3002/error
	curl -X POST --data "lname=last_name&fname=first_name" http://localhost:3000/cgi-bin/testpy.py
	curl -X POST --data "" http://localhost:3000/cgi-bin/testbash.sh
	curl -X POST --data "" http://localhost:3000/cgi-bin/testpl.pl
done