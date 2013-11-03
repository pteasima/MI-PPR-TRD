//
//  GDDataBuilder.h
//  GraphDecomposition
//

#ifndef GraphDecomposition_GDDataReader_h
#define GraphDecomposition_GDDataReader_h

#include "GDDefines.h"

typedef struct GDDataReader {
  
  unsigned int position;
  char * bytes;
  unsigned long int lenght;
  
} GDDataReader;

typedef GDDataReader * GDDataReaderRef;

GDDataReaderRef GDDataReaderCreateWithCapacity(char * bytes, unsigned long int lenght);

void GDDataReaderRelease(GDDataReaderRef dataBuilder);

unsigned int GDDataReaderReadUnsignedInt(GDDataReaderRef dataReader);

#endif
