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
#include <assert.h>


void waitForPreviousWorkProbeRequest();
void waitForPreviousWorkRequest();
void waitForPreviousSendWorkRequest();
void sendWork(int destination);
void checkIncomingWorkRequests();
void receiveWork();
void askForWork();
void cleanUpSentWorks();
void checkToken();
void terminateWork();
GDBool checkWorkEnd();
void sendToken();
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
#define TOKEN_TAG 3
#define END_WORK_TAG 4

typedef char Color;
#define BLACK 0
#define WHITE 1

GDBool hasToken;
Color tokenColor = WHITE;
Color processColor = WHITE;

GDBool isIdle = NO;

GDBool shouldTerminate = NO;



#pragma mark - Vars


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
	
	
	GDBool isMainWorker = myRank == 0;
	
	if ( isMainWorker ){
		printf("Finding best solution...\n");
		
		hasToken = YES;
	}else{
		hasToken = NO;
	}

	while ( YES ) {

	
		GDBool canExistBetterSolution;
//		printf("p%d running explorer\n", myRank);
		GDExplorerRun(explorer, EXPLORER_RUN_MAX_STEPS, &canExistBetterSolution);
		GDBool hasWork = explorer->explorationStack->count > 0;
		
		if ( !hasWork || !canExistBetterSolution ) {
			
			//found globally best solution, inform others to terminate
			if(!canExistBetterSolution){
				terminateWork();
			}
			if(hasToken){
				sendToken();
			}
			askForWork();
		}
		checkIncomingWorkRequests();
		cleanUpSentWorks();
		checkToken();
		
		if(checkWorkEnd()){
			break;
		}
		
	}
	
//	printf("--------------------------------\n");
//	printf("p%d terminating runLoop\n", myRank);
	
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
//			printf("p%d received work request from p%d\n", myRank, source);
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
//		printf("p%d sending work of length %d to p%d\n", myRank, (int)length, destination);
		MPI_Isend(sendWorkData.buffer, (int)length, MPI_BYTE, destination, WORK_RESPONSE_TAG, MPI_COMM_WORLD, &(sendWorkData.request));
		//save sendWorkData struct for later release;
		sentWorks[destination] = sendWorkData;
		
		//if sending work to lower cpu rank, set own color to BLACK
		if(destination < myRank){
			processColor = BLACK;
		}
	}else{
//		printf("p%d doesnt have enough work to share, informing p%d...\n", myRank, destination);
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
//			printf("p%d releasing buffer for sending data to p%d\n",myRank, i);
			SendWorkDataRelease(data);
			data = NULL;
		}else{
//			printf("p%d cant yet release buffer for sending data to p%d, request hasnt finished\n",myRank, i);
		}
	}
}


#pragma mark - Work In

void askForWork() {
	//randomly generate destination process number (different from self)
	//room for optimization - at least dont ask the same process twice in a row
	int destination;
	destination = (myRank +1) %processCount;
//	do {
//		destination = rand()%processCount;
//	} while (destination == myRank);
	
//	printf("p%d asking p%d for work.\n", myRank, destination);
	MPI_Send(&myRank, 1, MPI_CHAR, destination, WORK_REQUEST_TAG, MPI_COMM_WORLD);
	//theres no point continuing until i have successfully asked for work
	isIdle = YES;
	receiveWork();
}

void receiveWork(){
	
	int isWorkAvailable;
	do {
		checkToken();
		checkWorkEnd();
		checkIncomingWorkRequests();
		if (shouldTerminate) return;
		
		
		MPI_Status workProbeStatus;
		
		MPI_Iprobe(MPI_ANY_SOURCE, WORK_RESPONSE_TAG, MPI_COMM_WORLD, &isWorkAvailable, &workProbeStatus);
		if(isWorkAvailable){
			int count = 0;
			MPI_Get_count(&workProbeStatus, MPI_BYTE, &count);
			//no work received, check work end
			if(count == 1){
				char dummyByte;
				char *dummyBuffer = &dummyByte;
				MPI_Recv(dummyBuffer, count, MPI_BYTE, workProbeStatus.MPI_SOURCE, WORK_RESPONSE_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				if(checkWorkEnd()){
					return;
				}
				//ask someone else
				//				printf("p%d didnt receive any work from p%d, asking again\n", myRank, workRequestStatus.MPI_SOURCE);
				askForWork();
			
			

			}else{
				char *workBuffer = malloc(count * sizeof(char));
				MPI_Status workRequestStatus;
				//blocking receive - new work or message of length 0
				MPI_Recv(workBuffer, count, MPI_BYTE, workProbeStatus.MPI_SOURCE, WORK_RESPONSE_TAG, MPI_COMM_WORLD, &workRequestStatus);
				
				
				GDExplorerSetWork(explorer, workBuffer, count);
				free(workBuffer);
			}
			
		}
		
	} while (!isWorkAvailable);
	
	
}

#pragma mark - Token

void terminateWork(){
	shouldTerminate = YES;
	printf("----------------------------------------------------------------\n");
	printf ("p%d decided to terminate work, informing others..\n", myRank);
	for (int i = 0; i < processCount; i++){
		if(i == myRank) continue;
		MPI_Send(&myRank, 1, MPI_INT, i, END_WORK_TAG, MPI_COMM_WORLD);
	}
	
}

void checkToken() {
	MPI_Status tokenProbeStatus;
	int isTokenIncoming;
	MPI_Iprobe(MPI_ANY_SOURCE, TOKEN_TAG, MPI_COMM_WORLD, &isTokenIncoming, &tokenProbeStatus);
	if(isTokenIncoming){
		MPI_Status tokenReceiveStatus;
		MPI_Recv(&tokenColor, 1, MPI_BYTE, tokenProbeStatus.MPI_SOURCE, TOKEN_TAG, MPI_COMM_WORLD, &tokenReceiveStatus);
		
//			printf("p%d received token of color %s from p%d\n", myRank, (tokenColor == WHITE)? "white" : "black", tokenReceiveStatus.MPI_SOURCE);
		hasToken = YES;
		
		GDBool isMainWorker = myRank == 0;
		if(isMainWorker && tokenColor == WHITE){
			terminateWork();
			return;
		}else if (processColor == BLACK){
			tokenColor = BLACK;
		}
		
		if(isIdle){
			sendToken();
		}
		
	}
}

void sendToken(){
	assert(hasToken);
	GDBool isMainWorker = myRank == 0;
	int destination = (myRank + 1) % processCount;
	
	if(isMainWorker){
		tokenColor = WHITE;
	}
//	printf("p%d sending token of color %s to p%d\n", myRank, (tokenColor == WHITE)? "white" : "black", destination);
	MPI_Send(&tokenColor, 1, MPI_BYTE, destination, TOKEN_TAG, MPI_COMM_WORLD);
	hasToken = NO;
	processColor = WHITE;
}

GDBool checkWorkEnd(){

	if(shouldTerminate) return YES;
	int flag;
	MPI_Iprobe(MPI_ANY_SOURCE, END_WORK_TAG, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
	shouldTerminate = flag;
	
	return  shouldTerminate;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma mark - Finish

#define TRIANGLE_LIST_BUFFER_LENGTH 1024

#define SOLUTION_TAG 5

void finalize() {
	
		
	//receive all solutions, print best
	
	GDBool isMainWorker = myRank == 0;
	if(isMainWorker){
		printf("Finalizing...\n");
		GDTriangleListRef *solutionTriangleLists = malloc((processCount -1) *sizeof(GDTriangleListRef));
//		solutionTriangleLists[0] = explorer->bestSolution->triangleList;
		for (int i = 1; i < processCount; i++) {
			char *triangleListData = malloc(TRIANGLE_LIST_BUFFER_LENGTH *sizeof(char));
			MPI_Status solutionStatus;
			MPI_Recv(triangleListData, TRIANGLE_LIST_BUFFER_LENGTH, MPI_BYTE, i, SOLUTION_TAG, MPI_COMM_WORLD, &solutionStatus);
			int actualLength = -1;
			MPI_Get_count(&solutionStatus, MPI_BYTE, &actualLength);
			
//			printf("p%d received triangle list data of length %d from p%d\n", myRank, actualLength, solutionStatus.MPI_SOURCE);
			

			

			
			if(actualLength > 1){
				GDTriangleListRef triangleList = GDTriangleListCreateFromData(triangleListData, actualLength);
				solutionTriangleLists[i-1] = triangleList;
			}else{
				solutionTriangleLists[i-1] = NULL;
			}
		}
		GDTriangleListRef bestTriangleList = explorer->bestSolution->triangleList;
		for(int i = 0; i < processCount-1; i++){
			GDTriangleListRef triangleList = solutionTriangleLists[i];
			if(triangleList && triangleList->count > bestTriangleList->count){
				bestTriangleList = triangleList;
			}
		}
		GDSolutionRef bestSolution ;
		if(bestTriangleList == explorer->bestSolution->triangleList){
			bestSolution = explorer->bestSolution;
		}else{
			bestSolution = GDSolutionCreate(graph, bestTriangleList);
		}
		
		
		GDSolutionPrint(bestSolution);
		
		for(int i = 0; i < processCount -1; i++){
			GDTriangleListRef triangleList = solutionTriangleLists[i];
			if(triangleList){
				GDTriangleListRelease(triangleList);
			}
		}
		free(solutionTriangleLists);

	}else{
		if(explorer->bestSolution){
			GDTriangleListRef	bestLocalTriangleList= explorer->bestSolution->triangleList;
			char *bestLocalTriangleListData;
			unsigned long length;
			GDTriangleListGetData(bestLocalTriangleList, &bestLocalTriangleListData, &length);
//			printf("p%d sending triangle list data of length %d to p%d\n", myRank, (int)length, 0);
			MPI_Send(bestLocalTriangleListData, (int)length, MPI_BYTE, 0, SOLUTION_TAG, MPI_COMM_WORLD);
			free(bestLocalTriangleListData);
		}else{
			MPI_Send(&myRank, 1, MPI_BYTE, 0, SOLUTION_TAG, MPI_COMM_WORLD);
		}
	}
	
	
}


#pragma mark - Best Solution Reduction



