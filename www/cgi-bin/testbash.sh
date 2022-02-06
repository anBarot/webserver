#!/bin/bash

echo "Content-type: text/html"
echo ""

echo "<html>"

echo "<style>.stylito{"
echo "font-size: 150%%;"
echo "padding: 100px 0;"
echo "color: blue;"
echo "background-image: url(\"/images/bash.png\")"
echo "}</style>"

echo "<head>"
echo "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">"
echo "<title>Bash CGI script</title>"
echo "</head>"
echo "<body class=\"stylito\">"
echo "<center>"
echo "<h1>Hey $USER ! You are now executing the script $SCRIPT_NAME</h1>"
echo "<center>"
echo "</body>"
echo "</html>"

exit 0
EOF
