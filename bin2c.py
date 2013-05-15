import sys
shellcode = file(sys.argv[1],"rb").read()
N=15
print "/*"
print " * "+ " ".join(sys.argv)
print " */"

print "unsigned char shellcode[] ="
for i in range(0,len(shellcode),N):
	print '"'+''.join(["\\x%02X"%ord(c) for c in shellcode[i:i+N]]) + '"'
