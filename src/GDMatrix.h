//
//  GDMatrix.h
//  GraphDecomposition
//
 

#ifndef GraphDecomposition_GDMatrix_h
#define GraphDecomposition_GDMatrix_h

typedef struct GDMatrix {
  
  unsigned int rowsCount;
  unsigned int colsCount;
  unsigned int ** rows;
  
} GDMatrix;

typedef GDMatrix * GDMatrixRef;

GDMatrixRef GDMatrixCreate(unsigned int rowsCount, unsigned int colsCount);

void GDMatrixRelease(GDMatrixRef matrix);

void GDMatrixFillWithZeros(GDMatrixRef matrix);

void GDMatrixPrint(GDMatrixRef matrix);

#endif
