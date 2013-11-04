//
//  GDDataWriter.c
//  GraphDecomposition
//

#include "GDDataWriter.h"


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#pragma mark - Lifecycle

GDDataWriterRef GDDataWriterCreateWithCapacity(unsigned long int lenght) {

  GDDataWriterRef dataWriter = malloc(sizeof(GDDataWriter));

  dataWriter->bytes = malloc(sizeof(char) * lenght);
  dataWriter->lenght = lenght;
  dataWriter->position = 0;
  
  return dataWriter;
  
}

void GDDataWriterRelease(GDDataWriterRef dataWriter) {

  assert(dataWriter != NULL);
  
  free(dataWriter);
  
}


#pragma mark - Operations

void GDDataWriterWriteUnsignedInt(GDDataWriterRef dataWriter, unsigned int value) {

  size_t bytesCount = sizeof(unsigned int);
  assert((dataWriter->position + bytesCount) <= dataWriter->lenght);
  
  for ( size_t byteOffset = 0; byteOffset < bytesCount; byteOffset++ ) {
    dataWriter->bytes[dataWriter->position + byteOffset] = (value >> (8 * byteOffset)) & 0xFF;
  }
  
  dataWriter->position += bytesCount;
  
}

void GDDataWriterWriteUnsignedLongInt(GDDataWriterRef dataWriter, unsigned long int value) {
  
  size_t bytesCount = sizeof(unsigned long int);
  assert((dataWriter->position + bytesCount) <= dataWriter->lenght);
  
  for ( size_t byteOffset = 0; byteOffset < bytesCount; byteOffset++ ) {
    dataWriter->bytes[dataWriter->position + byteOffset] = (value >> (8 * byteOffset)) & 0xFF;
  }
  
  dataWriter->position += bytesCount;
  
}

void GDDataWriterWriteBytes(GDDataWriterRef dataWriter, char * bytes, unsigned long int lenght) {
  
  assert((dataWriter->position + lenght) <= dataWriter->lenght);
  
  char * destination = &dataWriter->bytes[dataWriter->position];
  
  memcpy(destination, bytes, lenght);
  
  dataWriter->position += lenght;
  
}




