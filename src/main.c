//
//  main.c
//  GraphDecomposition
//

#include <stdio.h>
#include <stdlib.h>
#include "GDGraph.h"
#include "GDExplorer.h"
#include "GDSolution.h"
#include "GDAlgorithmTests.h"

#pragma mark - General

void initialize(GDExplorerRef explorer) {
  
  printf("Initializing...\n");
  GDExplorationStackRef stack = GDExplorationStackCreateWithCapacity(64);
  GDExplorationStackAddAllNodes(stack, explorer->graph);
  GDExplorerSetExplorationStack(explorer, stack);
  
  /*
   
   TODO
   
   1. Poslat graf vsem ostanim
   
   2. rekne vsem ze muzou zahjit vypocet (poadat prvniho o praci)

   */
  
}

void runLoop(GDExplorerRef explorer) {
  
  printf("Finding best solution...\n");
  
  while ( YES ) {

    GDBool canExistBetterSolution;
    GDExplorerRun(explorer, -1, &canExistBetterSolution);
    
    if (explorer->explorationStack->count == 0 || canExistBetterSolution == NO ) {
      break;
    }
    
  }
  
  /*
   
   TODO
   
   while {
   
   1. Mam praci a pocitam - GDExplorerRun ( 200 )
   
   2. Vyberu postu
   
     a. Rozdelim svuj zasobnik kdyz nekdo zada (GDExplorationStackSplit) a poslu praci nekomu jemu (GDExplorationStackGetData, GDExplorationStackhCreateFromData)
   
     b. Poslu token kdyz nemam co delat
   
     c. master rozhoduje podle velikosti svyho zasobniku o tom jestli zahajit ukoncovaci posilani tokenu
   
   */
  
}

void finallize(GDExplorerRef explorer) {

  printf("Finalizing...\n");
  GDSolutionPrint(explorer->bestSolution);
  
  /*
   
   TODO
   
   1. redukce a sbirani vysledku
   
   2. prezentace vysledku
   
   */
  
}


#pragma mark - Main

int main(int argc, const char * argv[]) {
  
  if ( argc != 2 ) {
    printf("Invalid number of arguments\n");
    return 0;
  }
  
  const char * path = argv[1];
  
  GDGraphRef graph = GDGraphCreateFromFile(path);
  
  if ( graph == NULL ) {
    printf("Unable to load graph (%s)\n", path);
    return 0;
  }
  
  GDExplorerRef explorer = GDExplorerCreate(graph);
  
  initialize(explorer);
  runLoop(explorer);
  finallize(explorer);
  
  GDExplorerRelease(explorer);
  
  GDGraphRelease(graph);

  return 0;
  
}



