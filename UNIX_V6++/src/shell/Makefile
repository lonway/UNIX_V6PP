#
# Makefile for Unix V6++ Shell
#

#命令定义
CC = gcc
LD = ld

#参数定义
CFLAGS = -W -O0 -g -nostdlib -nostdinc -nostartfiles -fno-builtin -fno-exceptions

#目录定义
TARGET = .\OBJS
V6++LIB = "..\lib\Lib_V6++.a"
INCLUDE = .
LIB_INCLUDE = ..\lib\include

MAKEIMAGEPATH = ..\..\tools\MakeImage\bin\Debug\programs

.PHONY : all

SHELL_OBJS =$(TARGET)\commandtree.o	\
			$(TARGET)\executecommand.o	\
			$(TARGET)\preexecute.o	\
			$(TARGET)\jobs.o	\
			$(TARGET)\luanchjob.o	\
			$(TARGET)\main.o
			
all	:	$(SHELL_OBJS)
	$(CC) $(CFLAGS) $(SHELL_OBJS) $(V6++LIB) -e _main1 -o $(TARGET)\Shell.exe
	copy $(TARGET)\Shell.exe $(MAKEIMAGEPATH)\Shell.exe
	
$(TARGET)\commandtree.o :	CommandTree.c $(INCLUDE)\CommandTree.h
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)" -c $< -o $@
	
$(TARGET)\executecommand.o :	ExecuteCommand.c $(INCLUDE)\ExecuteCommand.h
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)" -c $< -o $@
	
$(TARGET)\preexecute.o :	PreExecute.c $(INCLUDE)\PreExecute.h
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)" -c $< -o $@
	
$(TARGET)\jobs.o :	Jobs.c $(INCLUDE)\Jobs.h
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)" -c $< -o $@
	
$(TARGET)\luanchjob.o :	LuanchJob.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)" -c $< -o $@
	
$(TARGET)\main.o :	main.c
	$(CC) $(CFLAGS) -I"$(INCLUDE)" -I"$(LIB_INCLUDE)" -c $< -o $@

	
clean:
	del $(TARGET)\*.o
	del $(TARGET)\*.exe
	