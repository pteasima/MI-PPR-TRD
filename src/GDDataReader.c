//
//  GDDataReader.c
//  GraphDecomposition
//

#include "GDDataReader.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


#pragma mark - Lifecycle

GDDataReaderRef GDDataReaderCreateWithCapacity(char * bytes, unsigned long int lenght) {

  GDDataReaderRef dataReader = malloc(sizeof(GDDataReader));
  
  dataReader->bytes = bytes;
  dataReader->lenght = lenght;
  dataReader->position = 0;
  
  return dataReader;
  
}

void GDDataReaderRelease(GDDataReaderRef dataReader) {
  
  assert(dataReader != NULL);
  
  free(dataReader);
  
}


#pragma mark - Operations

unsigned int GDDataReaderReadUnsignedInt(GDDataReaderRef dataReader) {

  size_t bytesCount = sizeof(unsigned int);
  
  unsigned int value = 0;
  for ( size_t byteOffset = 0; byteOffset < bytesCount; byteOffset++ ) {
    value |= (dataReader->bytes[dataReader->position + byteOffset] & 0xFF) << (8 * byteOffset);
  }
  
  dataReader->position += bytesCount;
  
  return value;
  
}

unsigned long int GDDataReaderReadUnsignedLongInt(GDDataReaderRef dataReader) {
  
  size_t bytesCount = sizeof(unsigned long int);
  unsigned long int value = 0;
  for ( size_t byteOffset = 0; byteOffset < bytesCount; byteOffset++ ) {
    value |= (dataReader->bytes[dataReader->position + byteOffset] & 0xFF) << (8 * byteOffset);
  }
  dataReader->position += bytesCount;
  
  return value;
  
}

void GDDataReaderReadBytes(GDDataReaderRef dataReader, unsigned long int lenght, char ** bytes) {
  
  assert(bytes != NULL);
  assert((dataReader->position + lenght) <= dataReader->lenght);
  
  *bytes = malloc(sizeof(char) * lenght);
  
  char * source = &dataReader->bytes[dataReader->position];
  
  memcpy(*bytes, source, lenght);
  
  dataReader->position += lenght;
  
}



