#
# Created by gmakemake (Sparc Aug  2 2007) on Mon Nov 26 12:52:27 2007
#

#
# Definitions
#

.SUFFIXES:
.SUFFIXES:	.a .o .c .C .cpp .s
.c.o:
		$(COMPILE.c) $<
.C.o:
		$(COMPILE.cc) $<
.cpp.o:
		$(COMPILE.cc) $<
.s.o:
		$(COMPILE.cc) $<
.c.a:
		$(COMPILE.c) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%
.C.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%
.cpp.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%

CC =		gcc
CXX =		g++

RM = rm -f
AR = ar
LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)
LINK.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS)
COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) -c
COMPILE.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c

########## Default flags (redefine these with a header.mak file if desired)
CXXFLAGS =	-ggdb
CFLAGS =	-ggdb
CLIBFLAGS =	-lm
CCLIBFLAGS =	
########## End of default flags


CPP_FILES =	
C_FILES =	bgshell.c blah.c showsignals.c
S_FILES =	
H_FILES =	
SOURCEFILES =	$(H_FILES) $(CPP_FILES) $(C_FILES) $(S_FILES)
.PRECIOUS:	$(SOURCEFILES)
OBJFILES =	

#
# Main targets
#

all:	bgshell blah showsignals 

bgshell:	bgshell.o $(OBJFILES)
	$(CC) $(CFLAGS) -o bgshell bgshell.o $(OBJFILES) $(CLIBFLAGS)

blah:	blah.o $(OBJFILES)
	$(CC) $(CFLAGS) -o blah blah.o $(OBJFILES) $(CLIBFLAGS)

showsignals:	showsignals.o $(OBJFILES)
	$(CC) $(CFLAGS) -o showsignals showsignals.o $(OBJFILES) $(CLIBFLAGS)

#
# Dependencies
#

bgshell.o:	
blah.o:	
showsignals.o:	

#
# Housekeeping
#

Archive:	archive.tgz

archive.tgz:	$(SOURCEFILES) Makefile
	tar cf - $(SOURCEFILES) Makefile | gzip > archive.tgz

clean:
	-/bin/rm $(OBJFILES) bgshell.o blah.o showsignals.o core 2> /dev/null

realclean:        clean
	-/bin/rm -rf bgshell blah showsignals 
