import pwnlib
import sys

pwnlib.term.init()

if len(sys.argv) < 2:
	print ("Usage: python3 loader.py <file.bef>")
	exit(0)

bef = open(sys.argv[1]).read()
lines = bef.split("\n")
h = len(lines)
w = max([len(x) for x in lines])

def pad(s, n):
    return s+(n-len(s))*" "

bef = "\n".join([pad(x, w) for x in lines])

proc = pwnlib.tubes.process.process(["./befunge",str(w),str(h)])
proc.send(bef)
proc.sendline("")
proc.interactive()
