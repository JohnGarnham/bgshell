#!/bin/csh -f 
#
# $Id: sigtest.csh,v 1.4 2007/08/30 19:10:09 bksteele Exp $
#
# History:
# $Log: sigtest.csh,v $
# Revision 1.4  2007/08/30 19:10:09  bksteele
# now works for mac and solaris
#
# Revision 1.3  2007/08/30 16:56:57  bksteele
# updated notes
#
# Revision 1.2  2007/08/30 16:38:20  bksteele
# solaris and mac platform support for testing
#
# Revision 1.1.1.1  2007/07/30 21:32:13  bksteele
# signal handling lab for OS1 in C language
#
# demonstration usage: see runtest.csh
#
# detailed instruction usage: source sigtest.csh
# (NOTE: using source in c shell is important)
#
# program can run by itself, or be run by runtest.csh
#

set thePROG = showsignals

# if the program name is longer than 8 chars, the ps commmand will truncate it.
# therefore use an abbreviated name.
set thePROGshort = showsign

# initialize 
set procID=""
set pgmOUT = pgmOUT-00 
rm -f $pgmOUT

# startup the program 

    # it is best to feed lots of input to the shell to keep it from quitting,
    # but some platforms do not need such input to keep running in background.
    # netstat provides input and runs forever when given the right options.
    #
    # the general idea is this: run netstat forever, feeding its output
    # as input to the showsignals program.
    # capture all normal and error output of the signals program in a file, and
    # run the netstat and signals in the background.
    # example invocation: ( netstat -P tcp -s 1 | ./$thePROG ) >& $pgmOUT  & 

if ( `arch` == "i386" ) then
  # mac architecture netstat options
  set NETSTAT_OPTIONS = ( -w 1 )
else if ( `arch` == "sun4" ) then
  # sun4 architecture netstat options
  set NETSTAT_OPTIONS = ( -P tcp -s 1 )
else
  echo error: not tested under "`arch`" architecture. ASSUMING solaris netstat.
  set NETSTAT_OPTIONS = ( -P tcp -s 1 )
endif

echo a signal sent to a program is a number.
echo the signal number may be in the inclusive interval 0 ... 31
echo note: some platforms support even more signals.
echo ""
echo the unix kill command sends the signal to a running process.

# now execute the program in the background so we can send signals to it

    ( netstat $NETSTAT_OPTIONS | ./$thePROG ) >& $pgmOUT  & 
    sleep 3      # sleep allow the os to start the program

# test the first 32 signals (original unix)
foreach n ( 00 01 02 03 04 05 06 07 08 09 \
            10 11 12 13 14 15 16 17 18 19 \
            20 21 22 23 24 25 26 27 28 29 \
            30 31 )

  # (re)get the pid (PID will be "" if program died from a previous signal)

  set procID=(`ps -a | sed -e 's/^ *//' | grep $thePROGshort | grep -v grep | sed -e 's/  */ /g' | cut -f1 -d' ' |tail -1`)
  if ( $procID == "" ) then
    echo "restarting the program to continue tests for remaining signals..." 
    set pgmOUT = pgmOUT-$n 
    rm -f $pgmOUT
    ( netstat $NETSTAT_OPTIONS | ./$thePROG ) >& $pgmOUT  & 
    # sleep to allow os to start program
    sleep 3
    set procID=(`ps -a | sed -e 's/^ *//' | grep $thePROGshort | grep -v grep | sed -e 's/  */ /g' | cut -f1 -d' ' |tail -1`)
  endif

  ############# SAY WHAT'S HAPPENING
  echo kill -$n $procID 
  echo kill -$n $procID >>& $pgmOUT
  kill -$n $procID >>& $pgmOUT

end
# end of tests of signal numbers

set procID=(`ps -a | sed -e 's/^ *//' | grep $thePROGshort | grep -v grep | sed -e 's/  */ /g' | cut -f1 -d' ' |tail -1`)

if ( $procID != "" ) then
  # kill last remaining active process, if any
  kill -9 $procID 
endif

echo all done
exit $status

