#!/usr/bin/python
# Import modules for CGI handling
import cgi, cgitb
cgitb.enable()
# Create instance of FieldStorage
form = cgi.FieldStorage()
# Get data from fields
first_name = form.getvalue('fname')
last_name  = form.getvalue('lname')
print ("Content-type:text/html\r\n\r\n")
print ("<!DOCTYPE html>")
print("<style>.supastyle{")
print ("font-size: 150%%;")
print ("padding: 100px 0;")
print ("color: blue;")
print ("}</style>")
print ("<head>")
print ("<title>Hello - Second CGI Program</title>")
print ("</head>")
print ("<body class=\"supastyle\">")
print ("<center>")
print ("<h1>Hello %s %s\n\n\n</h1>" % (first_name, last_name))
print ("<img src=\"/images/python.png\">")
print ("<center>")
print ("</body>")
print ("</html>")
