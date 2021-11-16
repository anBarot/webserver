#!/bin/bash

echo "Content-type: text/html"
echo ""

echo "<html>"
echo "<head>"
echo "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">"
echo "<title>Bash CGI script</title>"
echo "</head>"
echo "<body>"
echo "<p>Hello, Your IP address is $REMOTE_ADDR</p>"
echo "</body>"
echo "</html>"

exit 0
EOF
