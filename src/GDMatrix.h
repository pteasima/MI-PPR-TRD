//
//  GDMatrix.h
//  GraphDecomposition
//
 

#ifndef GraphDecomposition_GDMatrix_h
#define GraphDecomposition_GDMatrix_h

#include "GDDefines.h"

typedef struct GDMatrix {
  
  unsigned int rowsCount;
  unsigned int colsCount;
  unsigned int ** rows;
  
} GDMatrix;

typedef GDMatrix * GDMatrixRef;

GDMatrixRef GDMatrixCreate(unsigned int rowsCount, unsigned int colsCount);

GDMatrixRef GDMatrixCopy(GDMatrixRef matrix);

void GDMatrixRelease(GDMatrixRef matrix);

void GDMatrixFillWithZeros(GDMatrixRef matrix);

void GDMatrixPrint(GDMatrixRef matrix);

GDBool GDMatrixEqual(GDMatrixRef matrix1, GDMatrixRef matrix2);

#endif
