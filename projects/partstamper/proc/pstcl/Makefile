
include $(PROJROOT)/make/project.make

# directory name of this leaf
#############################
LEAF = pstcl

#############################################
# source file list in this directory, fill in
#  CFILES         (*.c)
#  CCFILES        (*.cc)
#  CPPFILES       (*.cpp)
#############################################
CFILES = $(LEAF).c rt_showsched.c rt_rt.c rt_parse.c sendcmd.c
CCFILES =
CPPFILES =

# add system libraries here
# -l<library>
###########################
LDSYSLIBS += -ltcl8.6

# add local libraries here
# LDLOCLIBS += -l<library>
# DEPLOCLIBS += $(PLIB)/lib<library>.a
##########################
LDLOCLIBS +=
DEPLOCLIBS +=

include $(MAKEINC)/target.process

include $(LEAF).dpd

