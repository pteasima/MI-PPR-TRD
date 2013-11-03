//
//  GDGraphSerializationTests.c
//  GraphDecomposition
//

#include <stdio.h>
#include <stdlib.h>
#include "GDGraph.h"


GDBool testSmallGraph() {
  
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

GDBool testEmptyGraph() {
  
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

void GDGraphSerializationTestsRun() {
  
  printf("\nGRAPH SERIALIZATION TESTS:\n");
  
  if ( testSmallGraph() == YES ) {
    printf("Test 1.....OK\n");
  } else {
    printf("Test 1.....failed\n");
  }
  
  if ( testEmptyGraph() == YES ) {
    printf("Test 2.....OK\n");
  } else {
    printf("Test 2.....failed\n");
  }
  
}


