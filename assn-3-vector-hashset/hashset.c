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
  h->elemSize = sizeof(vector);
  h->logLength = 0;
  h->allocLength = numBuckets;
  h->hashfn = hashfn;
  h->comparefn = comparefn;
  h->freefn = freefn;
}

void HashSetDispose(hashset *h)
{
	for(int i = 0 ; i < h->logLength ; i++){
		vector *v = (vector*)((char*)h->elems + (i * h->elemSize));
		if(h->freefn != NULL){
			for(int j = 0 ; j < v->logLength ; j++)
			  h->freefn((char*)v->elems + (j * v->elemSize));
		}
		VectorDispose(v);
	}
	free(h->elems);
}

int HashSetCount(const hashset *h)
{ return 0; }

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{}

void HashSetEnter(hashset *h, const void *elemAddr)
{}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{ return NULL; }
