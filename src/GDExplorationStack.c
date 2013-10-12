//
//  GDExplorationStack.c
//  GraphDecomposition
//
 

#include "GDExplorationStack.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#pragma mark - Item

GDExplorationStackItem GDExplorationStackCreateItem(unsigned int level, GDNodeID node) {
  
  GDExplorationStackItem item;
  item.level = level;
  item.node = node;
  return item;
  
}


#pragma mark - Lifecycle

GDExplorationStackRef GDExplorationStackCreateWithCapacity(unsigned int initialCapcity) {
  
  assert(initialCapcity > 0);
  
  GDExplorationStackRef stack = malloc(sizeof(GDExplorationStack));
  
  GDExplorationStackItem * values = malloc(sizeof(GDExplorationStackItem) * initialCapcity);
  stack->values = values;
  stack->capacity = initialCapcity;
  stack->count = 0;
  
  return stack;
  
}

void GDExplorationStackRelease(GDExplorationStackRef stack) {
  
  assert(stack != NULL);
  
  free(stack->values);
  stack->values = NULL;
  free(stack);
  
}

void GDExplorationStackExtendCapacity(GDExplorationStackRef stack) {
  
  unsigned int newCapacity = stack->capacity * 2;
  stack->values = realloc(stack->values, sizeof(GDExplorationStackItem) * newCapacity);
  stack->capacity = newCapacity;
  
}


#pragma mark - Operations

void GDExplorationStackPush(GDExplorationStackRef stack, GDExplorationStackItem item) {
  
  assert(stack != NULL);
  
  if ( stack->count == stack->capacity ) {
    GDExplorationStackExtendCapacity(stack);
  }
  
  stack->values[stack->count] = item;
  stack->count = stack->count + 1;
  
}

GDExplorationStackItem GDExplorationStackPop(GDExplorationStackRef stack) {
  
  assert(stack != NULL);
  assert(stack->count > 0);
  
  GDExplorationStackItem retVal = GDExplorationStackTop(stack);
  stack->count = stack->count - 1;
  return retVal;
  
}

GDExplorationStackItem GDExplorationStackTop(GDExplorationStackRef stack) {
  
  assert(stack != NULL);
  assert(stack->count > 0);
  
  return stack->values[stack->count - 1];
  
}


#pragma mark - Serialization

GDExplorationStackRef GDExplorationStackhCreateFromData(void * data, unsigned long int length) {
  
  assert("Not implemented");
  // TODO
  
  return NULL;
  
}

void GDExplorationStackGetData(GDExplorationStackRef stack, void ** data, unsigned long int * length) {
  
  assert("Not implemented");
  // TODOstack
  
}


#pragma mark - Initialization & Split

void GDExplorationStackAddAllNodes(GDExplorationStackRef stack, GDGraphRef graph) {
  
  for ( GDNodeID node = 0; node < graph->nodesCount; node++ ) {
    GDExplorationStackItem state = GDExplorationStackCreateItem(0, node);
    GDExplorationStackPush(stack, state);
  }
  
}

GDExplorationStackRef GDExplorationStackSplit(GDExplorationStackRef stack) {
  
  assert("Not implemented");
  // TODO
  
  return NULL;
  
}



