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

void GDExplorationStackRelease(GDExplorationStackRef stack);

void GDExplorationStackPush(GDExplorationStackRef stack, GDExplorationStackItem item);

GDExplorationStackItem GDExplorationStackPop(GDExplorationStackRef stack);

GDExplorationStackItem GDExplorationStackTop(GDExplorationStackRef stack);

GDBool GDExplorationStackEqual(GDExplorationStackRef stack1, GDExplorationStackRef stack2);

unsigned int GDExplorationStackNumberOfNodesAtLevel(GDExplorationStackRef stack, unsigned int level);

GDExplorationStackRef GDExplorationStackSplit(GDExplorationStackRef stack);

GDExplorationStackRef GDExplorationStackhCreateFromData(char * bytes, unsigned long int length);

void GDExplorationStackGetData(GDExplorationStackRef stack, char ** bytes, unsigned long int * length);


#endif
