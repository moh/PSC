# Designed for Ubuntu 18.04. 
# You might need to install C/C++ development tools by typing :
#    sudo apt-get install build-essential
# in a terminal.
# For more information on the configuration used, see http://www.ensta-bretagne.fr/lebars/Share/Ubuntu.txt .
# Use dos2unix *.txt to ensure line endings are correct for Linux in the configuration files.

PROGS = UxVSim

CC = gcc
CXX = g++

#CFLAGS += -g
CFLAGS += -O3
CFLAGS += -Wall -Wno-unknown-pragmas -Wextra
#CFLAGS += -Winline

#CFLAGS += -D _DEBUG -D _DEBUG_DISPLAY 
#CFLAGS += -D _DEBUG_MESSAGES 

# Depending on your OS (old Linux or macOS)...
#CFLAGS += -D USE_OLD_CHRONO

CXXFLAGS += $(CFLAGS) -fpermissive

# For Linux, if static needed...
#LDFLAGS += -static-libgcc -static-libstdc++ -static

# For MinGW
#LDFLAGS += -lpthread -lws2_32 -lm

# For macOS
#LDFLAGS += -lpthread -lm

# For Linux
LDFLAGS += -lpthread -lrt -lm

default: $(PROGS)

############################# OSUtils #############################

OSCore.o: OSCore.c OSCore.h
	$(CC) $(CFLAGS) -c $<

OSCriticalSection.o: OSCriticalSection.c OSCriticalSection.h OSThread.o
	$(CC) $(CFLAGS) -c $<

OSMisc.o: OSMisc.c OSMisc.h OSTime.o
	$(CC) $(CFLAGS) -c $<

OSNet.o: OSNet.c OSNet.h OSThread.o
	$(CC) $(CFLAGS) -c $<

OSThread.o: OSThread.c OSThread.h OSTime.o
	$(CC) $(CFLAGS) -c $<

OSTime.o: OSTime.c OSTime.h OSCore.o
	$(CC) $(CFLAGS) -c $<

############################# PROGS #############################

MTInterface.o: MTInterface.c
	$(CC) $(CFLAGS) -c $<

PololuInterface.o: PololuInterface.c
	$(CC) $(CFLAGS) -c $<

MESInterface.o: MESInterface.c
	$(CC) $(CFLAGS) -c $<

ProbeInterface.o: ProbeInterface.c
	$(CC) $(CFLAGS) -c $<

RazorAHRSInterface.o: RazorAHRSInterface.c
	$(CC) $(CFLAGS) -c $<

SBGInterface.o: SBGInterface.c
	$(CC) $(CFLAGS) -c $<

SSC32Interface.o: SSC32Interface.c
	$(CC) $(CFLAGS) -c $<

IM483IInterface.o: IM483IInterface.c
	$(CC) $(CFLAGS) -c $<

OntrakInterface.o: OntrakInterface.c
	$(CC) $(CFLAGS) -c $<

NMEAInterface.o: NMEAInterface.c
	$(CC) $(CFLAGS) -c $<

Globals.o: Globals.c
	$(CC) $(CFLAGS) -c $<

Config.o: Config.c
	$(CC) $(CFLAGS) -c $<

Main.o: Main.c MTInterface.h PololuInterface.h MESInterface.h ProbeInterface.h RazorAHRSInterface.h SBGInterface.h SSC32Interface.h IM483IInterface.h OntrakInterface.h NMEAInterface.h NMEAProtocol.h Config.o Globals.h OSThread.h OSNet.h OSComputerRS232Port.h OSMisc.h OSCriticalSection.h OSTime.h OSCore.h
	$(CC) $(CFLAGS) -c $<

UxVSim: Main.o MTInterface.o PololuInterface.o MESInterface.o ProbeInterface.o RazorAHRSInterface.o SBGInterface.o SSC32Interface.o IM483IInterface.o OntrakInterface.o NMEAInterface.o Globals.o OSThread.o OSNet.o OSMisc.o OSCriticalSection.o OSTime.o OSCore.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(PROGS) $(PROGS:%=%.elf) $(PROGS:%=%.exe) *.o *.obj core *.gch
