#
# This MAKEFILE was automatically built with Falch.net DeveloperStudio, and
# any changes you make in this file will be overwritten the next time you
# build this project.
#
# To edit this file manually, turn off "Automatically rebuild makefile on each
# build" in the "Project Properties" dialog ("Project" -> "Properties" ->
# "Advanced" Tab)
#

#  
#  Target ALL
#  
all : ./Release/matreo.prc

#  
#  Target Remove temporary files
#  
temp : 
	cd ./Release/ && \
	rm -f clean ./Release/main.o ./Release/frmMain.o ./Release/device.o ./Release/graphics.o ./Release/anim.o ./Release/sysstuff.o ./Release/MathLib.o ./Release/frmOptions.o ./Release/matro2.ro ./Release/matreo

#  
#  Target Clean
#  
clean : 
	rm -f ./Release/main.o ./Release/frmMain.o ./Release/device.o ./Release/graphics.o ./Release/anim.o ./Release/sysstuff.o ./Release/MathLib.o ./Release/frmOptions.o ./Release/matro2.ro ./Release/matreo ./Release/matreo.prc

#  
#  Rule for building sourcefile main.c
#  
./Release/main.o : main.c matro2.h matro2_res.h
	@echo "Compiling main.c..." && \
	m68k-palmos-gcc -palmos4.0 -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/PalmDev/Sdk-4.0/include/Handspring  -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/PalmDev/MathLib/Src -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/bin/POSE -DERROR_CHECK_LEVEL=2 -O2 -o./Release/main.o -c main.c -MD
	@mv  main.d ./Release/main.d

#  
#  Rule for building sourcefile frmMain.c
#  
./Release/frmMain.o : frmMain.c matro2.h matro2_res.h
	@echo "Compiling frmMain.c..." && \
	m68k-palmos-gcc -palmos4.0 -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/PalmDev/Sdk-4.0/include/Handspring  -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/PalmDev/MathLib/Src -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/bin/POSE -DERROR_CHECK_LEVEL=2 -O2 -o./Release/frmMain.o -c frmMain.c -MD
	@mv  frmMain.d ./Release/frmMain.d

#  
#  Rule for building sourcefile device.c
#  
./Release/device.o : device.c matro2.h device.h
	@echo "Compiling device.c..." && \
	m68k-palmos-gcc -palmos4.0 -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/PalmDev/Sdk-4.0/include/Handspring  -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/PalmDev/MathLib/Src -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/bin/POSE -DERROR_CHECK_LEVEL=2 -O2 -o./Release/device.o -c device.c -MD
	@mv  device.d ./Release/device.d

#  
#  Rule for building sourcefile graphics.c
#  
./Release/graphics.o : graphics.c graphics.h
	@echo "Compiling graphics.c..." && \
	m68k-palmos-gcc -palmos4.0 -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/PalmDev/Sdk-4.0/include/Handspring  -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/PalmDev/MathLib/Src -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/bin/POSE -DERROR_CHECK_LEVEL=2 -O2 -o./Release/graphics.o -c graphics.c -MD
	@mv  graphics.d ./Release/graphics.d

#  
#  Rule for building sourcefile anim.c
#  
./Release/anim.o : anim.c matro2.h device.h graphics.h
	@echo "Compiling anim.c..." && \
	m68k-palmos-gcc -palmos4.0 -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/PalmDev/Sdk-4.0/include/Handspring  -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/PalmDev/MathLib/Src -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/bin/POSE -DERROR_CHECK_LEVEL=2 -O2 -o./Release/anim.o -c anim.c -MD
	@mv  anim.d ./Release/anim.d

#  
#  Rule for building sourcefile sysstuff.c
#  
./Release/sysstuff.o : sysstuff.c matro2.h matro2_res.h
	@echo "Compiling sysstuff.c..." && \
	m68k-palmos-gcc -palmos4.0 -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/PalmDev/Sdk-4.0/include/Handspring  -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/PalmDev/MathLib/Src -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/bin/POSE -DERROR_CHECK_LEVEL=2 -O2 -o./Release/sysstuff.o -c sysstuff.c -MD
	@mv  sysstuff.d ./Release/sysstuff.d

#  
#  Rule for building sourcefile MathLib.c
#  
./Release/MathLib.o : MathLib.c ../../../../../Documents\ and\ Settings/matro/My\ Documents/Falch/matro2/MathLib.h
	@echo "Compiling MathLib.c..." && \
	m68k-palmos-gcc -palmos4.0  -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/PalmDev/MathLib/Src -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/bin/POSE -DERROR_CHECK_LEVEL=2 -O2 -o./Release/MathLib.o -c MathLib.c -MD
	@mv  MathLib.d ./Release/MathLib.d

#  
#  Rule for building sourcefile frmOptions.c
#  
./Release/frmOptions.o : frmOptions.c sysstuff.h mathlib.h device.h graphics.h matro2.h matro2_res.h
	@echo "Compiling frmOptions.c..." && \
	m68k-palmos-gcc -palmos4.0 -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/PalmDev/Sdk-4.0/include/Handspring  -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/PalmDev/MathLib/Src -I/cygdrive/c/Program\ Files/Falch.net/DeveloperStudio\ 2.6/bin/POSE -DERROR_CHECK_LEVEL=2 -O2 -o./Release/frmOptions.o -c frmOptions.c -MD
	@mv  frmOptions.d ./Release/frmOptions.d

#  
#  Rule for building resources
#  
./Release/matro2.ro : matro2.rcp
	@pilrc -q -ro matro2.rcp ./Release/matro2.ro

#  
#  Rule for Linking the project
#  
./Release/matreo : ./Release/main.o ./Release/frmMain.o ./Release/device.o ./Release/graphics.o ./Release/anim.o ./Release/sysstuff.o ./Release/MathLib.o ./Release/frmOptions.o /cygdrive/c/DOCUME~1/matro/MYDOCU~1/Falch/matro2/matro2.fnp
	@echo "Linking..." && \
	m68k-palmos-gcc ./Release/main.o ./Release/frmMain.o ./Release/device.o ./Release/graphics.o ./Release/anim.o ./Release/sysstuff.o ./Release/MathLib.o ./Release/frmOptions.o -palmos4.0 -o "$@"

#  
#  Rule for building Palm OS Program File
#  
./Release/matreo.prc : ./Release/matro2.ro ./Release/matreo
	@echo "Building program file ./Release/matreo.prc..." && \
	build-prc  -t appl -o ./Release/matreo.prc -n "MaTreo" -c MTRO ./Release/matro2.ro ./Release/matreo
