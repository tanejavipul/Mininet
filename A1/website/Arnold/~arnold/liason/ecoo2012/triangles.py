def trianglesA(n):
	if n==0: 
		return
	trianglesA(n-1)
	print "x" * n

def trianglesV(n):
	if n==0: 
		return
	print "x" * n
	trianglesV(n-1)

for i in range(10):
	trianglesA(i)

for i in range(10):
	trianglesV(i)

	

	
