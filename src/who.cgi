#!/usr/local/bin/perl

open(IN, "ps -ax|fgrep rot|");
$a = 0;
while (<IN>) {
    chomp;
    chomp;
    if ($_ =~ /\.\/rot/) {
	$a = 1;
    }
}
close IN;
if ($a == 1) {
    system("rm -f output/tmp.fil");
    system("/usr/ucb/telnet localhost 4001 > output/tmp.fil 2>&1");
    open(IN, "output/rot.who");
    undef @All;
    while (<IN>) {
	chomp;
	chomp;
	push(@All, $_);
    }
    close IN;
    system("rm -f output/tmp.fil");
    print "Content-type: text/html\n\n"; 
    print "<HTML><HEAD><TITLE>Who's on RoT</TITLE></HEAD>\n"; 
    print "<BODY>\n"; 
    print "<center><H1>Users currently on Realms of Thoth<BR>\n"; 
    print "(<i>Visible at level 1</i>)</H1><HR><P>\n"; 
    print "<table border=4 cellspacing=4 cellpadding=3><tr>\n";
    print "<td align=middle><FONT COLOR=\"#009999\">Level</FONT></td>\n";
    print "<td align=middle><FONT COLOR=\"#009999\">Race</FONT></td>\n";
    print "<td align=middle><FONT COLOR=\"#009999\">Class</FONT></td>\n";
    print "<td align=middle><FONT COLOR=\"#009999\">Clan</FONT></td>\n";
    print "<td align=middle><FONT COLOR=\"#009999\">Name</FONT></td>\n<TR>\n";
    foreach $line (@All) {
	($level,$race,$class,$clan,$clandata,$clandatb,$name) = split(/,/, $line, 7);
	$level = int($level);
	if ($level == 0) {
	    next;
	}
	if ($level == 110) {
		$level = "<FONT COLOR=\"#00FF00\">IMP</FONT>";
	}
	if ($level == 109) {
		$level = "<FONT COLOR=\"#00FF00\">CRE</FONT>";
	}
	if ($level == 108) {
		$level = "<FONT COLOR=\"#00FF00\">SUP</FONT>";
	}
	if ($level == 107) {
		$level = "<FONT COLOR=\"#00FF00\">DEI</FONT>";
	}
	if ($level == 106) {
		$level = "<FONT COLOR=\"#00FF00\">GOD</FONT>";
	}
	if ($level == 105) {
		$level = "<FONT COLOR=\"#00FF00\">IMM</FONT>";
	}
	if ($level == 104) {
		$level = "<FONT COLOR=\"#00FF00\">DEM</FONT>";
	}
	if ($level == 103) {
		$level = "<FONT COLOR=\"#00FFFF\">KNI</FONT>";
	}
	if ($level == 102) {
		$level = "<FONT COLOR=\"#00FFFF\">SQU</FONT>";
	}
	if ($level == 101) {
		$level = "<FONT COLOR=\"#0000FF\">HRO</FONT>";
	}
	print "<td align=middle>${level}</td>\n";
	if ($race eq "Hfling") {
	    $race = "Halfling";
	}
	if ($race eq "Hf-Orc") {
	    $race = "Half-Orc";
	}
	if ($race eq "Hf-Elf") {
	    $race = "Half-Elf";
	}
	if ($race eq "Dracon") {
	    $race = "Draconian";
	}
	if ($race eq "Centr") {
	    $race = "Centaur";
	}
	if ($race eq "Heucuv") {
	    $race = "Heucuva";
	}
	if ($race eq "Minotr") {
	    $race = "Minotaur";
	}
	print "<td align=middle>${race}</td>\n";
	if ($class eq "Mag") {
	    $class = "<FONT COLOR=\"#000099\">Mage</FONT>";
	}
	if ($class eq "Cle") {
	    $class = "<FONT COLOR=\"#000099\">Cleric</FONT>";
	}
	if ($class eq "Thi") {
	    $class = "<FONT COLOR=\"#000099\">Thief</FONT>";
	}
	if ($class eq "War") {
	    $class = "<FONT COLOR=\"#000099\">Warrior</FONT>";
	}
	if ($class eq "Ran") {
	    $class = "<FONT COLOR=\"#000099\">Ranger</FONT>";
	}
	if ($class eq "Dru") {
	    $class = "<FONT COLOR=\"#000099\">Druid</FONT>";
	}
	if ($class eq "Vam") {
	    $class = "<FONT COLOR=\"#000099\">Vampire</FONT>";
	}
	if ($class eq "Wiz") {
	    $class = "<FONT COLOR=\"#990000\">Wizard</FONT>";
	}
	if ($class eq "Prs") {
	    $class = "<FONT COLOR=\"#990000\">Priest</FONT>";
	}
	if ($class eq "Mer") {
	    $class = "<FONT COLOR=\"#990000\">Mercenary</FONT>";
	}
	if ($class eq "Gla") {
	    $class = "<FONT COLOR=\"#990000\">Gladiator</FONT>";
	}
	if ($class eq "Str") {
	    $class = "<FONT COLOR=\"#990000\">Strider</FONT>";
	}
	if ($class eq "Sag") {
	    $class = "<FONT COLOR=\"#990000\">Sage</FONT>";
	}
	if ($class eq "Lic") {
	    $class = "<FONT COLOR=\"#990000\">Lich</FONT>";
	}
	print "<td align=middle>${class}</td>\n";
	if ($clan eq "X") {
	    print "<td align=middle> </td>\n";
	} else {
	    if ($clandata == 0) {
		print "<td align=middle><FONT COLOR=\"#FF0000\">${clan}</FONT></td>\n";
	    } elsif (($clandata == 1) && ($clandatb == 1)) {
		print "<td align=middle><FONT COLOR=\"#0000FF\">${clan}</FONT></td>\n";
	    } elsif (($clandata == 2) && ($clandatb == 1)) {
		print "<td align=middle><FONT COLOR=\"#FF00FF\">${clan}</FONT></td>\n";
	    } elsif (($clandata == 1) && ($clandatb == 0)) {
		print "<td align=middle><FONT COLOR=\"#000099\">${clan}</FONT></td>\n";
	    } else {
		print "<td align=middle><FONT COLOR=\"#990099\">${clan}</FONT></td>\n";
	    }
	}
	undef @orig;
	@orig = split(//, $name);
	$count = @orig;
	push(@orig, " ");
	$outname = "";
	$fonts = 0;
	for ($b = 0; $b < $count; $b++) {
	    $tmp = "$orig[$b]$orig[$b+1]";
	    $tmp2 = $orig[$b];
	    if ($tmp eq "{z") {
		if ($fonts == 0) {
		    $tmp2 = "<FONT COLOR=\"#000000\">";
		    $fonts = 1;
		} else {
		    $tmp2 = "</FONT><FONT COLOR=\"#000000\">";
		    $fonts = 1;
		}
		$b++;
	    } elsif (($tmp eq "{b")||($tmp eq "{4")){
		if ($fonts == 0) {
		    $tmp2 = "<FONT COLOR=\"#000099\">";
		    $fonts = 1;
		} else {
		    $tmp2 = "</FONT><FONT COLOR=\"#000099\">";
		    $fonts = 1;
		}
		$b++;
	    } elsif (($tmp eq "{c")||($tmp eq "{6")){
		if ($fonts == 0) {
		    $tmp2 = "<FONT COLOR=\"#009999\">";
		    $fonts = 1;
		} else {
		    $tmp2 = "</FONT><FONT COLOR=\"#009999\">";
		    $fonts = 1;
		}
		$b++;
	    } elsif (($tmp eq "{g")||($tmp eq "{2")){
		if ($fonts == 0) {
		    $tmp2 = "<FONT COLOR=\"#009900\">";
		    $fonts = 1;
		} else {
		    $tmp2 = "</FONT><FONT COLOR=\"#009900\">";
		    $fonts = 1;
		}
		$b++;
	    } elsif (($tmp eq "{m")||($tmp eq "{5")){
		if ($fonts == 0) {
		    $tmp2 = "<FONT COLOR=\"#990099\">";
		    $fonts = 1;
		} else {
		    $tmp2 = "</FONT><FONT COLOR=\"#990099\">";
		    $fonts = 1;
		}
		$b++;
	    } elsif (($tmp eq "{r")||($tmp eq "{1")){
		if ($fonts == 0) {
		    $tmp2 = "<FONT COLOR=\"#990000\">";
		    $fonts = 1;
		} else {
		    $tmp2 = "</FONT><FONT COLOR=\"#990000\">";
		    $fonts = 1;
		}
		$b++;
	    } elsif (($tmp eq "{w")||($tmp eq "{7")){
		if ($fonts == 0) {
		    $tmp2 = "<FONT COLOR=\"#CCCCCC\">";
		    $fonts = 1;
		} else {
		    $tmp2 = "</FONT><FONT COLOR=\"#CCCCCC\">";
		    $fonts = 1;
		}
		$b++;
	    } elsif (($tmp eq "{y")||($tmp eq "{3")){
		if ($fonts == 0) {
		    $tmp2 = "<FONT COLOR=\"#999900\">";
		    $fonts = 1;
		} else {
		    $tmp2 = "</FONT><FONT COLOR=\"#999900\">";
		    $fonts = 1;
		}
		$b++;
	    } elsif (($tmp eq "{B")||($tmp eq "{\$")){
		if ($fonts == 0) {
		    $tmp2 = "<FONT COLOR=\"#0000FF\">";
		    $fonts = 1;
		} else {
		    $tmp2 = "</FONT><FONT COLOR=\"#0000FF\">";
		    $fonts = 1;
		}
		$b++;
	    } elsif (($tmp eq "{C")||($tmp eq "{^")){
		if ($fonts == 0) {
		    $tmp2 = "<FONT COLOR=\"#00FFFF\">";
		    $fonts = 1;
		} else {
		    $tmp2 = "</FONT><FONT COLOR=\"#00FFFF\">";
		    $fonts = 1;
		}
		$b++;
	    } elsif (($tmp eq "{G")||($tmp eq "{@")){
		if ($fonts == 0) {
		    $tmp2 = "<FONT COLOR=\"#00FF00\">";
		    $fonts = 1;
		} else {
		    $tmp2 = "</FONT><FONT COLOR=\"#00FF00\">";
		    $fonts = 1;
		}
		$b++;
	    } elsif (($tmp eq "{M")||($tmp eq "{%")||($tmp eq "{-")){
		if ($fonts == 0) {
		    $tmp2 = "<FONT COLOR=\"#FF00FF\">";
		    $fonts = 1;
		} else {
		    $tmp2 = "</FONT><FONT COLOR=\"#FF00FF\">";
		    $fonts = 1;
		}
		$b++;
	    } elsif (($tmp eq "{R")||($tmp eq "{!")){
		if ($fonts == 0) {
		    $tmp2 = "<FONT COLOR=\"#FF0000\">";
		    $fonts = 1;
		} else {
		    $tmp2 = "</FONT><FONT COLOR=\"#FF0000\">";
		    $fonts = 1;
		}
		$b++;
	    } elsif (($tmp eq "{W")||($tmp eq "{&")){
		if ($fonts == 0) {
		    $tmp2 = "<FONT COLOR=\"#FFFFFF\">";
		    $fonts = 1;
		} else {
		    $tmp2 = "</FONT><FONT COLOR=\"#FFFFFF\">";
		    $fonts = 1;
		}
		$b++;
	    } elsif (($tmp eq "{Y")||($tmp eq "{#")){
		if ($fonts == 0) {
		    $tmp2 = "<FONT COLOR=\"#FFFF00\">";
		    $fonts = 1;
		} else {
		    $tmp2 = "</FONT><FONT COLOR=\"#FFFF00\">";
		    $fonts = 1;
		}
		$b++;
	    } elsif (($tmp eq "{D")||($tmp eq "{8")||($tmp eq "{*")){
		if ($fonts == 0) {
		    $tmp2 = "<FONT COLOR=\"#666666\">";
		    $fonts = 1;
		} else {
		    $tmp2 = "</FONT><FONT COLOR=\"#666666\">";
		    $fonts = 1;
		}
		$b++;
	    } elsif ($tmp eq "{{"){
		$tmp2 = "{";
		$b++;
	    } elsif (($tmp eq "{x")||($tmp eq "{X")||($tmp eq "{0")){
		if ($fonts == 1) {
		    $tmp2 = "</FONT>";
		    $fonts = 0;
		} else {
		    $tmp2 = "";
		}
		$b++;
	    } elsif ($tmp2 eq "{") {
                if ($fonts == 1) {
                    $tmp2 = "</FONT>";
                    $fonts = 0;
                } else {
                    $tmp2 = "";
                }
                $b++;
	    }
	    $tmp3 = "${outname}${tmp2}";
	    $outname = $tmp3;
	}
	if ($fonts == 0) {
	    print "<td align=left>${outname}</td>\n<TR>\n";
	} else {
	    print "<td align=left>${outname}</FONT></td>\n<TR>\n";
	}
    }
    print "</tr></table></font><P>\n";
    print "<PRE><B>Clan Color Codes</B>\n";
    print "------------------\n";
    print "<FONT COLOR=\"#FF0000\">Loner</FONT>\n";
    print "<FONT COLOR=\"#000099\">Pkill Member</FONT>\n";
    print "<FONT COLOR=\"#0000FF\">Pkill Leader</FONT>\n";
    print "<FONT COLOR=\"#990099\">Non-Pkill Member</FONT>\n";
    print "<FONT COLOR=\"#FF00FF\">Non-Pkill Leader</FONT>\n";
    print "\n\n";
    print "<B>Class Color Codes</B>\n";
    print "-------------------\n";
    print "<FONT COLOR=\"#000099\">1st tier</FONT>\n";
    print "<FONT COLOR=\"#990000\">2nd tier</FONT>\n";
    print "</PRE></center><P>\n";
} else {
    print "Content-type: text/html\n\n"; 
    print "<HTML><HEAD><TITLE>Who's on RoT</TITLE></HEAD>\n"; 
    print "<BODY>\n"; 
    print "<H2>The game appears to be down, try again later</H2>\n";
}
print "<HR><P></BODY>\n</HTML>\n";
exit;

