//
//  GDExplorer.h
//  GraphDecomposition
//
 


#ifndef GraphDecomposition_GDExplorer_h
#define GraphDecomposition_GDExplorer_h

#include "GDGraph.h"
#include "GDSolution.h"
#include "GDExplorationStack.h"
#include "GDTriangleList.h"

typedef struct GDExplorer {
  
  GDGraphRef graph;
  GDTriangleListRef triangleList;
  GDExplorationStackRef explorationStack;
  GDSolutionRef bestSolution;
  
} GDExplorer;

typedef GDExplorer * GDExplorerRef;

GDExplorerRef GDExplorerCreate(GDGraphRef graph);

void GDExplorerRelease(GDExplorerRef explorer);

/**
 * @param stepsLimit If -1 then runs until has work on stack
 */
void GDExplorerRun(GDExplorerRef explorer, int stepsLimit);

GDExplorationStackRef GDExplorerGetExplorationStack(GDExplorerRef explorer);

void GDExplorerSetExplorationStack(GDExplorerRef explorer, GDExplorationStackRef explorationStack);

#endif
