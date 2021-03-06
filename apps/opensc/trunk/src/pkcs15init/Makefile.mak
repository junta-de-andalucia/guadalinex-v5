TOPDIR = ..\..

TARGET = pkcs15init.dll

HEADERS = pkcs15-init.h keycache.h
HEADERSDIR = $(TOPDIR)\src\include\opensc

OBJECTS = profile.obj pkcs15-lib.obj keycache.obj \
          pkcs15-miocos.obj pkcs15-gpk.obj pkcs15-cflex.obj \
          pkcs15-cardos.obj pkcs15-jcop.obj pkcs15-starcos.obj \
          pkcs15-oberthur.obj pkcs15-setcos.obj pkcs15-incrypto34.obj \
          pkcs15-muscle.obj pkcs15-asepcos.obj

all: install-headers $(TARGET)

!INCLUDE $(TOPDIR)\win32\Make.rules.mak

$(TARGET): $(OBJECTS)
	perl $(TOPDIR)\win32\makedef.pl $*.def $* $(OBJECTS)
	link $(LINKFLAGS) /dll /def:$*.def /implib:$*.lib /out:$(TARGET) $(OBJECTS) ..\scconf\scconf.lib ..\common\common.lib ..\libopensc\opensc.lib winscard.lib $(OPENSSL_LIB) gdi32.lib $(LIBLTDL_LIB)
	if EXIST $(TARGET).manifest mt -manifest $(TARGET).manifest -outputresource:$(TARGET);2
