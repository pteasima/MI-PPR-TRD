//
//  GDExplorerDataDistributionTests.c
//  GraphDecomposition
//

#include <stdio.h>
#include <stdlib.h>
#include "GDDefines.h"
#include "GDExplorationStack.h"
#include "GDGraph.h"
#include "GDTriangleList.h"

#pragma mark - Graph

GDBool testGraphSerializationAny() {
  
  GDGraphRef graph = GDGraphCreateEmpty(6);
  GDGraphAddConnection(graph, 0, 1);
  GDGraphAddConnection(graph, 1, 2);
  GDGraphAddConnection(graph, 2, 0);
  GDGraphAddConnection(graph, 3, 0);
  GDGraphAddConnection(graph, 3, 2);
  GDGraphAddConnection(graph, 4, 1);
  GDGraphAddConnection(graph, 4, 2);
  GDGraphAddConnection(graph, 1, 5);
  GDGraphAddConnection(graph, 4, 5);
  GDGraphAddConnection(graph, 3, 5);
  GDGraphAddConnection(graph, 0, 5);
  
  unsigned long lenght;
  char * data;
  GDGraphGetData(graph, &data, &lenght);
  GDGraphRef graphCopy = GDGraphCreateFromData(data, lenght);
  free(data);
  GDBool equals = GDGraphEqual(graphCopy, graph);
  
  GDGraphRelease(graphCopy);
  GDGraphRelease(graph);
  
  return equals;
  
}

GDBool testGraphSerializationEmpty() {
  
  GDGraphRef graph = GDGraphCreateEmpty(0);
  
  unsigned long lenght;
  char * data;
  GDGraphGetData(graph, &data, &lenght);
  GDGraphRef graphCopy = GDGraphCreateFromData(data, lenght);
  free(data);
  
  GDBool equals = GDGraphEqual(graphCopy, graph);
  
  GDGraphRelease(graphCopy);
  GDGraphRelease(graph);
  
  return equals;
  
}


#pragma mark - Stack

GDBool testStackSerializationAny() {
  
  GDExplorationStackRef stack = GDExplorationStackCreateWithCapacity(100);
  GDExplorationStackPush(stack, (GDExplorationStackItem){6, 1});
  GDExplorationStackPush(stack, (GDExplorationStackItem){5, 2});
  GDExplorationStackPush(stack, (GDExplorationStackItem){4, 3});
  GDExplorationStackPush(stack, (GDExplorationStackItem){3, 4});
  GDExplorationStackPush(stack, (GDExplorationStackItem){2, 5});
  GDExplorationStackPush(stack, (GDExplorationStackItem){1, 6});
  GDExplorationStackPush(stack, (GDExplorationStackItem){111122, 1234567});
  
  unsigned long lenght;
  char * bytes;
  GDExplorationStackGetData(stack, &bytes, &lenght);
  GDExplorationStackRef stackCopy = GDExplorationStackhCreateFromData(bytes, lenght);
  free(bytes);
  
  GDBool equals = GDExplorationStackEqual(stackCopy, stack);
  
  GDExplorationStackRelease(stackCopy);
  GDExplorationStackRelease(stack);
  
  return equals;
  
}

GDBool testStackSerializationEmpty() {
  
  GDExplorationStackRef stack = GDExplorationStackCreateWithCapacity(100);
  
  unsigned long lenght;
  char * bytes;
  GDExplorationStackGetData(stack, &bytes, &lenght);
  GDExplorationStackRef stackCopy = GDExplorationStackhCreateFromData(bytes, lenght);
  free(bytes);
  
  GDBool equals = GDExplorationStackEqual(stackCopy, stack);
  
  GDExplorationStackRelease(stackCopy);
  GDExplorationStackRelease(stack);
  
  return equals;
  
}

GDBool testStackSplit() {
  
  GDExplorationStackRef stack = GDExplorationStackCreateWithCapacity(100);
  GDExplorationStackPush(stack, (GDExplorationStackItem){1, 1});
  GDExplorationStackPush(stack, (GDExplorationStackItem){1, 2});
  GDExplorationStackPush(stack, (GDExplorationStackItem){1, 3});
  GDExplorationStackPush(stack, (GDExplorationStackItem){3, 4});
  GDExplorationStackPush(stack, (GDExplorationStackItem){3, 5});
  GDExplorationStackPush(stack, (GDExplorationStackItem){4, 6});
  GDExplorationStackPush(stack, (GDExplorationStackItem){100, 1});
  
  unsigned countBeforeSplit = stack->count;
  
  GDExplorationStackRef stackSecondHalf = GDExplorationStackSplit(stack);
  
  GDBool result = stack->count + stackSecondHalf->count == countBeforeSplit;

  GDExplorationStackRelease(stack);
  GDExplorationStackRelease(stackSecondHalf);
  
  return result;
  
}


#pragma mark - Triangle List

GDBool testTriangleListSerializationAny() {
  
  GDTriangleListRef triangleList = GDTriangleListCreateWithCapacity(100, 6);
  GDTriangleListPush(triangleList, 1);
  GDTriangleListPush(triangleList, 2);
  GDTriangleListPush(triangleList, 3);
  GDTriangleListPush(triangleList, 4);
  GDTriangleListPush(triangleList, 5);

  unsigned long lenght;
  char * bytes;
  GDTriangleListGetData(triangleList, &bytes, &lenght);
  GDTriangleListRef triangleListCopy = GDTriangleListCreateFromData(bytes, lenght);
 
  GDBool equals = GDTriangleListEqual(triangleListCopy, triangleList);
  
  GDTriangleListRelease(triangleListCopy);
  GDTriangleListRelease(triangleList);
  free(bytes);
  
  return equals;
  
}

GDBool testTriangleListSerializationEmpty() {
  
  GDTriangleListRef triangleList = GDTriangleListCreateWithCapacity(100, 10);
  
  unsigned long lenght;
  char * bytes;
  GDTriangleListGetData(triangleList, &bytes, &lenght);
  GDTriangleListRef triangleListCopy = GDTriangleListCreateFromData(bytes, lenght);
  free(bytes);
  
  GDBool equals = GDTriangleListEqual(triangleListCopy, triangleList);
  
  GDTriangleListRelease(triangleList);
  GDTriangleListRelease(triangleListCopy);
  
  return equals;
  
}

GDBool testTriangleListSerializationEdges() {
  
  GDTriangleListRef triangleList = GDTriangleListCreateWithCapacity(100, 6);
  GDTriangleListPush(triangleList, 1);
  GDTriangleListPush(triangleList, 2);
  GDTriangleListPush(triangleList, 3);
  GDTriangleListPush(triangleList, 4);
  GDTriangleListPush(triangleList, 5);
  
  unsigned long lenght;
  char * bytes;
  GDTriangleListGetData(triangleList, &bytes, &lenght);
  GDTriangleListRef triangleListCopy = GDTriangleListCreateFromData(bytes, lenght);
  
  GDBool equals = GDMatrixEqual(triangleList->edgesUsage, triangleListCopy->edgesUsage) &&
  GDMatrixEqual(triangleList->lastTriangleEdgesUsage, triangleListCopy->lastTriangleEdgesUsage);
  
  GDTriangleListEqual(triangleListCopy, triangleList);
  
  GDTriangleListRelease(triangleListCopy);
  GDTriangleListRelease(triangleList);
  free(bytes);
  
  return equals;
  
}


#pragma mark - Exploration Work Distribution

GDBool testWorkMove() {
  
  return NO;
  
}

//TODO


#pragma mark - Main

void GDExplorerDataDistributionTestsRun() {
  
  printf("\nGRAPH TESTS:\n");
  
  if ( testGraphSerializationAny() == YES ) {
    printf("Serialization 1.....OK\n");
  } else {
    printf("Serialization 1.....failed\n");
  }
  
  if ( testGraphSerializationEmpty() == YES ) {
    printf("Serialization 2.....OK\n");
  } else {
    printf("Serialization 2.....failed\n");
  }
  
  printf("\nEXPLORATION STACK TESTS:\n");
  
  if ( testStackSerializationAny() == YES ) {
    printf("Serialization 1.....OK\n");
  } else {
    printf("Serialization 1.....failed\n");
  }
  
  if ( testStackSerializationEmpty() == YES ) {
    printf("Serialization 2.....OK\n");
  } else {
    printf("Serialization 2.....failed\n");
  }
  
  if ( testStackSplit() == YES ) {
    printf("Split.....OK\n");
  } else {
    printf("Split .....failed\n");
  }
  
  printf("\nTRIANGLE LIST TESTS:\n");
  
  if ( testTriangleListSerializationAny() == YES ) {
    printf("Serializaiton 1.....OK\n");
  } else {
    printf("Serializaiton 1 .....failed\n");
  }
  
  if ( testTriangleListSerializationEmpty() == YES ) {
    printf("Serializaiton 2.....OK\n");
  } else {
    printf("Serializaiton 2 .....failed\n");
  }
  
  if ( testTriangleListSerializationEdges() == YES ) {
    printf("Serializaiton 3.....OK\n");
  } else {
    printf("Serializaiton 3 .....failed\n");
  }
  
  printf("\nWORK DISTRIBUTION TESTS:\n");
  
  if ( testWorkMove() == YES ) {
    printf("Move 1.....OK\n");
  } else {
    printf("Move 1 .....failed\n");
  }

  
}


