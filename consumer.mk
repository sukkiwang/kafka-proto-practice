INCLIBS = -lrdkafka++
INCLIBS += -lrdkafka
INCLIBS += -lssl
INCLIBS += -lcrypto -ldl
INCLIBS += -llz4
INCLIBS += -lzstd
INCLIBS += -ldl


PCDEPS = bmsdiscovery
PCDEPS += audtschemas
RS_LIBS =

# -*- makefile -*-
# Generated on nytsdev1 at Tue Apr 26 10:18:13 2016 by tliu240 using:
# /bbsrc/bin/prod/bin/bde_metamkmk.pl 'test.mk' '-t' 'other'
# $Id$ $CSID$

TASK=consumer.tsk

# Use the SRCS macro to limit the source files that will be compiled and
# linked into your task to only those listed. If SRCS is not used then all
# source files in the current directory will be used.
#SRCS=

# Use the SRCDIRS macro to specify subdirectories that should be searched in
# addition to the current directory for source files. If SRCS has been
# specified explicitly then SRCDIRS has no effect.
#SRCDIRS=

# remove this to disable checking for gcc warnings
IS_GCC_WARNINGS_CLEAN=yes

USER_CFLAGS   += -I.
USER_CPPFLAGS += -I.
USER_FFLAGS   += -I.

# Specify here which libraries from the RS_Suite you need, e.g.: xerces z


# The LIBS macro should always contain $(INCLIBS) which is what llcalc
# controls. You can add additional libraries in LIBS before $(INCLIBS) but not
# after. llcalc will rewrite the INCLIBS macro completely each time it is run
# so avoid making modifications to INCLIBS.


LIBS= $(INCLIBS)

LLCALC_PKG_INCLUDE +=
LLCALC_SET_INCLUDE +=
LLCALC_PKG_EXCLUDE +=
LLCALC_SET_EXCLUDE +=

# do not modify below this point
METAMKMK_VER=1.0
IS_CPPMAIN=1
IS_DEPENDS_NATIVE=1
MKINCL?=/bbsrc/mkincludes/
include $(MKINCL)sourcelist.mk
include $(LIBMACROS_MK)
OBJS?=$(OBJS_AR)
include $(MKINCL)machdep.newlink
include $(MKINCL)linktask.newlink
# vim:ft=make
