def sum1(i):
	if(i<=0): return 0
	else: return i+sum(i-1)

def sum2(i,j):
	if(i<j): return 0
	else return sum(i,j-1)+j

def sum3(i,j):
	if(i<j): return 0
	else return i+sum(i+1,j)

def sum4(i,j):
	s=0
	if i>j: s=0
	elif i==j: s=i
	else: 
		mid=(i+j)/2
		s=sum(i,mid)+sum(mid+1,j)
	return s

print sum(3,3)
print sum(99,99)
print sum(1,2)
print sum(1,3)
print sum(1,4)
print sum(10,20)
print sum(0,9)
print sum(0,20)
