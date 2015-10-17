GPP=g++
OUTFILE="SuperSlot.so"
COMPILE_FLAGS=-c -fPIC -O3 -w -DLINUX -ISDK/amx/

all:
	$(GPP) $(COMPILE_FLAGS) SDK/*.cpp	
	$(GPP) $(COMPILE_FLAGS) *.cpp	
	$(GPP) -O2 -m32 -fshort-wchar -shared -o $(OUTFILE) *.o