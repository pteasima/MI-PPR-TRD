//
//  GDExplorationStack.h
//  GraphDecomposition
//
 


#ifndef GraphDecomposition_GDExplorationStack_h
#define GraphDecomposition_GDExplorationStack_h

#include "GDDefines.h"
#include "GDGraph.h"

typedef struct GDExplorationStackItem {
  
  unsigned int level;
  GDNodeID node;
  
} GDExplorationStackItem;

GDExplorationStackItem GDExplorationStackCreateItem(unsigned int level, GDNodeID node);


typedef struct GDExplorationStack {
  
  GDExplorationStackItem * values;
  unsigned int capacity;
  unsigned int count;
  
} GDExplorationStack;


typedef GDExplorationStack * GDExplorationStackRef;

GDExplorationStackRef GDExplorationStackCreateWithCapacity(unsigned int initialCapcity);

GDExplorationStackRef GDExplorationStackhCreateFromData(void * data, unsigned long int length);
  
void GDExplorationStackRelease(GDExplorationStackRef stack);

void GDExplorationStackPush(GDExplorationStackRef stack, GDExplorationStackItem item);

GDExplorationStackItem GDExplorationStackPop(GDExplorationStackRef stack);

GDExplorationStackItem GDExplorationStackTop(GDExplorationStackRef stack);

void GDExplorationStackAddAllNodes(GDExplorationStackRef stack, GDGraphRef graph);

/**
 * Use this to serialize exploration stack
 */
void GDExplorationStackGetData(GDExplorationStackRef stack, void ** data, unsigned long int * length);


/**
 * Splits exploration (remove half of states from stack)
 * @returns Second half of splited stack
 */
GDExplorationStackRef GDExplorationStackSplit(GDExplorationStackRef stack);


#endif
