//
//  GDExplorationStack.c
//  GraphDecomposition
//
 

#include "GDExplorationStack.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "GDDataWriter.h"
#include "GDDataReader.h"

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

GDExplorationStackRef GDExplorationStackhCreateFromData(char * bytes, unsigned long int length) {
  
  GDExplorationStackRef stack = malloc(sizeof(GDExplorationStack));
  
  GDDataReaderRef reader = GDDataReaderCreateWithCapacity(bytes, length);
  stack->count = GDDataReaderReadUnsignedInt(reader);
  stack->capacity = GDDataReaderReadUnsignedInt(reader);
  
  GDExplorationStackItem * values = malloc(sizeof(GDExplorationStackItem) * stack->capacity);
  stack->values = values;

  for ( unsigned int itemIdx = 0; itemIdx < stack->count; itemIdx++ ) {
    stack->values[itemIdx].level = GDDataReaderReadUnsignedInt(reader);
    stack->values[itemIdx].node = GDDataReaderReadUnsignedInt(reader);
  }
  
  GDDataReaderRelease(reader);
  
  return stack;
  
}

void GDExplorationStackGetData(GDExplorationStackRef stack, char ** bytes, unsigned long int * length) {
  
  assert(bytes != NULL);
  assert(length != NULL);
  
  size_t size = sizeof(unsigned int) * (1 + 2 * stack->count);
  
  GDDataWriterRef writer = GDDataWriterCreateWithCapacity(size);
  
  GDDataWriterWriteUnsignedInt(writer, stack->count);
  
  for ( unsigned int itemIdx = 0; itemIdx < stack->count; itemIdx++ ) {
    GDDataWriterWriteUnsignedInt(writer, stack->values[itemIdx].level);
    GDDataWriterWriteUnsignedInt(writer, stack->values[itemIdx].node);
  }
  
  *bytes = writer->bytes;
  *length = writer->lenght;
  
  GDDataWriterRelease(writer);
  
}


#pragma mark - Initialization & Split

void GDExplorationStackAddAllNodes(GDExplorationStackRef stack, GDGraphRef graph) {
  
  for ( GDNodeID node = 0; node < graph->nodesCount; node++ ) {
    GDExplorationStackItem state = GDExplorationStackCreateItem(0, node);
    GDExplorationStackPush(stack, state);
  }
  
}

GDExplorationStackRef GDExplorationStackSplit(GDExplorationStackRef stack) {
  
  // TODO
  
  return NULL;
  
}


#pragma mark - Helpers

GDBool GDExplorationStackEqual(GDExplorationStackRef stack1, GDExplorationStackRef stack2) {
  
  assert(stack1 != NULL);
  assert(stack2 != NULL);
  
  if ( stack1->count != stack2->count ) {
    return NO;
  }
  
  for ( unsigned int itemIdx = 0; itemIdx < stack1->count; itemIdx++ ) {
    GDExplorationStackItem item1 = stack1->values[itemIdx];
    GDExplorationStackItem item2 = stack1->values[itemIdx];
    if ( item1.level != item2.level || item1.node != item2.node ) {
      return NO;
    }
  }
  
  return YES;
  
}

