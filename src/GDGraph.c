//
//  GDGraph.c
//  GraphDecomposition
//
//  Created by Adam Zdara on 29.09.13.
//  Copyright (c) 2013 Adam Zdara. All rights reserved.
//

#include "GDGraph.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#pragma mark - Lifecycle

GDGraphRef GDGraphCreateEmpty(unsigned int nodesCount) {
  
  GDGraphRef graph = malloc(sizeof(GDGraph));
  
  graph->nodesCount = nodesCount;
  graph->edgesCount = 0;
  graph->adjacencyMatrix = GDMatrixCreate(nodesCount, nodesCount);
  
  return graph;
  
}

GDGraphRef GDGraphCreateRandom(unsigned int nodesCount) {
  
  GDGraphRef graph = malloc(sizeof(GDGraph));
  
  graph->nodesCount = nodesCount;
  graph->edgesCount = 0;
  graph->adjacencyMatrix = GDMatrixCreate(nodesCount, nodesCount);
  
  for ( unsigned int rowIdx = 0; rowIdx < graph->nodesCount; rowIdx++ ) {
    for ( unsigned int colIdx = 0; colIdx < graph->nodesCount; colIdx++) {
      GDBool value = colIdx != rowIdx ? random() % 2 : NO;
      graph->adjacencyMatrix->rows[rowIdx][colIdx] = value;
      graph->adjacencyMatrix->rows[colIdx][rowIdx] = value;
    }
  }
  
  return graph;
  
}


GDGraphRef GDGraphCreateFromFile(const char * path) {
  
  assert(path != NULL);
  
  FILE * fileHandler;
  fileHandler = fopen(path, "r");
  if ( fileHandler == NULL ) {
    return NULL;
  }
  
  int nodesCount;
  if ( !fscanf(fileHandler, "%d", &nodesCount) ) {
    return NULL;
  };
  if ( nodesCount < 0 ) {
    return NULL;
  }
  
  // Seek new line
  char newLineChar;
  fscanf(fileHandler, "%c", &newLineChar);
  
  GDGraphRef graph = GDGraphCreateEmpty(nodesCount);
  
  for ( int rowIdx = 0; rowIdx < nodesCount; rowIdx++ ) {
    for ( int colIdx = 0; colIdx < nodesCount + 1; colIdx++ ) {
      
      // Seek new line
      if ( colIdx == nodesCount ) {
        char newLineChar;
        fscanf(fileHandler, "%c", &newLineChar);
        break;
      }
      
      char readChar;
      if ( !fscanf(fileHandler, "%c", &readChar) ) {
        GDGraphRelease(graph);
        return NULL;
      }
      if ( rowIdx != colIdx && readChar == '1' ){
        GDGraphAddConnection(graph, rowIdx, colIdx);
      }
      
    }
    
  }
  
  fclose(fileHandler);
  
  return graph;
  
}

void GDGraphRelease(GDGraphRef graph) {
  
  assert(graph != NULL);
  
  GDMatrixRelease(graph->adjacencyMatrix);
  
  free(graph);
  
}


#pragma mark - Operations

void GDGraphAddConnection(GDGraphRef graph, GDNodeID node1, GDNodeID node2) {

  assert(graph != NULL);
  assert(graph->adjacencyMatrix != NULL);
  assert(node1 != node2);
  assert(node1 < graph->nodesCount);
  assert(node2 < graph->nodesCount);
  
  graph->adjacencyMatrix->rows[node1][node2] = 1;
  graph->adjacencyMatrix->rows[node2][node1] = 1;
  graph->edgesCount = graph->edgesCount + 1;
  
}

GDBool GDGraphHasConnection(GDGraphRef graph, GDNodeID node1, GDNodeID node2) {
  
  return graph->adjacencyMatrix->rows[node2][node1] == 1;
  
}

void GDGraphPrint(GDGraphRef graph) {
  
  assert(graph != NULL);
  assert(graph->adjacencyMatrix != NULL);
  
  printf("===============================\n");
  printf("== Graph (Adjacency Matrix)\n");
  printf("===============================\n");
  printf("    ");
  for ( unsigned int colIdx = 0; colIdx < graph->nodesCount; colIdx++) {
    printf("%d, ", colIdx);
  }
  printf("\n");
  printf("   ");
  for ( unsigned int colIdx = 0; colIdx < graph->nodesCount; colIdx++) {
    printf("___");
  }
  printf("\n");
  
  for ( unsigned int rowIdx = 0; rowIdx < graph->nodesCount; rowIdx++ ) {
    
    printf("%d | ", rowIdx);
    for ( unsigned int colIdx = 0; colIdx < graph->nodesCount; colIdx++) {
      if ( colIdx == rowIdx ) {
        printf("x, ");
      } else {
        printf("%d, ", graph->adjacencyMatrix->rows[rowIdx][colIdx]);
      }
    }
    printf("\n");
  }
  printf("=========================\n");
  
}



#pragma mark - Serialization

GDGraphRef GDGraphCreateFromData(const void * data, unsigned long int length) {
  
  assert("Not implemented");
  // TODO
  
  return NULL;
  
}

void GDGraphGetData(GDGraphRef graph, const void ** data, unsigned long int * length) {
  
  assert("Not implemented");
  // TODO
  
}


