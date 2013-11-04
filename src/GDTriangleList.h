//
//  GDTriangleList.h
//  GraphDecomposition
//
 
#ifndef GraphDecomposition_GDTriangleList_h
#define GraphDecomposition_GDTriangleList_h

#include "GDDefines.h"
#include "GDGraph.h"

typedef struct GDTriangleList {
  
  GDNodeID * values;
  GDMatrixRef edgesUsage; //without last triangle
  GDMatrixRef lastTriangleEdgesUsage;
  unsigned int capacity;
  unsigned int count;
  
} GDTriangleList;

typedef GDTriangleList * GDTriangleListRef;

GDTriangleListRef GDTriangleListCreateWithCapacity(unsigned int initialCapcity, unsigned int totalNodesCount);

/*
 * Edges usage is not implemented
 */
GDTriangleListRef GDTriangleListCopy(GDTriangleListRef triangleList);

void GDTriangleListRelease(GDTriangleListRef triangleList);

void GDTriangleListPush(GDTriangleListRef triangleList, GDNodeID value);

GDNodeID GDTriangleListPop(GDTriangleListRef triangleList);

void GDTriangleListPopMultiple(GDTriangleListRef triangleList, int count);

GDNodeID GDTriangleListTop(GDTriangleListRef triangleList);

void GDTriangleListPrint(GDTriangleListRef triangleList);

GDBool GDTriangleListContainsEdge(GDTriangleListRef triangleList, GDNodeID node1, GDNodeID node2, GDBool withoutLastTriangle);

GDBool GDTriangleListIsLastTriangleClosed(GDTriangleListRef triangleList, GDGraphRef graph);

int GDTriangleListGetNumberOfClosedTriangles(GDTriangleListRef triangleList);

int GDTriangleListGetNotFinishedNodesCount(GDTriangleListRef triangleList);

GDTriangleListRef GDTriangleListCreateFromData(char * bytes, unsigned long int length);

void GDTriangleListGetData(GDTriangleListRef triangleList, char ** bytes, unsigned long int * length);

GDBool GDTriangleListEqual(GDTriangleListRef triangleList1, GDTriangleListRef triangleList2);

#endif
