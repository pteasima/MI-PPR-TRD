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



#pragma mark - Private Interfaces

void processState(GDExplorerRef explorer, GDExplorationStackItem state);

void saveSolution(GDExplorerRef explorer);

GDBool expandToNeighbours(GDExplorerRef explorer, GDExplorationStackItem state);

GDBool expandToAnyNode(GDExplorerRef explorer, GDExplorationStackItem state);

GDBool expandToNeighboursOfNode(GDExplorerRef explorer, GDNodeID node, unsigned int level);

GDBool isNodeExpandableToNeighbours(GDExplorerRef explorer, GDNodeID node);

GDBool canExistsBetterSolution(GDExplorerRef explorer);



#pragma mark - Accessors

GDExplorationStackRef GDExplorerGetExplorationStack(GDExplorerRef explorer) {
  
  return explorer->explorationStack;
  
}

void GDExplorerSetExplorationStack(GDExplorerRef explorer, GDExplorationStackRef explorationStack) {
  
  GDExplorationStackRelease(explorer->explorationStack);
  explorer->explorationStack = explorationStack;
  
}



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


#pragma mark - Main

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


#pragma mark - Processing

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
      int levelsDelta = state.level - nextTopContext.level;
      for ( int level = 0; level < levelsDelta; level++ ) {
        GDTriangleListPop(explorer->triangleList);
      }
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


