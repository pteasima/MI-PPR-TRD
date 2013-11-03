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

GDExplorationStackRef GDExplorationStackhCreateFromData(char * bytes, unsigned long int length);
  
void GDExplorationStackRelease(GDExplorationStackRef stack);

void GDExplorationStackPush(GDExplorationStackRef stack, GDExplorationStackItem item);

GDExplorationStackItem GDExplorationStackPop(GDExplorationStackRef stack);

GDExplorationStackItem GDExplorationStackTop(GDExplorationStackRef stack);

void GDExplorationStackAddAllNodes(GDExplorationStackRef stack, GDGraphRef graph);

GDBool GDExplorationStackEqual(GDExplorationStackRef stack1, GDExplorationStackRef stack2);

/**
 * Use this to serialize exploration stack
 */
void GDExplorationStackGetData(GDExplorationStackRef stack, char ** bytes, unsigned long int * length);


/**
 * Splits exploration (remove half of states from stack)
 * @returns Second half of splited stack
 */
GDExplorationStackRef GDExplorationStackSplit(GDExplorationStackRef stack);


#endif
