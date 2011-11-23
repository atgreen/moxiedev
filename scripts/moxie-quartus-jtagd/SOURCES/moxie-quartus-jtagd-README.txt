This package assumes you have installed Quartus II in /opt/altera.

This has been tested with Quartus II v11.0 SP1 on 64-bit x86 Fedora
15.


Once installed, start the quartus-jtagd service like so...

# service quartus-jtagd start


Check the status like so...

# service status quartus-jtagd


You can set it up to start by default like so...

# chkconfig quartus-jtagd on


Be sure to remember to "touch ~/.jtag-conf" as yourself before running
the programmer.


Send any comments, correction or test results to green@moxielogic.com.

The author is not affiliated with Altera.



Anthony Green 
green@moxielogic.com


