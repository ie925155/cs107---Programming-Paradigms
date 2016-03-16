#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, \
		HashSetFreeFunction freefn)
{
	assert(elemSize > 0);
	assert(numBuckets > 0);
	assert(hashfn != NULL);
	assert(comparefn != NULL);
  h->elems = malloc(sizeof(vector) * numBuckets);
	for(int i = 0 ; i < numBuckets ; i++){
		vector vObj;
		VectorNew(&vObj, elemSize, NULL, 4);
		memcpy((char*)h->elems + (i * sizeof(vector)), &vObj, sizeof(vector));
	}
  h->elemSize = elemSize;
  //h->logLength = 0;
  h->allocLength = numBuckets;
  h->hashfn = hashfn;
  h->comparefn = comparefn;
  h->freefn = freefn;
}

void HashSetDispose(hashset *h)
{
	for(int i = 0 ; i < h->allocLength ; i++){
		vector *v = (vector*)((char*)h->elems + (i * sizeof(vector)));
		if(h->freefn != NULL){
			for(int j = 0 ; j < VectorLength(v) ; j++)
			  h->freefn(VectorNth(v, j));
		}
		VectorDispose(v);
	}
	free(h->elems);
}

int HashSetCount(const hashset *h)
{
	int totalCount = 0;
	for(int i = 0 ; i < h->allocLength ; i++)
		totalCount += VectorLength((vector*)(char*)h->elems + (i * sizeof(vector)));
	return totalCount;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
	for(int i = 0 ; i < h->allocLength ; i++){
		vector *v = (vector*)((char*)h->elems + (i * sizeof(vector)));
		for(int j = 0 ; j < VectorLength(v) ; j++)
			mapfn(VectorNth(v, j), auxData);
	}
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
	assert(elemAddr != NULL);
	int idx = h->hashfn(elemAddr, h->allocLength);
	assert(idx >= 0 && idx < h->allocLength);
  vector *v = (vector*)((char*)h->elems + (idx * sizeof(vector)));
	int position = VectorSearch(v, elemAddr, h->comparefn, 0, true);
	(position != -1) ? VectorReplace(v, elemAddr, position) : VectorAppend(v, \
		elemAddr);
	VectorSort(v, h->comparefn);
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{
	assert(elemAddr != NULL);
	int idx = h->hashfn(elemAddr, h->allocLength);
	assert(idx >= 0 && idx < h->allocLength);
  vector *v = (vector*)((char*)h->elems + (idx * sizeof(vector)));
	int position = VectorSearch(v, elemAddr, h->comparefn, 0, true);
	return (position != -1) ? VectorNth(v, position) : NULL;
}
