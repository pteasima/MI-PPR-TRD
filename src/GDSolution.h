//
//  GDSolution.h
//  GraphDecomposition
//
 

#include "GDGraph.h"
#include "GDTriangleList.h"
#include "GDMatrix.h"

#ifndef GraphDecomposition_GDSolution_h
#define GraphDecomposition_GDSolution_h

typedef struct GDSolution {

  GDTriangleListRef triangleList;
  GDGraphRef graph;
  
} GDSolution;

typedef GDSolution * GDSolutionRef;

/**
 * Creates copy of stack
 */
GDSolutionRef GDSolutionCreate(GDGraphRef graph, GDTriangleListRef triangleList);

void GDSolutionRelease(GDSolutionRef solution);

void GDSolutionPrint(GDSolutionRef solution);

GDBool GDSolutionCanExistsBetter(GDSolutionRef solution);

#endif
