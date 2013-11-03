//
//  GDGraph.h
//  GraphDecomposition
//
//  Created by Adam Zdara on 29.09.13.
//  Copyright (c) 2013 Adam Zdara. All rights reserved.
//

#include "GDDefines.h"
#include "GDMatrix.h"

#ifndef GraphDecomposition_GDGraph_h
#define GraphDecomposition_GDGraph_h

typedef struct GDGraph {
  
  unsigned int nodesCount;
  unsigned int edgesCount;
  GDMatrixRef adjacencyMatrix;
  
} GDGraph;

typedef GDGraph * GDGraphRef;

GDGraphRef GDGraphCreateEmpty(unsigned int nodesCount);

GDGraphRef GDGraphCreateFromFile(const char * path);

GDGraphRef GDGraphCreateFromData(char * bytes, unsigned long int length);

GDGraphRef GDGraphCreateRandom(unsigned int nodesCount);

void GDGraphRelease(GDGraphRef graph);

void GDGraphAddConnection(GDGraphRef graph, GDNodeID node1, GDNodeID node2);

GDBool GDGraphHasConnection(GDGraphRef graph, GDNodeID node1, GDNodeID node2);

void GDGraphPrint(GDGraphRef graph);

void GDGraphGetData(GDGraphRef graph, char ** bytes, unsigned long int * length);

GDBool GDGraphEqual(GDGraphRef graph1, GDGraphRef graph2);

#endif
