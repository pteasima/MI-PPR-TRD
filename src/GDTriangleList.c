//
//  GDTriangleList.c
//  GraphDecomposition
//
 
/**
 * Represents list of nodes and each three following nodes makes triangle (exception is last three 'not finished' triangle nodes)
 */

#include "GDTriangleList.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "GDDataWriter.h"
#include "GDDataReader.h"


void setEdgeUsage(GDTriangleListRef triangleList, GDNodeID node1, GDNodeID node2, GDBool used);

void setLastTriangleEdgeUsage(GDTriangleListRef triangleList, GDNodeID node1, GDNodeID node2, GDBool used);


#pragma mark - Lifecycle

GDTriangleListRef GDTriangleListCreateWithCapacity(unsigned int initialCapcity, unsigned int totalNodesCount) {
  
  assert(initialCapcity > 0);
  
  GDTriangleListRef triangleList = malloc(sizeof(GDTriangleList));
  
  triangleList->values = malloc(sizeof(GDNodeID) * initialCapcity);
  triangleList->capacity = initialCapcity;
  triangleList->count = 0;
  triangleList->edgesUsage = GDMatrixCreate(totalNodesCount, totalNodesCount);
  triangleList->lastTriangleEdgesUsage = GDMatrixCreate(totalNodesCount, totalNodesCount);
  
  return triangleList;
  
}

GDTriangleListRef GDTriangleListCopy(GDTriangleListRef triangleList) {
  
  assert(triangleList != NULL);
  
  GDTriangleListRef copy = malloc(sizeof(GDTriangleList));
  
  GDNodeID * valuesCopy = malloc(sizeof(GDNodeID) * triangleList->capacity);
  memcpy(valuesCopy, triangleList->values, sizeof(GDNodeID) * triangleList->capacity);
  copy->values = valuesCopy;
  copy->capacity = triangleList->capacity;
  copy->count = triangleList->count;
  copy->edgesUsage = NULL;
  copy->lastTriangleEdgesUsage = NULL;
  
  return copy;
  
}

void GDTriangleListRelease(GDTriangleListRef triangleList) {
  
  assert(triangleList != NULL);
  
  GDMatrixRelease(triangleList->edgesUsage);
  GDMatrixRelease(triangleList->lastTriangleEdgesUsage);
  free(triangleList->values);
  free(triangleList);
  
}

void GDTriangleListExtendCapacity(GDTriangleListRef triangleList) {
  
  unsigned int newCapacity = triangleList->capacity * 2;
  triangleList->values = realloc(triangleList->values, sizeof(GDNodeID) * newCapacity);
  triangleList->capacity = newCapacity;
  
}


#pragma mark - Stack

void GDTriangleListPush(GDTriangleListRef triangleList, GDNodeID value) {
  
  assert(triangleList != NULL);
  
  int count = GDTriangleListGetNotFinishedNodesCount(triangleList);
  if ( count == 0 ) {
    if ( triangleList->count >= 3 ) {
      setEdgeUsage(triangleList, triangleList->values[triangleList->count - 1], triangleList->values[triangleList->count - 2], YES);
      setEdgeUsage(triangleList, triangleList->values[triangleList->count - 1], triangleList->values[triangleList->count - 3], YES);
      setEdgeUsage(triangleList, triangleList->values[triangleList->count - 2], triangleList->values[triangleList->count - 3], YES);
      setLastTriangleEdgeUsage(triangleList, triangleList->values[triangleList->count - 1], triangleList->values[triangleList->count - 2], NO);
      setLastTriangleEdgeUsage(triangleList, triangleList->values[triangleList->count - 1], triangleList->values[triangleList->count - 3], NO);
      setLastTriangleEdgeUsage(triangleList, triangleList->values[triangleList->count - 2], triangleList->values[triangleList->count - 3], NO);
    }
  } else if ( count == 1 ) {
    setLastTriangleEdgeUsage(triangleList, triangleList->values[triangleList->count - 1], value, YES);
  } else if ( count == 2 ) {
    setLastTriangleEdgeUsage(triangleList, triangleList->values[triangleList->count - 1], value, YES);
    setLastTriangleEdgeUsage(triangleList, triangleList->values[triangleList->count - 2], value, YES);
  }
  
  if ( triangleList->count == triangleList->capacity ) {
    GDTriangleListExtendCapacity(triangleList);
  }
  
  triangleList->values[triangleList->count] = value;
  triangleList->count = triangleList->count + 1;
  
}

GDNodeID GDTriangleListPop(GDTriangleListRef triangleList) {
  
  assert(triangleList != NULL);
  assert(triangleList->count > 0);
  
  int count = GDTriangleListGetNotFinishedNodesCount(triangleList);
  if ( count == 0 ) {
    setLastTriangleEdgeUsage(triangleList, triangleList->values[triangleList->count - 1], triangleList->values[triangleList->count - 2], NO);
    setLastTriangleEdgeUsage(triangleList, triangleList->values[triangleList->count - 1], triangleList->values[triangleList->count - 3], NO);
  } else if ( count == 1 ) {
    if ( triangleList->count >= 4 ) {
      setEdgeUsage(triangleList, triangleList->values[triangleList->count - 1 - 1], triangleList->values[triangleList->count - 1 - 2], NO);
      setEdgeUsage(triangleList, triangleList->values[triangleList->count - 1 - 1], triangleList->values[triangleList->count - 1 - 3], NO);
      setEdgeUsage(triangleList, triangleList->values[triangleList->count - 1 - 2], triangleList->values[triangleList->count - 1 - 3], NO);
      
      setLastTriangleEdgeUsage(triangleList, triangleList->values[triangleList->count - 1 - 1], triangleList->values[triangleList->count - 1 - 2], YES);
      setLastTriangleEdgeUsage(triangleList, triangleList->values[triangleList->count - 1 - 1], triangleList->values[triangleList->count - 1 - 3], YES);
      setLastTriangleEdgeUsage(triangleList, triangleList->values[triangleList->count - 1 - 2], triangleList->values[triangleList->count - 1 - 3], YES);
    }
  } else if ( count == 2 ) {
    setLastTriangleEdgeUsage(triangleList, triangleList->values[triangleList->count - 1], triangleList->values[triangleList->count - 2], NO);
  }
  
  GDNodeID retVal = GDTriangleListTop(triangleList);
  triangleList->count = triangleList->count - 1;
  return retVal;
  
}

GDNodeID GDTriangleListTop(GDTriangleListRef triangleList) {
  
  assert(triangleList != NULL);
  assert(triangleList->count > 0);
  
  return triangleList->values[triangleList->count - 1];
  
}


#pragma mark - General

GDBool GDTriangleListIsLastTriangleClosed(GDTriangleListRef triangleList, GDGraphRef graph) {
  
  if ( triangleList->count % 3 == 0 ) {
    GDNodeID thirdNodeValue = triangleList->values[triangleList->count - 1];
    GDNodeID firstNodeValue = triangleList->values[triangleList->count - 3];
    if ( GDGraphHasConnection(graph, firstNodeValue, thirdNodeValue) &&
        !GDTriangleListContainsEdge(triangleList, firstNodeValue, thirdNodeValue, YES) ) {
      return YES;
    }
  }
  return NO;
  
}

int GDTriangleListGetNumberOfClosedTriangles(GDTriangleListRef triangleList) {
  
  return triangleList->count / 3;
  
}

int GDTriangleListGetNotFinishedNodesCount(GDTriangleListRef triangleList) {
  
  return triangleList->count - 3 * (triangleList->count / 3);
  
}


#pragma mark - Edge Usage

void setEdgeUsage(GDTriangleListRef triangleList, GDNodeID node1, GDNodeID node2, GDBool used) {
  
  triangleList->edgesUsage->rows[node1][node2] = used;
  triangleList->edgesUsage->rows[node2][node1] = used;
  
}

void setLastTriangleEdgeUsage(GDTriangleListRef triangleList, GDNodeID node1, GDNodeID node2, GDBool used) {
  
  triangleList->lastTriangleEdgesUsage->rows[node1][node2] = used;
  triangleList->lastTriangleEdgesUsage->rows[node2][node1] = used;
  
}

GDBool equalsEdge(GDNodeID firstEdgeNode1, GDNodeID firstEdgeNode2, GDNodeID secondEdgeNode1, GDNodeID secondEdgeNode2) {
  
  if ( firstEdgeNode1 == secondEdgeNode1 && firstEdgeNode2 == secondEdgeNode2 ) {
    return YES;
  }
  
  if ( firstEdgeNode1 == secondEdgeNode2 && firstEdgeNode2 == secondEdgeNode1 ) {
    return YES;
  }
  
  return NO;
  
}

GDBool GDTriangleListContainsEdge(GDTriangleListRef triangleList, GDNodeID node1, GDNodeID node2, GDBool withoutLastTriangle) {
  
  if ( !withoutLastTriangle ) {
    return triangleList->edgesUsage->rows[node1][node2] || triangleList->lastTriangleEdgesUsage->rows[node1][node2];
  } else {
    return triangleList->edgesUsage->rows[node1][node2];
  }
  
}



#pragma mark - Debugging

void GDTriangleListPrint(GDTriangleListRef triangleList) {
  
  assert(triangleList != NULL);
  
  printf("\n");
  for ( unsigned int valueIdx = 0; valueIdx < triangleList->count; valueIdx++ ) {
    printf("%d \t", triangleList->values[valueIdx]);
  }
  printf("\n");
  
}


#pragma mark - Serialization

GDTriangleListRef GDTriangleListCreateFromData(char * bytes, unsigned long int length) {

  GDTriangleListRef triangleList = malloc(sizeof(GDTriangleList));
  
  GDDataReaderRef reader = GDDataReaderCreateWithCapacity(bytes, length);
  
  unsigned int count = GDDataReaderReadUnsignedInt(reader);
  unsigned int totalNodesCount = GDDataReaderReadUnsignedInt(reader);
  triangleList->capacity = count;
  triangleList->count = 0;
  triangleList->edgesUsage = GDMatrixCreate(totalNodesCount, totalNodesCount);
  triangleList->lastTriangleEdgesUsage = GDMatrixCreate(totalNodesCount, totalNodesCount);
  
  GDNodeID * values = malloc(sizeof(GDNodeID) * triangleList->capacity);
  triangleList->values = values;
  
  for ( unsigned int itemIdx = 0; itemIdx < count; itemIdx++ ) {
    unsigned int node = GDDataReaderReadUnsignedInt(reader);
    GDTriangleListPush(triangleList, node);
  }
  
  GDDataReaderRelease(reader);
  
  return triangleList;
  
}

void GDTriangleListGetData(GDTriangleListRef triangleList, char ** bytes, unsigned long int * length) {

  assert(bytes != NULL);
  assert(length != NULL);
  
  size_t size = sizeof(unsigned int) * (2 + triangleList->count);
  
  GDDataWriterRef writer = GDDataWriterCreateWithCapacity(size);
  
  GDDataWriterWriteUnsignedInt(writer, triangleList->count);
  GDDataWriterWriteUnsignedInt(writer, triangleList->edgesUsage->rowsCount);
  
  for ( unsigned int itemIdx = 0; itemIdx < triangleList->count; itemIdx++ ) {
    GDDataWriterWriteUnsignedInt(writer, triangleList->values[itemIdx]);
  }
  
  *bytes = writer->bytes;
  *length = writer->lenght;
  
  GDDataWriterRelease(writer);
  
}


#pragma mark - Helpers

GDBool GDTriangleListEqual(GDTriangleListRef triangleList1, GDTriangleListRef triangleList2) {
  
  assert(triangleList1 != NULL);
  assert(triangleList2 != NULL);
  
  if ( triangleList1->count != triangleList2->count ) {
    return NO;
  }
  
  for ( unsigned int itemIdx = 0; itemIdx < triangleList1->count; itemIdx++ ) {
    if ( triangleList1->values[itemIdx] != triangleList2->values[itemIdx] ) {
      return NO;
    }
  }
  
  return YES;
  
}

