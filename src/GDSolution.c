//
//  GDSolution.c
//  GraphDecomposition
//
 

#include "GDSolution.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#pragma mark - Private Interface

GDMatrixRef GDSolutionCreateIncidenceMatrix(GDSolutionRef solution);

void GDSolutionAddTriangleToIncidenceMatrix(GDMatrixRef matrix, GDTriangleID triangle, GDNodeID node1,
                                            GDNodeID node2, GDNodeID node3, unsigned int * edgeCounter);


#pragma mark - Lifecycle

GDSolutionRef GDSolutionCreate(GDGraphRef graph, GDTriangleListRef triangleList) {
  
  assert(graph != NULL);
  assert(triangleList->count % 3 == 0);
  
  GDSolutionRef solution = malloc(sizeof(GDSolution));
  solution->graph = graph;
  solution->triangleList = GDTriangleListCopy(triangleList);

  return solution;
  
}

void GDSolutionRelease(GDSolutionRef solution) {
  
  assert(solution != NULL);
  
  GDTriangleListRelease(solution->triangleList);
  solution->triangleList = NULL;
  free(solution);
  
}


#pragma mark - Print

void GDSolutionPrint(GDSolutionRef solution) {

  if ( solution ) {
    
    GDMatrixRef incidenceMatrix = GDSolutionCreateIncidenceMatrix(solution);
    
    printf("===============================\n");
    printf("== Solution (Incidence Matrix)\n");
    printf("===============================\n");
    printf("triangles = %d\n", solution->triangleList->count / 3);
    for ( unsigned int rowIdx = 0; rowIdx < incidenceMatrix->rowsCount; rowIdx++ ) {
      printf("%d | ", rowIdx);
      for ( unsigned int colIdx = 0; colIdx < incidenceMatrix->colsCount; colIdx++) {
        printf("%d, ", incidenceMatrix->rows[rowIdx][colIdx]);
      }
      printf("\n");
    }
    printf("=========================\n");
    
    GDMatrixRelease(incidenceMatrix);
    
  } else {
    
    printf("===============================\n");
    printf("== NO SOLUTION\n");
    printf("===============================\n");
    
  }
  
}

GDMatrixRef GDSolutionCreateIncidenceMatrix(GDSolutionRef solution) {
  
  unsigned int edgesCount = solution->triangleList->count;
  GDMatrixRef incidenceMatrix = GDMatrixCreate(solution->graph->nodesCount, edgesCount);
  
  unsigned int edgeCounter = 0;
  for ( unsigned int firstNodeIdx = 0; firstNodeIdx < solution->triangleList->count; firstNodeIdx += 3 ) {
    
    GDTriangleID triangle = 1 + firstNodeIdx / 3;
    
    GDSolutionAddTriangleToIncidenceMatrix(incidenceMatrix,
                                           triangle,
                                           solution->triangleList->values[firstNodeIdx],
                                           solution->triangleList->values[firstNodeIdx + 1],
                                           solution->triangleList->values[firstNodeIdx + 2],
                                           &edgeCounter);
    
  }
  
  return incidenceMatrix;
  
}

void GDSolutionAddTriangleToIncidenceMatrix(GDMatrixRef matrix, GDTriangleID triangle, GDNodeID node1,
                                            GDNodeID node2, GDNodeID node3, unsigned int * edgeCounter) {

  assert(matrix != NULL);
  assert(edgeCounter != NULL);
  
  matrix->rows[node1][*edgeCounter] = triangle;
  matrix->rows[node2][*edgeCounter] = triangle;
  (*edgeCounter)++;

  matrix->rows[node2][*edgeCounter] = triangle;
  matrix->rows[node3][*edgeCounter] = triangle;
  (*edgeCounter)++;
  
  matrix->rows[node1][*edgeCounter] = triangle;
  matrix->rows[node3][*edgeCounter] = triangle;
  (*edgeCounter)++;

}

GDBool GDSolutionCanExistsBetter(GDSolutionRef solution) {
  
  if ( solution == NULL ) {
    return YES;
  }
  
  return GDTriangleListGetNumberOfClosedTriangles(solution->triangleList) < solution->graph->edgesCount / 3;
  
}

