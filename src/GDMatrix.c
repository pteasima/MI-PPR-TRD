//
//  GDMatrix.c
//  GraphDecomposition
//
 

#include "GDMatrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#pragma mark - Lifecycle

GDMatrixRef GDMatrixCreate(unsigned int rowsCount, unsigned int colsCount) {
  
  GDMatrixRef matrix = malloc(sizeof(GDMatrix));
  
  unsigned int ** rows = malloc(sizeof(unsigned int *) * rowsCount);
  for ( unsigned int rowIdx = 0; rowIdx < rowsCount; rowIdx++ ) {
    rows[rowIdx] = malloc(sizeof(unsigned int) * colsCount);
  }
  matrix->rowsCount = rowsCount;
  matrix->colsCount = colsCount;
  matrix->rows = rows;
  
  GDMatrixFillWithZeros(matrix);
 
  return matrix;
  
}

GDMatrixRef GDMatrixCopy(GDMatrixRef matrix) {
  
  assert(matrix != NULL);
  
  GDMatrixRef copy = malloc(sizeof(GDMatrix));
  
  unsigned int ** rows = malloc(sizeof(unsigned int *) * matrix->rowsCount);
  for ( unsigned int rowIdx = 0; rowIdx < matrix->rowsCount; rowIdx++ ) {
    rows[rowIdx] = malloc(sizeof(unsigned int) * matrix->colsCount);
  }
  copy->rowsCount = matrix->rowsCount;
  copy->colsCount = matrix->colsCount;
  copy->rows = rows;
  for ( unsigned int rowIdx = 0; rowIdx < matrix->rowsCount; rowIdx++ ) {
    for ( unsigned int colIdx = 0; colIdx < matrix->colsCount; colIdx++) {
      copy->rows[rowIdx][colIdx] = matrix->rows[rowIdx][colIdx];;
    }
  }
  
  return copy;
  
}


void GDMatrixRelease(GDMatrixRef matrix) {
  
  if ( !matrix ) {
    return;
  }
  
  for ( unsigned int rowIdx = 0; rowIdx < matrix->rowsCount; rowIdx++ ) {
    free(matrix->rows[rowIdx]);
  }
  
  free(matrix->rows);
  free(matrix);
  
}


#pragma mark - Operations

void GDMatrixFillWithZeros(GDMatrixRef matrix) {
  
  assert(matrix != NULL);
  
  for ( unsigned int rowIdx = 0; rowIdx < matrix->rowsCount; rowIdx++ ) {
    for ( unsigned int colIdx = 0; colIdx < matrix->colsCount; colIdx++) {
      matrix->rows[rowIdx][colIdx] = 0;
    }
  }
  
}

GDBool GDMatrixEqual(GDMatrixRef matrix1, GDMatrixRef matrix2) {
  
  assert(matrix1 != NULL);
  assert(matrix2 != NULL);
  
  if ( matrix1->rowsCount != matrix2->rowsCount ) {
    return NO;
  }
  if ( matrix1->colsCount != matrix2->colsCount ) {
    return NO;
  }
  
  for ( unsigned int rowIdx = 0; rowIdx < matrix1->rowsCount; rowIdx++ ) {
    for ( unsigned int colIdx = 0; colIdx < matrix1->colsCount; colIdx++) {
      if ( matrix1->rows[rowIdx][colIdx] != matrix2->rows[rowIdx][colIdx] ) {
        return NO;
      }
    }
  }
  
  return YES;
  
}


#pragma mark - Debug

void GDMatrixPrint(GDMatrixRef matrix) {
  
  printf("===============================\n");
  printf("    ");
  for ( unsigned int colIdx = 0; colIdx < matrix->colsCount; colIdx++) {
    printf("%d, ", colIdx);
  }
  printf("\n");
  printf("   ");
  for ( unsigned int colIdx = 0; colIdx < matrix->colsCount; colIdx++) {
    printf("___");
  }
  printf("\n");
  
  for ( unsigned int rowIdx = 0; rowIdx < matrix->rowsCount; rowIdx++ ) {
    printf("%d | ", rowIdx);
    for ( unsigned int colIdx = 0; colIdx < matrix->colsCount; colIdx++) {
      if ( colIdx == rowIdx ) {
        printf("x, ");
      } else {
        printf("%d, ", matrix->rows[rowIdx][colIdx]);
      }
    }
    printf("\n");
  }

  printf("=========================\n");
  
  
}
