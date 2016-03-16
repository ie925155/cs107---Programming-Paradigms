#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
	assert(elemSize > 0);
	assert(numBuckets > 0);
	assert(hashfn != NULL);
	assert(comparefn != NULL);
  h->elems = malloc(sizeof(vector) * numBuckets);
  h->elemSize = elemSize;
  h->logLength = 0;
  h->allocLength = numBuckets;
  h->hashfn = hashfn;
  h->comparefn = comparefn;
  h->freefn = freefn;
}

void HashSetDispose(hashset *h)
{
	for(int i = 0 ; i < h->logLength ; i++){
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
	for(int i = 0 ; i < h->logLength ; i++){
		totalCount += VectorLength((vector*)(char*)h->elems + (i * sizeof(vector)));
	}
	return totalCount;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
	for(int i = 0 ; i < h->logLength ; i++){
		vector *v = (vector*)((char*)h->elems + (i * sizeof(vector)));
		for(int j = 0 ; j < VectorLength(v) ; j++){
			mapfn(VectorNth(v, j), auxData);
		}
	}
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
	assert(elemAddr != NULL);
	int idx = h->hashfn(elemAddr, h->allocLength);
	assert(idx >= 0 && idx < h->allocLength);
  vector *v = (vector*)((char*)h->elems + (idx * sizeof(vector)));
	int position = VectorSearch(v, elemAddr, h->comparefn, 0, false);
	(position != -1) ? VectorReplace(v, elemAddr, position) : VectorAppend(v, \
		elemAddr);
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{ return NULL; }
