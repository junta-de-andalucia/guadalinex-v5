
TOPDIR = ..\..

TARGETS = base64.exe p15dump.exe \
	  p15dump.exe pintest.exe # prngtest.exe lottery.exe

all: print.obj sc-test.obj $(TARGETS)

!INCLUDE $(TOPDIR)\win32\Make.rules.mak

.c.obj:
	cl $(COPTS) /c $<

.c.exe:
	cl $(COPTS) /c $<
        link $(LINKFLAGS) /pdb:$*.pdb /out:$@ $*.obj sc-test.obj print.obj \
        ..\common\common.lib ..\libopensc\opensc.lib $(TOPDIR)\win32\version.res
	if EXIST $@.manifest mt -manifest $@.manifest -outputresource:$@;1

