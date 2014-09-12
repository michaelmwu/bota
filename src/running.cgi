#!/usr/bin/perl

# This script is by Thyrr of Blades of the Apocalypse
# Since this is called by crontab, it is advised that you
# do NOT modify anything in here unless you know enough
# not to screw anything up.

# Move into the area directory; probably redundant
chdir("/home/thyrr/RTest/area/");

# Read in a list of the startup processes
open(IN, "ps ux|fgrep 'startup'|");
$a = 0;
while (<IN>) {
    chomp;
    chomp;
    # If the regular expression is found ( again, redunant )
    if ($_ =~ /startup/) {
        $a = 1;
        exit;
    }
}
close IN;

# Startup process is running presently!
if ($a == 1)
{
    # Absolutely do NOT remove/disable this line!
    exit;
}

# If the MUD was shut down, don't start up again
if (-e "shutdown.txt")
{
    exit;
}

# Kill anything that's running.
system("killall -q /home/thyrr/RTest/area/startup");
system("killall -q /home/thyrr/RTest/src/rot");

chdir("/home/thyrr/RTest/area/");

# Start the startup script
system("~/RTest/area/startup &");

# End program
exit;
