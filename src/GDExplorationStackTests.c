//
//  GDGraphSerializationTests.c
//  GraphDecomposition
//

#include <stdio.h>
#include <stdlib.h>
#include "GDDefines.h"
#include "GDExplorationStack.h"

GDBool testSerializationAnyData() {
  
  GDExplorationStackRef stack = GDExplorationStackCreateWithCapacity(100);
  GDExplorationStackPush(stack, (GDExplorationStackItem){6, 1});
  GDExplorationStackPush(stack, (GDExplorationStackItem){5, 2});
  GDExplorationStackPush(stack, (GDExplorationStackItem){4, 3});
  GDExplorationStackPush(stack, (GDExplorationStackItem){3, 4});
  GDExplorationStackPush(stack, (GDExplorationStackItem){2, 5});
  GDExplorationStackPush(stack, (GDExplorationStackItem){1, 6});
  GDExplorationStackPush(stack, (GDExplorationStackItem){111122, 1234567});
  
  unsigned long lenght;
  char * bytes;
  GDExplorationStackGetData(stack, &bytes, &lenght);
  GDExplorationStackRef stackCopy = GDExplorationStackhCreateFromData(bytes, lenght);
  free(bytes);
  
  GDBool equals = GDExplorationStackEqual(stackCopy, stack);
  
  GDExplorationStackRelease(stackCopy);
  GDExplorationStackRelease(stack);
  
  return equals;
  
}

GDBool testSerializationNoData() {
  
  GDExplorationStackRef stack = GDExplorationStackCreateWithCapacity(100);
  
  unsigned long lenght;
  char * bytes;
  GDExplorationStackGetData(stack, &bytes, &lenght);
  GDExplorationStackRef stackCopy = GDExplorationStackhCreateFromData(bytes, lenght);
  free(bytes);
  
  GDBool equals = GDExplorationStackEqual(stackCopy, stack);
  
  GDExplorationStackRelease(stackCopy);
  GDExplorationStackRelease(stack);
  
  return equals;
  
}

GDBool testSplitAndRestore() {
  
  return NO;
  
}

void GDExplorationStackTestsRun() {
  
  printf("\nEXPLORATION STACK TESTS:\n");
  
  if ( testSerializationAnyData() == YES ) {
    printf("Serialization 1.....OK\n");
  } else {
    printf("Serialization 1.....failed\n");
  }
  
  if ( testSerializationNoData() == YES ) {
    printf("Serialization 2.....OK\n");
  } else {
    printf("Serialization 2.....failed\n");
  }
  
  if ( testSplitAndRestore() == YES ) {
    printf("Split & Restore.....OK\n");
  } else {
    printf("Split & Restore .....failed\n");
  }
  
  
}


