#####################################################################
#
# FILE : project.make
#
# DESC : Generic project make include file for source tree makefiles
#        defines macros, implicit rules, etc.
#
# CVS  : $Id: project.make,v 1.6 2003/10/07 22:53:45 tmike Exp $
#
#        $Log: project.make,v $
#        Revision 1.6  2003/10/07 22:53:45  tmike
#        fixed dependency bug with local libraries
#
#        Revision 1.5  2003/09/30 05:03:01  tmike
#        Major Project Milestone, first BETA release
#
#        Revision 1.4  2003/09/27 22:47:40  tmike
#        Attempted to fix local library dependency issue
#
#        Revision 1.3  2003/09/23 18:30:24  tmike
#        SHM working, attempting to verify MSG
#
#        Revision 1.2  2003/09/20 20:02:07  tmike
#        added partstamper project
#
#        Revision 1.1  2003/08/26 19:20:44  tmike
#        Initial check-in of RPMS : Recursive Project Make System
#
#
#	Copyright (c) 2003 Tools Made Tough
#
#####################################################################


##############
# Project Name
##############
PROJECT = partstamper
#PROJECT = global

##############
# Architecture
##############
ARCHITECTURE = host
#ARCHITECTURE = x86
#ARCHITECTURE = ppc
#ARCHITECTURE = arm


##############################
# Global Directory Path Macros
##############################

MAKEINC = $(PROJROOT)/make

include $(MAKEINC)/project.$(ARCHITECTURE)

# Gxxx macros are global to all projects
########################################
GINC = $(PROJROOT)/projects/global/include
GLIB = $(PROJROOT)/projects/global/lib/lib
GBIN = $(PROJROOT)/projects/global/bin


# THISPROJ identifies the project currently
#          being referenced by this user
###########################################
THISPROJ = $(PROJROOT)/projects/$(PROJECT)


###############################
# Project Directory Path Macros
###############################
PINC = $(THISPROJ)/include
PLIB = $(THISPROJ)/lib/lib
PBIN = $(THISPROJ)/bin


################################
# Kernel paths for driver builds
################################
KERNEL_DIR = $(SYSROOT)/kernel
INCLUDE_DIR = $(KERNEL_DIR)/include

MODULE_DIR =


##########################
# Deployed filesystem path
##########################
MVLFS = /exports/rootfs/$(USER)


#######################
# Host Tool Name Macros
#######################
REMOVE = rm -f
COPY = cp -f
TOUCH = touch


##################
# Tool Flag Macros
##################

# dynamic command line flag for compiler
CL_FLAG =

# debug flags
DEBUG = -g -ggdb

# optimization flags
#OPTIMIZE = -O2

# base compiler command line flags
##################################
INCLUDE = -I. -I$(PINC) -I$(GINC)
COMMON_FLAGS = $(INCLUDE) $(OPTIMIZE) $(DEBUG) $(CL_FLAG)

# compiler flags
################
C_FLAGS = $(COMMON_FLAGS)
CC_FLAGS = $(COMMON_FLAGS)
CPP_FLAGS = $(COMMON_FLAGS)

# archiver flags
################
AR_FLAG = rv

# loader/linker flags
#####################
LDLIBDIRS = -L$(PLIB)
LDLOCLIBS = -lmessage -lmemory -llog -lio
LDSYSLIBS = -lm -lpthread

LDLIBS = $(LDSYSLIBS) $(LDLOCLIBS)
LD_FLAGS = $(LDLIBDIRS) $(LDLIBS)


############################
# Derived Target File Macros
############################
SOURCES = \
	$(CPPFILES) \
	$(CCFILES) \
	$(CFILES)

OBJECTS = \
	$(CFILES:.c=.o) \
	$(CCFILES:.cc=.o) \
	$(CPPFILES:.cpp=.o)

CLEAN = \
	$(OBJECTS)


########################
# Implicit Rules Section
########################
SUFFIXES = .o .c .cc .cpp
.SUFFIXES:
.SUFFIXES: $(SUFFIXES)

%.o : %.c
	$(CC) $(C_FLAGS) -c $<

%.o : %.cc
	$(CPP) $(CC_FLAGS) -c $<

%.o : %.cpp
	$(CPP) $(CPP_FLAGS) -c $<


