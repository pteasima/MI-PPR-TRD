//
//  GDAlgorithmTests.c
//  GraphDecomposition
//

#include <stdio.h>
#include <stdlib.h>
#include "GDGraph.h"
#include "GDExplorer.h"
#include "GDSolution.h"

int runWithGraph(GDGraphRef graph) {
  
  GDExplorerRef explorer = GDExplorerCreate(graph);
  
  GDExplorationStackRef stack = GDExplorationStackCreateWithCapacity(64);
  GDExplorationStackAddAllNodes(stack, explorer->graph);
  GDExplorerSetExplorationStack(explorer, stack);
  
  while ( YES ) {
    GDExplorerRun(explorer, -1, NULL);
    if (explorer->explorationStack->count == 0 ) {
      break;
    }
  }

  GDBool result = 0;
  if ( explorer->bestSolution != NULL ) {
    result = GDTriangleListGetNumberOfClosedTriangles(explorer->bestSolution->triangleList);
  }
  
  GDExplorerRelease(explorer);
  
  return result;
  
}

GDBool test1() {
  
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
  
  int count = runWithGraph(graph);
  GDGraphRelease(graph);
  
  return count == 3;
  
}

GDBool test2() {
  
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
  
  int count = runWithGraph(graph);
  GDGraphRelease(graph);
  
  return count == 2;
  
}

GDBool test3() {
  
  GDGraphRef graph = GDGraphCreateEmpty(5);
  GDGraphAddConnection(graph, 0, 1);
  GDGraphAddConnection(graph, 1, 2);
  GDGraphAddConnection(graph, 2, 0);
  GDGraphAddConnection(graph, 3, 0);
  GDGraphAddConnection(graph, 3, 2);
  GDGraphAddConnection(graph, 4, 1);
  GDGraphAddConnection(graph, 4, 2);
  
  int count = runWithGraph(graph);
  GDGraphRelease(graph);
  
  return count == 2;
  
}

GDBool test4() {
  
  GDGraphRef graph = GDGraphCreateEmpty(5);
  GDGraphAddConnection(graph, 0, 1);
  GDGraphAddConnection(graph, 1, 2);
  GDGraphAddConnection(graph, 3, 0);
  GDGraphAddConnection(graph, 3, 2);
  GDGraphAddConnection(graph, 4, 1);
  GDGraphAddConnection(graph, 4, 2);
  
  int count = runWithGraph(graph);
  GDGraphRelease(graph);
  
  return count == 1;
  
}

GDBool test5() {
  
  GDGraphRef graph = GDGraphCreateEmpty(4);
  GDGraphAddConnection(graph, 0, 1);
  GDGraphAddConnection(graph, 1, 2);
  GDGraphAddConnection(graph, 2, 0);
  GDGraphAddConnection(graph, 3, 0);
  GDGraphAddConnection(graph, 3, 2);
  
  int count = runWithGraph(graph);
  GDGraphRelease(graph);
  
  return count == 1;
  
}

GDBool test6() {
  
  GDGraphRef graph = GDGraphCreateEmpty(3);
  GDGraphAddConnection(graph, 0, 1);
  GDGraphAddConnection(graph, 1, 2);
  GDGraphAddConnection(graph, 0, 2);
  
  int count = runWithGraph(graph);
  GDGraphRelease(graph);
  
  return count == 1;
  
}

GDBool test7() {
  
  GDGraphRef graph = GDGraphCreateEmpty(3);
  GDGraphAddConnection(graph, 0, 1);
  GDGraphAddConnection(graph, 1, 2);
  
  int count = runWithGraph(graph);
  GDGraphRelease(graph);
  
  return count == 0;
  
}

GDBool test8() {
  
  GDGraphRef graph = GDGraphCreateEmpty(0);
  
  int count = runWithGraph(graph);
  GDGraphRelease(graph);
  
  return count == 0;
  
}

void GDAlgorithmTestsRun() {
  
  if ( test1() == YES ) {
    printf("Test 1 OK\n");
  } else {
    printf("Test 1 failed\n");
  }
  
  if ( test2() == YES ) {
    printf("Test 2 OK\n");
  } else {
    printf("Test 2 failed\n");
  }
  
  if ( test3() == YES ) {
    printf("Test 3 OK\n");
  } else {
    printf("Test 3 failed\n");
  }
  
  if ( test4() == YES ) {
    printf("Test 4 OK\n");
  } else {
    printf("Test 4 failed\n");
  }
  
  if ( test5() == YES ) {
    printf("Test 5 OK\n");
  } else {
    printf("Test 5 failed\n");
  }
  
  if ( test6() == YES ) {
    printf("Test 6 OK\n");
  } else {
    printf("Test 6 failed\n");
  }
  
  if ( test7() == YES ) {
    printf("Test 7 OK\n");
  } else {
    printf("Test 7 failed\n");
  }
  
  if ( test8() == YES ) {
    printf("Test 8 OK\n");
  } else {
    printf("Test 8 failed\n");
  }
  
}

