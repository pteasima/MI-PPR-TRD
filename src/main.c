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
#include "GDExplorerDataDistributionTests.h"

#include "mpi.h"


#include <string.h>
#include <unistd.h>

#pragma mark - General

#define WORK_REQUEST_TAG 1
#define WORK_RESPONSE_TAG 2
#define GRAPH_TAG 3


#define GRAPH_BUFFER_LENGTH 1024
#define STACK_BUFFER_LENGTH 1024

#define EXPLORER_RUN_MAX_STEPS 200


/*
 
 1. jak zjistit velikost bufferu pro graf?
   1.1 v p0
	1.2 v ostatnich (slo by blokujici Probe, Get_count, alokovat pamet a az potom Recv)
 
 2. jak zjistit velikost bufferu pro stack?
	2.1 v odesilateli (process ktery dela split stacku)
	2.2 v prijemci(recv volam az po neblokujicim probe, takze by slo Get_count, alokovat pamet a az potom Recv)
 
 3. process ktery byl pozadan musi bud poslat praci nebo odpoved delky 0. Process ktery o praci zadal je blokovan dokud nedostane odpoved.
	
 4. TODO:logika posilani tokenu musi byt soucasti run loopu, finalize udela az redukci vysledku
 */

GDExplorerRef explorer;
GDGraphRef graph;

void * graphBuffer;
void * stackBuffer;


MPI_Request workRequest;
MPI_Status workRequestStatus;

MPI_Request sendWorkRequest;
MPI_Status sendWorkStatus;


int myRank;
int processCount;


void initialize(const char * path) {
	//init MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &processCount);
	
	//TODO determine correct buffer sizes.
	graphBuffer = malloc(GRAPH_BUFFER_LENGTH * sizeof(MPI_BYTE));
	stackBuffer = malloc(STACK_BUFFER_LENGTH * sizeof(MPI_BYTE));
	
	if(myRank == 0){
		graph = GDGraphCreateFromFile(path);
		if(graph == NULL){
			printf("Unable to load graph (%s)\n", path);
			
			MPI_Abort(MPI_COMM_WORLD, 0);
			//unreachable code? abort terminates all processes immediatelly (including this one?)
			MPI_Finalize();
			exit(0);
		}
		
		
		//this code distributes the graph among all processes in O(processCount) steps, it could be done in O(log(processCount)) steps.
		unsigned long length = 0;
		GDGraphGetData(graph, &graphBuffer, &length);
		int i;
		MPI_Request sendGraphRequest = NULL;
		MPI_Status sendGraphStatus;
		for (i = 1; i < processCount; i++) {
			//TODO handle too large messages if needed

			//wait for previous request to finish
			if (sendGraphRequest != NULL) {
				MPI_Wait(&sendGraphRequest, &sendGraphStatus);
			}
			
			
			MPI_Isend(graphBuffer, (int)length, MPI_BYTE, i, GRAPH_TAG, MPI_COMM_WORLD, &sendGraphRequest);
		}
	}else{
		MPI_Status recvGraphStatus;
		MPI_Recv(graphBuffer, GRAPH_BUFFER_LENGTH, MPI_BYTE, MPI_ANY_SOURCE, GRAPH_TAG, MPI_COMM_WORLD, &recvGraphStatus);
		int actualLength = -1;
		MPI_Get_count(&recvGraphStatus, MPI_BYTE, &actualLength);
		graph = GDGraphCreateFromData(graphBuffer, actualLength);
	}
	
	GDExplorerRef explorer = GDExplorerCreate(graph);
	
	
	if(myRank == 0){
		printf("Initializing...\n");
		GDExplorationStackRef stack = GDExplorationStackCreateWithCapacity(64);
		GDExplorationStackAddAllNodes(stack, explorer->graph);
		GDExplorerSetExplorationStack(explorer, stack);
	}
	//wait until all processes have received graph and p0 has initialized its stack
	MPI_Barrier(MPI_COMM_WORLD);
	if(myRank == 0) printf("processes can now ask for work.\n");
	
	/*
	 
	 TODO
	 
	 (DONE) 1. Poslat graf vsem ostanim
	 
	 (DONE) 2. rekne vsem ze muzou zahjit vypocet (pozadat prvniho o praci) - nepozadaji p0, pusti se runloop a zacnou zadat nahodne. Zadat na zacatku vsichni p0 by bylo spatne - nejdriv se rozpuli, potom zase rozpuli atd... Pokud chceme efektivnejsi zacatek nez random zadani o praci, musime praci rozdelit uz tady v initialize().
	 
	 */
	
}

//wait for previous work request to finish
void waitForPreviousWorkRequest()
{
	MPI_Wait(&workRequest, &workRequestStatus);
}

//wait for previous send work request to finish
void waitForPreviousSendWorkRequest()
{
	MPI_Wait(&sendWorkRequest, &sendWorkStatus);
}



void sendWork(int destination){
	waitForPreviousSendWorkRequest();
	
	// do we need unsinged long? Max mpi message length is INT_MAX. Sending larger stacks would require chaining messages or defining a contiguous data type (http://stackoverflow.com/questions/13558861/maximum-amount-of-data-that-can-be-sent-using-mpisend)
	unsigned long length = 0;
	
	//TODO where to put optimization logic? - do not split and send a stack that is already small
	//-- GDExplorationStackSplit returns stack of length 0
	//OR
	//-- check originalStack->count here and send message of length 0 if count too small
	GDExplorationStackRef originalStack = 	GDExplorerGetExplorationStack(explorer);
	GDExplorationStackRef splitStack = GDExplorationStackSplit(originalStack);
	GDExplorationStackGetData(splitStack, &stackBuffer, &length);

	MPI_Isend(stackBuffer, (int)length, MPI_BYTE, destination, WORK_RESPONSE_TAG, MPI_COMM_WORLD, &sendWorkRequest);
}

void checkIncomingWorkRequests()
{
	//reads all pending work requests sequentially and responds to each
	int flag = 0;
	do{
		//status can be local, theres no need to wait for previous requests since we using blocking Recv
		MPI_Status status;
		MPI_Iprobe(MPI_ANY_SOURCE, WORK_REQUEST_TAG, MPI_COMM_WORLD, &flag, &status);
		if(flag){
			
			int length = 0;
			MPI_Recv(NULL, 0, MPI_CHAR, MPI_ANY_SOURCE, WORK_REQUEST_TAG, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status, MPI_CHAR, &length);
			int source = status.MPI_SOURCE;
			printf("process %d received work request from process %d\n", myRank, source);
//			printf ("Message status: source=%d, tag=%d, error=%d, length=%d\n",source, status.MPI_TAG, status.MPI_ERROR, length);

			sendWork(source);
		}
	}while(flag);
}

void askForWork();

void receiveWork(){
	//blocking receive - new work or message of length 0
	MPI_Recv(&stackBuffer, STACK_BUFFER_LENGTH, MPI_BYTE, MPI_ANY_SOURCE, WORK_RESPONSE_TAG, MPI_COMM_WORLD, &workRequestStatus);
	int count = 0;
	MPI_Get_count(&workRequestStatus, MPI_BYTE, &count);
	//no work received, ask someone else
	//TODO this solution might not be consistent with token logic - could lead to a deadlock (everyone asking everyone else for work indefinitelly)
	if(count == 0){
		askForWork();
	}else{
		GDExplorationStackRef newStack = GDExplorationStackhCreateFromData(stackBuffer, count);
		GDExplorerSetExplorationStack(explorer, newStack);
	}
	
}


void askForWork()
{
	//randomly generate destination process number (different from self)
	//room for optimization - at least dont ask the same process twice in a row
	int destination;
	do {
		destination = rand() % processCount;
	} while (destination == myRank);
	
	printf("process %d sending work request to process %d.\n", myRank, destination);
	MPI_Isend(NULL, 0, MPI_CHAR, destination, WORK_REQUEST_TAG, MPI_COMM_WORLD, &workRequest);
	//theres no point continuing until i have successfully asked for work
	waitForPreviousWorkRequest();
	
	receiveWork();
}

void checkMail()
{
	checkIncomingWorkRequests();
}


void runLoop() {
	
	if(myRank == 0){
		printf("Finding best solution...\n");
	}
	
	
	while ( YES ) {
		
		checkMail();
		
		GDBool canExistBetterSolution;
		GDExplorerRun(explorer, EXPLORER_RUN_MAX_STEPS, &canExistBetterSolution);
		

		if (explorer->explorationStack->count == 0 || canExistBetterSolution == NO) {
			if (myRank == 0) {
					//TODO zacit ukoncovani
			}
			askForWork();
		}
		
	}
	
	/*
	 
	 TODO
	 
	 while {
	 
	 (DONE)1. Mam praci a pocitam - GDExplorerRun ( 200 )
	 
	 2. Vyberu postu
	 
	 a. Rozdelim svuj zasobnik kdyz nekdo zada (GDExplorationStackSplit) a poslu praci nekomu jemu (GDExplorationStackGetData, GDExplorationStackhCreateFromData)
	 
	 b. Poslu token kdyz nemam co delat
	 
	 c. master rozhoduje podle velikosti svyho zasobniku o tom jestli zahajit ukoncovaci posilani tokenu
	 
	 */
	
}

void finalize() {
	
	printf("Finalizing...\n");
	GDSolutionPrint(explorer->bestSolution);
	
	/*
	 
	 TODO
	 
	 1. redukce a sbirani vysledku
	 
	 2. prezentace vysledku
	 
	 */
	
}



#pragma mark - Main

int main(int argc, char * argv[]) {
	
	if ( argc != 2 ) {
		printf("Invalid number of arguments\n");
		return 0;
	}
	MPI_Init(&argc, &argv);
	
	
	const char * path = argv[1];
		
	initialize(path);
	runLoop();
	finalize();
	
	GDExplorerRelease(explorer);
	
	GDGraphRelease(graph);
	
	//buffery pevne velikosti alokovane v initialize(), odstranit pokud vyresim alokaci bufferu pred kazdym recv
	free(graphBuffer);
	free(stackBuffer);
	
	
	//finalize MPI
	MPI_Finalize();
  
#warning <#message#>
  GDExplorerDataDistributionTestsRun();
  GDAlgorithmTestsRun();
  
	return 0;
	
}



