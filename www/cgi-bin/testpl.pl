#!/usr/bin/perl

print "Content-type:text/html\r\n\r\n";
print '<!DOCTYPE html>';

print '<style>.stylito{';
print 'font-size: 150%;';
print 'padding: 300px 0;';
print 'color: blue;';
print '}</style>';

print '<head>';
print '<title>Hello Word - First CGI Program</title>';
print '</head>';
print '<body call="stylito">';
print '<center>';
print '<h2>This is the perl cgi script !</h2>';
print '<img src="/images/perl.png">';
print '<center>';
print '</body>';
print '</html>';

1;