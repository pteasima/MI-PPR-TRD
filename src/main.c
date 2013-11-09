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


void waitForPreviousWorkProbeRequest();
void waitForPreviousWorkRequest();
void waitForPreviousSendWorkRequest();
void sendWork(int destination);
void checkIncomingWorkRequests();
void receiveWork();
void askForWork();
void checkMail();
void cleanUpSentWorks();
void initialize(const char * path);
void runLoop();
void finalize();

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


#define PRINTLOCATION printf("file: %s, function: %s, line: %d\n",__FILE__, __FUNCTION__, __LINE__)
#define PRINTPROCESSANDLOCATION printf("--- p%d   |   file: %s, function: %s, line: %d\n", myRank ,__FILE__, __FUNCTION__, __LINE__)

#pragma mark - Constants

#define EXPLORER_RUN_MAX_STEPS 200


#pragma mark - Vars

GDExplorerRef explorer;
GDGraphRef graph;

int myRank;
int processCount;

#pragma mark - Main

int main(int argc, char * argv[]) {
	
	
	
	if ( argc != 2 ) {
		printf("Invalid number of arguments\n");
		return 0;
	}
	MPI_Init(&argc, &argv);
	//		sleep(3);
	
	const char * path = argv[1];
	
	initialize(path);
	runLoop();
	finalize();
	
	GDExplorerRelease(explorer);
	
	GDGraphRelease(graph);
	
	//buffery pevne velikosti alokovane v initialize(), odstranit pokud vyresim alokaci bufferu pred kazdym recv
	//	free(graphBuffer);
	
	
	//finalize MPI
	MPI_Finalize();
	//
	//#warning <#message#>
	//  GDExplorerDataDistributionTestsRun();
	//  GDAlgorithmTestsRun();
	
	return 0;
	
}


//////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma mark - Constants

#define GRAPH_TAG 3

#define GRAPH_BUFFER_LENGTH 1024


#pragma mark - Init

void initialize(const char * path) {
	//init MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &processCount);
	
	//TODO determine correct buffer sizes.
	
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
		char *graphData;
		GDGraphGetData(graph, &graphData, &length);
		int i;
		for (i = 1; i < processCount; i++) {
			MPI_Send(graphData, (int)length, MPI_BYTE, i, GRAPH_TAG, MPI_COMM_WORLD);
		}
	}else{
		MPI_Status recvGraphStatus;
		char *graphData;
		graphData = malloc(GRAPH_BUFFER_LENGTH * sizeof(MPI_BYTE));
		MPI_Recv(graphData, GRAPH_BUFFER_LENGTH, MPI_BYTE, MPI_ANY_SOURCE, GRAPH_TAG, MPI_COMM_WORLD, &recvGraphStatus);
		int actualLength = -1;
		MPI_Get_count(&recvGraphStatus, MPI_BYTE, &actualLength);
		graph = GDGraphCreateFromData(graphData, actualLength);
	}
	
	explorer = GDExplorerCreate(graph);
	
	
	if(myRank == 0){
		printf("Initializing...\n");
		GDExplorerInitializeWork(explorer);
	}
	//wait until all processes have received graph and p0 has initialized its stack

	if(myRank == 0) printf("processes can now ask for work.\n");
	MPI_Barrier(MPI_COMM_WORLD);
	/*
	 
	 TODO
	 
	 (DONE) 1. Poslat graf vsem ostanim
	 
	 (DONE) 2. rekne vsem ze muzou zahjit vypocet (pozadat prvniho o praci) - nepozadaji p0, pusti se runloop a zacnou zadat nahodne. Zadat na zacatku vsichni p0 by bylo spatne - nejdriv se rozpuli, potom zase rozpuli atd... Pokud chceme efektivnejsi zacatek nez random zadani o praci, musime praci rozdelit uz tady v initialize().
	 
	 */
	
}


//////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma mark - Constants

#define WORK_REQUEST_TAG 1
#define WORK_RESPONSE_TAG 2

#define STACK_BUFFER_LENGTH 1024

#pragma mark - Vars

char * workBuffer;

typedef struct SendWorkData {
	MPI_Request request;
	char *buffer;
} SendWorkData;
typedef SendWorkData* SendWorkDataRef;

void SendWorkDataRelease(SendWorkDataRef data){
	free(data->buffer);
	data->buffer = NULL;
	data->request = NULL;
}



SendWorkData *sentWorks;




#pragma mark - RumLoop

void runLoop() {
	
	//initialize array of SendWorkDataRefs
	sentWorks = malloc(processCount * sizeof(SendWorkData));
	for (int i = 0; i < processCount; i++) {
		sentWorks[i].request = NULL;
		sentWorks[i].buffer = NULL;
	}
	
	workBuffer = malloc(STACK_BUFFER_LENGTH * sizeof(MPI_BYTE));
	
	
	GDBool isMainWorker = myRank == 0;
	
	if ( isMainWorker ){
		printf("Finding best solution...\n");
	}

	while ( YES ) {

	
		GDBool canExistBetterSolution;
		printf("p%d running explorer\n", myRank);
		GDExplorerRun(explorer, EXPLORER_RUN_MAX_STEPS, &canExistBetterSolution);
		GDBool hasWork = explorer->explorationStack->count > 0;
		if ( !hasWork || !canExistBetterSolution ) {
			if ( isMainWorker ) {
				//TODO zacit ukoncovani
			}
			askForWork();
		}
		checkIncomingWorkRequests();
		cleanUpSentWorks();
	}
	
	free(workBuffer);
	free(sentWorks);
	
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

#pragma mark - Termination (Token)


#pragma mark - Check Work Request

void checkIncomingWorkRequests() {
	
	//reads all pending work requests sequentially and responds to each
	int flag = 0;
	do{
		MPI_Status workProbeStatus;
		MPI_Iprobe(MPI_ANY_SOURCE, WORK_REQUEST_TAG, MPI_COMM_WORLD, &flag, &workProbeStatus);
		
		if(flag){
			int length = 0;
			MPI_Status status;
			char dummyReceiveBuffer;
			MPI_Recv(&dummyReceiveBuffer, 1, MPI_CHAR, workProbeStatus.MPI_SOURCE, workProbeStatus.MPI_TAG, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status, MPI_CHAR, &length);
			int source = status.MPI_SOURCE;
			printf("p%d received work request from p%d\n", myRank, source);
			sendWork(source);
		}
	}while(flag);
}

void sendWork(int destination) {
	
	//it is possible (although unlikely), that process 'destination' is asking for more work before we have freed buffer from previous send work request
	SendWorkDataRef previousSendWorkData = &(sentWorks[destination]);
	if(previousSendWorkData){
		SendWorkDataRelease(previousSendWorkData);
		previousSendWorkData = NULL;
	}
	
	
	// do we need unsinged long? Max mpi message length is INT_MAX. Sending larger stacks would require chaining messages or defining a contiguous data type (http://stackoverflow.com/questions/13558861/maximum-amount-of-data-that-can-be-sent-using-mpisend)
	unsigned long length = 0;
	

	SendWorkData sendWorkData;
	GDBool hasWork = GDExplorerGetWork(explorer, &sendWorkData.buffer, &length);
	if(hasWork){
		printf("p%d sending work of length %d to p%d\n", myRank, (int)length, destination);
		MPI_Isend(sendWorkData.buffer, (int)length, MPI_BYTE, destination, WORK_RESPONSE_TAG, MPI_COMM_WORLD, &(sendWorkData.request));
		//save sendWorkData struct for later release;
		sentWorks[destination] = sendWorkData;
	}else{
		printf("p%d doesnt have enough work to share, informing p%d...\n", myRank, destination);
		MPI_Isend(&myRank, 1, MPI_BYTE, destination, WORK_RESPONSE_TAG, MPI_COMM_WORLD, &(sendWorkData.request));
	}
	
	
}


void cleanUpSentWorks() {
	for (int i = 0; i < processCount; i++) {
		SendWorkDataRef data = &(sentWorks[i]);
		if(!(data -> request)) continue; //either we havent sent anything to process 'i' or the send buffer has already been freed
	
		int isRequestFinished;
		MPI_Test(&(data->request), &isRequestFinished, MPI_STATUS_IGNORE);
		if(isRequestFinished){
			printf("p%d releasing buffer for sending data to p%d\n",myRank, i);
			SendWorkDataRelease(data);
			data = NULL;
		}else{
			printf("p%d cant yet release buffer for sending data to p%d, request hasnt finished\n",myRank, i);
		}
	}
}


#pragma mark - Work In

void askForWork() {
	//randomly generate destination process number (different from self)
	//room for optimization - at least dont ask the same process twice in a row
	int destination;
//	do {
//		destination = rand() % processCount;
//	} while (destination == myRank);
	destination = (myRank +1) %processCount;
	
	printf("p%d asking p%d for work.\n", myRank, destination);
	MPI_Send(&myRank, 1, MPI_CHAR, destination, WORK_REQUEST_TAG, MPI_COMM_WORLD);
	//theres no point continuing until i have successfully asked for work
	
	receiveWork();
}

void receiveWork(){
	MPI_Status workRequestStatus;
	//blocking receive - new work or message of length 0
	MPI_Recv(workBuffer, STACK_BUFFER_LENGTH, MPI_BYTE, MPI_ANY_SOURCE, WORK_RESPONSE_TAG, MPI_COMM_WORLD, &workRequestStatus);
	int count = 0;
	MPI_Get_count(&workRequestStatus, MPI_BYTE, &count);
	//no work received, ask someone else
	//TODO this solution might not be consistent with token logic - could lead to a deadlock (everyone asking everyone else for work indefinitelly)
	if(count == 1){
		printf("p%d didnt receive any work from p%d, asking again\n", myRank, workRequestStatus.MPI_SOURCE);
		askForWork();
	}else{
		//		GDExplorationStackRef newStack = GDExplorationStackCreateFromData(workBuffer, count);
		printf("p%d received work of length %d from p%d\n", myRank, count, workRequestStatus.MPI_SOURCE);
		GDExplorerSetWork(explorer, workBuffer, count);
	}
	
}


//////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma mark - Finish

void finalize() {
	
	printf("Finalizing...\n");
	GDSolutionPrint(explorer->bestSolution);
	
	/*
	 
	 TODO
	 
	 1. redukce a sbirani vysledku
	 
	 2. prezentace vysledku
	 
	 */
	
}


#pragma mark - Best Solution Reduction



