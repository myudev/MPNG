GPP=g++
OUTFILE="../Output/MPNG.so"
COMPILE_FLAGS=-m32 -c -fPIC -O3 -w -fpermissive -DLINUX -lpthread -I../../../SDK/amx/ -I../lib/win32/libpng-1.2.51 -I../lib/win32/pngpp-0.2.5

compile:
	$(GPP) $(COMPILE_FLAGS) ../../../SDK/*.cpp	
	
	$(GPP) $(COMPILE_FLAGS) ../lib/win32/libpng-1.2.51/*.c	
	#$(GPP) $(COMPILE_FLAGS) ../lib/win32/pngpp-0.2.5/*.c
	
	$(GPP) $(COMPILE_FLAGS) *.cpp	
	$(GPP) -m32 -O2 -fshort-wchar -shared -o $(OUTFILE) *.o