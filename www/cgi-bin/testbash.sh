#!/bin/bash

echo "Content-type: text/html"
echo ""

echo "<html>"
echo "<head>"
echo "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">"
echo "<title>Bash CGI script</title>"
echo "</head>"
echo "<body>"
echo "<p>Hey $USER ! You are now executing the script $SCRIPT_NAME, enjoy! \^^/</p>"
echo "</body>"
echo "</html>"

exit 0
EOF
