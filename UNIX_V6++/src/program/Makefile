#
# Makefile for Unix V6++ Shell
#

#命令定义
CC = gcc
LD = ld
OBJDUMP = objdump

#参数定义-nostartfiles
CFLAGS = -w -O0 -g -nostdlib -nostartfiles -nostdinc -fno-builtin -fno-exceptions

#目录定义
TARGET = .\objs
V6++LIB = "..\lib\Lib_V6++.a"
INCLUDE = .
LIB_INCLUDE = ..\lib\include

MAKEIMAGEPATH = ..\..\tools\MakeImage\bin\Debug\programs
DEMO = demo
BIN = bin

.PHONY : all

SHELL_OBJS =$(TARGET)\cat.exe	\
			$(TARGET)\cp.exe	\
			$(TARGET)\ls.exe	\
			$(TARGET)\mkdir.exe	\
			$(TARGET)\rm.exe	\
			$(TARGET)\perf.exe	\
			$(TARGET)\sig.exe \
			$(TARGET)\copyfile.exe	\
			$(TARGET)\shutdown.exe	\
			$(TARGET)\test.exe	\
			$(TARGET)\forks.exe	\
			$(TARGET)\trace.exe	\
			$(TARGET)\echo.exe	\
			$(TARGET)\date.exe \
			$(TARGET)\newsig.exe \
			$(TARGET)\sigTest.exe \
			$(TARGET)\stack.exe	\
			$(TARGET)\helloworld.exe	
			
#$(TARGET)\performance.exe
			
all	:	$(SHELL_OBJS)

	
$(TARGET)\cat.exe :	cat.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\cat.exe $(MAKEIMAGEPATH)\$(BIN)\cat.exe
	
$(TARGET)\cp.exe :	cp.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\cp.exe $(MAKEIMAGEPATH)\$(BIN)\cp.exe
	
$(TARGET)\ls.exe :	ls.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\ls.exe $(MAKEIMAGEPATH)\$(BIN)\ls.exe
	
$(TARGET)\mkdir.exe :	mkdir.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\mkdir.exe $(MAKEIMAGEPATH)\$(BIN)\mkdir.exe
	
$(TARGET)\perf.exe :	performance.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\perf.exe $(MAKEIMAGEPATH)\$(BIN)\perf.exe
	
$(TARGET)\rm.exe :	rm.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\rm.exe $(MAKEIMAGEPATH)\$(BIN)\rm.exe
	
$(TARGET)\copyfile.exe :	copyfile.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\copyfile.exe $(MAKEIMAGEPATH)\$(BIN)\cpfile.exe
	
$(TARGET)\shutdown.exe :	shutdown.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\shutdown.exe $(MAKEIMAGEPATH)\$(BIN)\shutdown.exe

$(TARGET)\test.exe :	test.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\test.exe $(MAKEIMAGEPATH)\$(BIN)\test.exe
	
$(TARGET)\forks.exe :		forks.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\forks.exe $(MAKEIMAGEPATH)\$(BIN)\forks.exe
	
$(TARGET)\sig.exe :		sig.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\sig.exe $(MAKEIMAGEPATH)\$(BIN)\sig.exe

$(TARGET)\sigTest.exe :		sigTest.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\sigTest.exe $(MAKEIMAGEPATH)\$(BIN)\sigTest.exe
	
$(TARGET)\trace.exe :		trace.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\trace.exe $(MAKEIMAGEPATH)\$(BIN)\trace.exe
	
$(TARGET)\echo.exe :		echo.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\echo.exe $(MAKEIMAGEPATH)\$(BIN)\echo.exe
	
$(TARGET)\date.exe :		date.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\date.exe $(MAKEIMAGEPATH)\$(BIN)\date.exe
	
$(TARGET)\newsig.exe :		newsig.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\newsig.exe $(MAKEIMAGEPATH)\$(BIN)\newsig.exe
	
$(TARGET)\stack.exe :		stack.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\stack.exe $(MAKEIMAGEPATH)\$(BIN)\stack.exe
	
$(TARGET)\helloworld.exe :		helloworld.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)"  $< -e _main1 $(V6++LIB) -o $@
	copy $(TARGET)\helloworld.exe $(MAKEIMAGEPATH)\$(BIN)\helloworld.exe
	
clean:
	del $(TARGET)\*.o
	del $(TARGET)\*.exe
	del $(TARGET)\*.asm
	