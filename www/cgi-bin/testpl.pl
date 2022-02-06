#!/usr/bin/perl

print "Content-type: text/html\n\n";
print "<html><body>\n";
<GIF src='../images/macron-perlimpinpin.gif'>;
print "</body></html>\n";

use Unix::Passwd::File;
use File::Path;
use CGI;
use strict;

my $cgi = new CGI;

qx(perl index.pl);