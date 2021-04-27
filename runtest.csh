#!/bin/csh 
#
# $Id: runtest.csh,v 1.2 2007/08/30 16:38:20 bksteele Exp $
#
# History:
# $Log: runtest.csh,v $
# Revision 1.2  2007/08/30 16:38:20  bksteele
# solaris and mac platform support for testing
#
# Revision 1.1.1.1  2007/07/30 21:32:13  bksteele
# signal handling lab for OS1 in C language
#
# NOTE: this script must be sourced; it does not work when
#       directly invoked as a unix shell command.
#
# run the sigtest, hiding the results of uncaught signals
( source sigtest.csh ) |& egrep -iv 'Abort|Done|Killed|Stop|Suspend'

exit $status

