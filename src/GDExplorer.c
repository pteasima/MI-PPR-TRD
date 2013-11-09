//
//  GDExplorer.c
//  GraphDecomposition
//
 

#include "GDGraph.h"
#include "GDSolution.h"
#include "GDExplorationStack.h"
#include "GDTriangleList.h"
#include "GDExplorer.h"
#include "GDDefines.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "GDDataWriter.h"
#include "GDDataReader.h"


#pragma mark - Private Interfaces

void processState(GDExplorerRef explorer, GDExplorationStackItem state);

void saveSolution(GDExplorerRef explorer);

GDBool expandToNeighbours(GDExplorerRef explorer, GDExplorationStackItem state);

GDBool expandToAnyNode(GDExplorerRef explorer, GDExplorationStackItem state);

GDBool expandToNeighboursOfNode(GDExplorerRef explorer, GDNodeID node, unsigned int level);

GDBool isNodeExpandableToNeighbours(GDExplorerRef explorer, GDNodeID node);

GDBool canExistsBetterSolution(GDExplorerRef explorer);


#pragma mark - Lifecycle

GDExplorerRef GDExplorerCreate(GDGraphRef graph) {
  
  assert(graph != NULL);
  
  GDExplorerRef explorer = malloc(sizeof(GDExplorer));
  explorer->graph = graph;
  explorer->triangleList = GDTriangleListCreateWithCapacity(32, graph->nodesCount);
  explorer->explorationStack = GDExplorationStackCreateWithCapacity(32);
  explorer->bestSolution = NULL;
  return explorer;
  
}

void GDExplorerRelease(GDExplorerRef explorer) {
  
  if ( explorer->bestSolution ) {
    GDSolutionRelease(explorer->bestSolution);
    explorer->bestSolution = NULL;
  }
  
  GDExplorationStackRelease(explorer->explorationStack);
  GDTriangleListRelease(explorer->triangleList);
  
  free(explorer);
  
}


#pragma mark - Exploration

void GDExplorerRun(GDExplorerRef explorer, int stepsLimit, GDBool * canExistBetter) {
  
  int stepCounter = 0;
  while ( explorer->explorationStack->count > 0 ) {
    
    GDExplorationStackItem state = GDExplorationStackPop(explorer->explorationStack);
    processState(explorer, state);

    if ( !GDSolutionCanExistsBetter(explorer->bestSolution) ) {
      if ( canExistBetter ) {
        *canExistBetter = NO;
      }
      return;
    }
    
    stepCounter++;
    if ( stepsLimit > 0 && stepCounter >= stepsLimit ) {
      break;
    }
    
  }
  
  if ( canExistBetter ) {
    *canExistBetter = YES;
  }
  
}

void processState(GDExplorerRef explorer, GDExplorationStackItem state) {
  
  GDTriangleListPush(explorer->triangleList, state.node);
  
  GDBool closed = GDTriangleListIsLastTriangleClosed(explorer->triangleList, explorer->graph);
  
  if ( closed ) {
    saveSolution(explorer);
  }
  
  GDBool expanded = NO;
  if ( canExistsBetterSolution(explorer) ) {
    if ( !closed ) {
      expanded = expandToNeighbours(explorer, state);
    } else {
      expanded = expandToAnyNode(explorer, state);
    }
  }
  
  if ( !expanded ) {
    
    GDTriangleListPop(explorer->triangleList);
    if ( explorer->explorationStack->count > 0 ) {
      GDExplorationStackItem nextTopContext = GDExplorationStackTop(explorer->explorationStack);
      GDTriangleListPopMultiple(explorer->triangleList, state.level - nextTopContext.level);
    }
    
  }
  
}

void saveSolution(GDExplorerRef explorer) {
  
  assert(explorer != NULL);
  
  if ( explorer->bestSolution == NULL ) {
    explorer->bestSolution = GDSolutionCreate(explorer->graph, explorer->triangleList);
  } else {
    
    unsigned int currentSolutionTrianglesCount = explorer->triangleList->count / 3;
    unsigned int bestSolutionTrianglesCount = explorer->bestSolution->triangleList->count / 3;
    
    if ( currentSolutionTrianglesCount > bestSolutionTrianglesCount ) {
      GDSolutionRelease(explorer->bestSolution);
      explorer->bestSolution = GDSolutionCreate(explorer->graph, explorer->triangleList);
    }
    
  }
  
}

GDBool expandToNeighbours(GDExplorerRef explorer, GDExplorationStackItem state) {
  
  // Triangle can have only three nodes
  if ( explorer->triangleList->count % 3 == 0 ) {
    GDNodeID thirdNodeValue = explorer->triangleList->values[explorer->triangleList->count - 1];
    GDNodeID firstNodeValue = explorer->triangleList->values[explorer->triangleList->count - 3];
    if ( !GDGraphHasConnection(explorer->graph, firstNodeValue, thirdNodeValue) ) {
      return NO;
    }
    if ( GDTriangleListContainsEdge(explorer->triangleList, firstNodeValue, thirdNodeValue, NO) ) {
      return NO;
    }
  }
  
  // Must have any neighbour
  return expandToNeighboursOfNode(explorer, state.node, state.level + 1);
  
}

GDBool expandToNeighboursOfNode(GDExplorerRef explorer, GDNodeID node, unsigned int level) {
  
  GDBool expanded = NO;
  for ( GDNodeID otherNode = 0; otherNode < explorer->graph->nodesCount; otherNode++ ) {
    if ( otherNode == node ) {
      continue;
    }
    if ( !GDGraphHasConnection(explorer->graph, node, otherNode) ) {
      continue;
    }
    if ( !GDTriangleListContainsEdge(explorer->triangleList, node, otherNode, NO) ) {
      GDExplorationStackItem newState = GDExplorationStackCreateItem(level, otherNode);
      GDExplorationStackPush(explorer->explorationStack, newState);
      expanded = YES;
    }
  }
  return expanded;
  
}

GDBool expandToAnyNode(GDExplorerRef explorer, GDExplorationStackItem state) {
  
  GDBool expanded = NO;
  for ( GDNodeID node = 0; node < explorer->graph->nodesCount; node++ ) {
    if ( isNodeExpandableToNeighbours(explorer, node) ) {
      GDExplorationStackItem newState = GDExplorationStackCreateItem(state.level + 1, node);
      GDExplorationStackPush(explorer->explorationStack, newState);
      expanded = YES;
    }
  }
  return expanded;
  
}

GDBool isNodeExpandableToNeighbours(GDExplorerRef explorer, GDNodeID node) {
  
  GDBool expandableNode = NO;
  for ( GDNodeID otherNode = 0; otherNode < explorer->graph->nodesCount; otherNode++ ) {
    if ( otherNode == node ) {
      continue;
    }
    GDBool connected = explorer->graph->adjacencyMatrix->rows[node][otherNode];
    if ( !connected ) {
      continue;
    }
    if ( !GDTriangleListContainsEdge(explorer->triangleList, node, otherNode, NO) ) {
      expandableNode = YES;
      break;
    }
  }
  return expandableNode;
  
}

GDBool canExistsBetterSolution(GDExplorerRef explorer) {

  if ( explorer->bestSolution == NULL ) {
    return YES;
  }
  
  int totalEdgesCount = explorer->graph->edgesCount;
  int closedTrianglesEdgesCount = GDTriangleListGetNumberOfClosedTriangles(explorer->triangleList) * 3;
  int canUseEdgesCount = totalEdgesCount - closedTrianglesEdgesCount;
  int bestPossibleTrianglesCount = GDTriangleListGetNumberOfClosedTriangles(explorer->triangleList) + canUseEdgesCount / 3;

  GDBool retVal = bestPossibleTrianglesCount > GDTriangleListGetNumberOfClosedTriangles(explorer->bestSolution->triangleList);
  
  return retVal;
  
}


#pragma mark - Work Distribution

void GDExplorerInitializeWork(GDExplorerRef explorer) {
  
  GDExplorationStackRef stack = GDExplorationStackCreateWithCapacity(64);
  for ( GDNodeID node = 0; node < explorer->graph->nodesCount; node++ ) {
    GDExplorationStackItem state = GDExplorationStackCreateItem(0, node);
    GDExplorationStackPush(stack, state);
  }
  
  GDExplorationStackRelease(explorer->explorationStack);
  explorer->explorationStack = stack;
  
}

GDBool GDExplorerGetWork(GDExplorerRef explorer, char ** bytes, unsigned long int * lenght) {

  if ( explorer->explorationStack->count == 0 ) {
    return NO;
  }
  
  GDExplorationStackRef stack = explorer->explorationStack;
  GDExplorationStackRef stackSplited = GDExplorationStackSplit(stack);

  if ( stackSplited->count == 0 ) {
    GDExplorationStackRelease(stackSplited);
    return NO;
  }
  
  assert( stackSplited->count > 0 );
  assert( stack->count > 0 );
  
  GDTriangleListRef triangleListSplited = GDTriangleListCopy(explorer->triangleList);
  GDTriangleListPopMultiple(triangleListSplited, triangleListSplited->count - GDExplorationStackTop(stackSplited).level);
  GDTriangleListPopMultiple(explorer->triangleList, explorer->triangleList->count - GDExplorationStackTop(stack).level);
  
  char * triangleListBytes;
  unsigned long int triangleListLenght;
  GDTriangleListGetData(triangleListSplited, &triangleListBytes, &triangleListLenght);
  
  char * stackBytes;
  unsigned long int stackLenght;
  GDExplorationStackGetData(stackSplited, &stackBytes, &stackLenght);
  
  GDDataWriterRef writer = GDDataWriterCreateWithCapacity(sizeof(unsigned long int) + triangleListLenght + sizeof(unsigned long int) + stackLenght);

  GDDataWriterWriteUnsignedLongInt(writer, triangleListLenght);
  GDDataWriterWriteBytes(writer, triangleListBytes, triangleListLenght);
  
  GDDataWriterWriteUnsignedLongInt(writer, stackLenght);
  GDDataWriterWriteBytes(writer, stackBytes, stackLenght);
  
  free(triangleListBytes);
  free(stackBytes);
  
  if ( bytes ) {
    *bytes = writer->bytes;
  } else {
    free(bytes);
  }
  
  if ( lenght ) {
    *lenght = writer->lenght;
  }
  
  GDTriangleListRelease(triangleListSplited);
  GDExplorationStackRelease(stackSplited);
  GDDataWriterRelease(writer);
  
  return YES;
  
}

void GDExplorerSetWork(GDExplorerRef explorer, char * bytes, unsigned long int lenght) {
	
  GDDataReaderRef reader = GDDataReaderCreateWithCapacity(bytes, lenght);
  unsigned long int triangleListLenght = GDDataReaderReadUnsignedLongInt(reader);
  char * triangleListBytes;
  GDDataReaderReadBytes(reader, triangleListLenght, &triangleListBytes);
  GDTriangleListRef triangleList = GDTriangleListCreateFromData(triangleListBytes, triangleListLenght);
  free(triangleListBytes);
  unsigned long int stackLenght = GDDataReaderReadUnsignedLongInt(reader);
  char * stackBytes;
  GDDataReaderReadBytes(reader, stackLenght, &stackBytes);
  GDExplorationStackRef stack = GDExplorationStackCreateFromData(stackBytes, stackLenght);
  free(stackBytes);
  
  GDTriangleListRelease(explorer->triangleList);
  GDExplorationStackRelease(explorer->explorationStack);
  explorer->triangleList = triangleList;
  explorer->explorationStack = stack;
}





