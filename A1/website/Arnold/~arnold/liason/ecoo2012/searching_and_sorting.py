def merge(L1,L2):
	''' return a list resulting from merging sorted lists L1 and L2 together '''
	L=[]
	i1=i2=0
	while i1<len(L1) and i2<len(L2):
		if L1[i1]<L2[i2]:
			L.append(L1[i1])
			i1=i1+1
		else:
			L.append(L2[i2])
			i2=i2+1
	if i1==len(L1):
		L.extend(L2[i2:])
	if i2==len(L2):
		L.extend(L1[i1:])

	return L

def test_merge(L1, L2):
	''' test the execution of merge on two sorted lists '''
	assert merge(L1, L2) == merge(L2, L1) 
	L=L1[:]
	L.extend(L2)
	assert merge(L1, L2) == sorted(L) 

test_merge([],[])
test_merge([1],[])
test_merge([1],[1])
test_merge([1],[2])
test_merge([1, 2, 4, 5],[0])
test_merge([1, 2, 4, 5],[3])
test_merge([1, 2, 4, 5],[6])
test_merge([0], [1, 2, 3, 4, 5])
test_merge([1, 2, 3, 4, 5],[1, 2, 3, 4, 5, 6, 7, 8])
test_merge([2,4,6,8,10,12], [1,3,5,7,9,11,13,15,17])

def merge_sort(L):
	''' return a copy of L whose elements are sorted in increasing order '''
	''' use the merge sort algorithm '''
	if len(L)<2: # charge 1
		return L
	else:
		m=len(L)/2 # 1
		return merge(merge_sort(L[:m]),merge_sort(L[m:]))

def bsearch(L,e,lower,upper):
	''' 
		Preconditions: 

		lower<=upper and are in 0,...,len(L)
		L is sorted in increasing order
		if e were to be in L it belongs somewhere in L[lower]...L[upper]
	
		Return: The index i, lower<=i<=upper, where e belongs in L
		that is, the least i such that e<=L[i]
	'''

	if upper-lower<1:
		return lower
	else:
		mid = (lower+upper)/2
		if e<=L[mid]:
			return bsearch(L,e,lower,mid)
		else:
			return bsearch(L,e,mid+1,upper)

def linear_search(L,e):
	''' Return: The index of where e belongs in sorted list L 
		that is, the least i such that e<=L[i]
	'''
	for i in range(len(L)):
		if e<=L[i]:
			return i
	return len(L)

L=[]
for i in range(10):
	L.append(3*i)

for i in range(-5,35):
	assert bsearch(L, i, 0, len(L))==linear_search(L,i)

for i in range(-5,35):
	print i, bsearch(L,i,0,len(L))


