#!/usr/local/bin/perl

undef %Names;
undef %URL;

$maxlev = 110;
$minlev = 102;

# Set the following lines to reflect your MUD
$wizlist="/Rot/area/wizlist.txt";
$homeurl="http://www.mud.com/mud.html";
$mudname="Name of your MUD";

$Names{$maxlev}="Implementors";
$Names{$maxlev-1}="Creators";
$Names{$maxlev-2}="Supremacies";
$Names{$maxlev-3}="Deities";
$Names{$maxlev-4}="Gods";
$Names{$maxlev-5}="Immortals";
$Names{$maxlev-6}="DemiGods";
$Names{$maxlev-7}="Knights";
$Names{$maxlev-8}="Squires";

# The following names will be linked to a homepage
$URL{"Vengeance"}="http://www.lim.com/~russ";

    open(IN, "$wizlist");
    undef @All;
    while (<IN>)
    {
	chomp;
	chomp;
	push(@All, $_);
    }
    close IN;
    print "Content-type: text/html\n\n"; 
    print "<HTML><HEAD><TITLE>RoT Wizlist</TITLE></HEAD>\n"; 
    print "<BODY>\n";
    print "<center><H1>The Gods of ${mudname}</H1>\n"; 
    print "<HR><BR>\n";
    for ($a = $maxlev; $a >= $minlev; $a--)
    {
	$found = 0;
	foreach $tmp (@All)
	{
	    ($name,$level)=split(' ', $tmp);
	    if ($level == $a)
	    {
		$found = 1;
	    }
	}
	if ($found == 1)
	{
	    $title=$Names{$a};
	    print "<H2>${title} [${a}]</H2>\n";
	    if ($a > $maxlev-7)
	    {
		print "<font color=\"#00FF00\"><H3>\n";
	    } else
	    {
		print "<font color=\"#00FFFF\"><H3>\n";
	    }
	    foreach $tmp (@All)
	    {
		($name,$level)=split(' ', $tmp);
		if ($level == $a)
		{
		    if (defined($URL{$name}))
		    {
			$url=$URL{$name};
			print "<A HREF=\"${url}\">${name}</A><BR>\n";
		    } else
		    {
			print "${name}<BR>\n";
		    }
		}
	    }
	    print "</H3></font>\n";
	    print "<HR><BR>\n";
	}
    }
    print "<P></center>\n";
    print "Return to <A HREF=\"${homeurl}\">${mudname}</A> homepage.<P>\n";
    print "</BODY></HTML>\n";
exit;
