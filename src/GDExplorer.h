//
//  GDExplorer.h
//  GraphDecomposition
//
 

/*
 
 Finds the best solution (max # of triangles in graph) using following steps:
 
 1) Expand to all nodes

 2) Get top state from exploration stack
 
 3) Add node to triangle list
 
 4) If last three nodes in triangle list makes triangle then save solution (if better then existing)
 
 5) Expand to following states (if maximum possible # of triangles that can be made using remaining edges is better than # of triangles in current best solution)
 
    a) If triangle is not finished then expand to neighbours
 
    b) else expand to all nodes that can be part of any triangle
 
 6) Go to step 2
 
 */

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
 * @param stepsLimit If -1 then runs until have items in exploration stack
 */
void GDExplorerRun(GDExplorerRef explorer, int stepsLimit, GDBool * canExistBetter);

void GDExplorerInitializeWork(GDExplorerRef explorer);

/**
 * @returns YES if exploration stack was split and bytes and lenght out parameters are valid
 */
GDBool GDExplorerGetWork(GDExplorerRef explorer, char ** bytes, unsigned long int * lenght);

void GDExplorerSetWork(GDExplorerRef explorer, char * bytes, unsigned long int lenght);



#endif
