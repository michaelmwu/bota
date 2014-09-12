#!/usr/bin/perl

$count = 0;
$total = 0;

open(IN, "find ~/bota/player/* -mtime -20 -print | xargs grep -h \'Levl [0-9]*$\' |");
while (<IN>) {
    chomp;
    chomp;
    if ($_ =~ /\d+/) {
        if( int($&) > 1 && int($&) < 102 ) {
            $count++;
            $total = $total + int($&);
        }
        else {
            if( int($&) == 1 ) {
                $ncount++;
                $ntotal = $ntotal + int($&);
            }
        }
    }
}
close IN;

$average = $total / $count;
$naverage = ($ntotal + $total) / ($ncount + $count);

print("$total / $count = $average\n");
print("Newbies ($ncount) = $naverage\n");

