######################################################################################
######################################################################################
## Copyright:»··© 3S - Smart Software Solutions GmbH, Kempten                       ##
## Program: »···Linux dynamic linking makefile                                      ##
## Module: »····Makefile                                                            ##
## Version:»····0.1                                                                 ##
## Author:      3s-software. (PhB)                                                  ##
######################################################################################

TARGET = CmpModbusTCP

INCLUDE = -I../ -I../../../Components -I../../../Platforms/Linux
BUILDUTILS = ../../../BuildUtils
LIB_DIR =../
LIB_NAME = lmodbus

CXX = arm-linux-gnueabihf-gcc
# TRG_X86 | TRG_PPC | TRG_ARM | TRG_CORTEX
CXXFLAGS = -DTRG_X86

DEBFLAGS = -g
LDLIBS = -lpthread
CXXFLAGS += -DALIGNATTRIB= -DCDECL= -DHANDLE_WIN32_PRAGMA -DDLL_DECL= -D_REENTRANT= -DHUGEPTR= 
CXXFLAGS += -O0 -Wall -Wno-unused
CXXFLAGS += $(INCLUDE)
CXXFLAGS += -DLINUX
CXXFLAGS += $(DEBFLAGS)
CXXFLAGS += -fPIC
LDFLAGS += $(LDLIBS)
LIBS += -L$(LIB_DIR)
LIBS += -$(LIB_NAME)

MAJORVER = 0
MINORVER = 1
SONAME = lib$(TARGET).so

SRC =  ../$(TARGET).c
OBJ = $(SRC:.c=.o)

all: $(SONAME)

$(SONAME): ../$(TARGET)Dep.h ../$(TARGET)Itf.h $(OBJ)   
	$(CXX) -shared $(CXXFLAGS) $(LDFLAGS) $(OBJ) -lc -Wl,-soname,$(SONAME) -o $(SONAME) $(LIBS)

.c.o: 
	$(CXX) -fPIC -c $(CXXFLAGS) -o $*.o $<
	
$(TARGET).o: $(OBJ)
	
../$(TARGET)Dep.h: ../$(TARGET)Dep.m4 ../$(TARGET)Itf.m4
	rm -f $(OBJ)
	fromdos ../$(TARGET)Dep.m4
	m4 -I$(BUILDUTILS)/M4Defs -I../../../Components -I../ Implementation.m4 - < ../$(TARGET)Dep.m4 > ../$(TARGET)Dep.h

../$(TARGET)Itf.h: ../$(TARGET)Itf.m4
	rm -f $(OBJ)
	fromdos ../$(TARGET)Itf.m4
	m4 -I$(BUILDUTILS)/M4Defs Exports.m4 - < ../$(TARGET)Itf.m4 > ../$(TARGET)Itf.h

clean:
	rm -f $(OBJ) *~ core .depend $(SONAME)

cleanall: clean
	rm -f ../$(TARGET)Dep.h ../$(TARGET)Itf.h

.depend:
	$(CXX) $(CXXFLAGS) -M $(SRC) > $@

ifeq (.depend,$(wildcard .depend))
include .depend
endif

