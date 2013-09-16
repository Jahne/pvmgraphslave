/*slave.c*/
#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>
#include <time.h>
#include "dijkstra.h"

int **cost; //graf
int max; //liczba wierzcholkow grafu
int infinite; //oznacza brak powiazania
int startVertex; //wierzcholek startowy
int *graph;

double getTime() {
	struct timeval t;
	struct timezone tzp;
	gettimeofday(&t, &tzp);
	return t.tv_sec + t.tv_usec * 1e-6;
}

void printTab(int* tab[], int size) {
	int row, col;
	for (col = 0; col < size; ++col) {
		for (row = 0; row < size; ++row) {
			printf("%d", tab[col][row]);
			if (row != size - 1)
				printf(", ");
		}
		puts("");
	}

}

void recvInitData(int ip) {

	pvm_recv(ip, 10);

	pvm_upkint(&max, 1, 1);
	pvm_upkint(&infinite, 1, 1);

	graph = malloc(max * max * sizeof(int));
	pvm_upkint(graph, max * max, 1);
}

int recvStartVertex(int ip) {
	int startVertex = -1;

	pvm_recv(ip, 7);
	pvm_upkint(&startVertex, 1, 1);

	return startVertex;
}

void rewrideArrays(int length) {
	int i, j = 0;

	cost = malloc(sizeof *cost * length);

	if (cost) {
		for (i = 0; i < length; i++) {
			cost[i] = malloc(sizeof *cost[i] * length);
		}
	}

	for (i = 0; i < length; i++) {
		for (j = 0; j < length; j++) {
			cost[i][j] = graph[i * (length - 1) + j];
		}
	}

	free(graph);
}

void printGraph(int max) {
	int i, j = 0;

	puts("graf graph");
	for (i = 0; i < max; i++) {
		for (j = 0; j < max; j++) {
			printf(" %d,", graph[i * (max - 1) + j]);
		}
		printf("\n");
	}

	for (i = 0; i < max * max; i++) {
		printf(" %d,", graph[i]);
	}
}

void printGraph2(int max) {
	int i, j = 0;

	puts("graf cost");
	for (i = 0; i < max; i++) {
		for (j = 0; j < max; j++) {
			printf(" %d,", cost[i][j]);
		}
		printf("\n");
	}
}

int main() {
	int myId, parentId;
//	pvm_catchout(stdout);

	myId = pvm_mytid();
//	printf("[%x] startuje\n", myId);

	if (PvmNoParent == (parentId = pvm_parent())) {
		printf("Program ten powinien być uruchomiony z programu głównego!");
	} else {
		int suma = 0;
		double totalTime = 0, computingTime, startTime = 0;
		//odebranie tablicy grafu i inicjalizacja wszystkich potrzebnych zmiennych
//		printf("odbieram graf");
		recvInitData(parentId);
//		printf("odebralem graf");
//		printGraph(max);
		rewrideArrays(max);
		int *preced;
		int *distance;
		preced = malloc(max * sizeof(int));
		distance = malloc(max * sizeof(int));

		while (1) {
			int suma = 0;
			double totalTime = 0, computingTime, startTime = 0;

			memset(preced, 0, sizeof(int) * max);
			memset(distance, 0, sizeof(int) * max);

			startTime = getTime();
			startVertex = recvStartVertex(parentId);
			if (startVertex < 0)
				break;

			computingTime = getTime();
//			puts("wyznaczam sume najkrotszych sciezek");
			shortpath(cost, max, startVertex, infinite, preced, distance);
//			puts("wyznaczone sumy");
			computingTime = getTime() - computingTime;

			//zliczanie sumy najkrotszych sciezek z danego wierzcholka
			int i;
			for (i = 0; i < max; i++)
				suma += distance[i];

//			printf("pid= %d, suma= %d, ", myId, suma);
//			printf("computingTime = %f", computingTime);

			totalTime = getTime() - startTime;
//			printf("totalTime= %f\n", totalTime);

			//wysylanie wynikow
			pvm_initsend(PvmDataDefault);
			pvm_pkint(&startVertex, 1, 1);
			pvm_pkint(&suma, 1, 1);
			pvm_pkdouble(&computingTime, 1, 1);
			pvm_pkdouble(&totalTime, 1, 1);
			pvm_send(parentId, 5);
		}

	}
	pvm_exit();
	exit(0);
}
