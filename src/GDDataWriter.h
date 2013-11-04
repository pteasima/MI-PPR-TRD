//
//  GDDataWriter.h
//  GraphDecomposition
//

#ifndef GraphDecomposition_GDDataWriter_h
#define GraphDecomposition_GDDataWriter_h

#include "GDDefines.h"

typedef struct GDDataWriter {
  
  unsigned int position;
  char * bytes;
  unsigned long int lenght;
  
} GDDataWriter;

typedef GDDataWriter * GDDataWriterRef;

GDDataWriterRef GDDataWriterCreateWithCapacity(unsigned long int lenght);

void GDDataWriterRelease(GDDataWriterRef dataWriter);

void GDDataWriterWriteUnsignedInt(GDDataWriterRef dataWriter, unsigned int value);

void GDDataWriterWriteUnsignedLongInt(GDDataWriterRef dataWriter, unsigned long int value);

void GDDataWriterWriteBytes(GDDataWriterRef dataWriter, char * bytes, unsigned long int lenght);

#endif
