#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
    assert(elemSize > 0);
    v->elemSize = elemSize;
    v->logLength = 0;
    v->allocLength = initialAllocation;
    v->elems = malloc(initialAllocation * elemSize);
    v->freeFn = freeFn;
    assert(v->elems != NULL);
}

void VectorDispose(vector *v)
{
    assert(v->elems != NULL);
    free(v->elems);
}

int VectorLength(const vector *v)
{
  return v->logLength;
}

void *VectorNth(const vector *v, int position)
{
  return (char*)v->elems + (v->elemSize * position);
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
  assert(elemAddr != NULL);
  assert(position < v ->allocLength);
  memcpy((char*)v->elems + (v->elemSize * position), elemAddr, v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{}

void VectorAppend(vector *v, const void *elemAddr)
{}

void VectorDelete(vector *v, int position)
{}

void VectorSort(vector *v, VectorCompareFunction compare)
{}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{ return -1; }
