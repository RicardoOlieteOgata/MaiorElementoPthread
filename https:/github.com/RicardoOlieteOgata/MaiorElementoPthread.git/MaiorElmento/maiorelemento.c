#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>

#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)

int *vetor;
int vetorSize = 4000000;
int *maiorVetor;
int qtdThreads;

struct thread_info {
	pthread_t thread_id;
	int thread_num;
	int tam;
};

static void *
thread_busca_maior(void *arg) {
	int i, maiorParcial, inicio, fim, tam;
	struct thread_info *tinfo = arg;

	inicio = (tinfo->thread_num) * (tinfo->tam);
	if (tinfo->thread_num + 1 == qtdThreads)
		fim = vetorSize;
	else
		fim = inicio + tinfo->tam;

	maiorParcial = vetor[inicio++];

	for (i = inicio; i < fim; i++)
		if (vetor[i] > maiorParcial)
			maiorParcial = vetor[i];

	maiorVetor[tinfo->thread_num] = maiorParcial;
}

int maiorElemento(int *vetor, int tam) {
	int maior, i;
	maior = vetor[0];
	for (i = 1; i < tam; i++)
		if (vetor[i] > maior)
			maior = vetor[i];
	return maior;
}

double parte_serial(int threadNum) {
	struct thread_info *tinfo;
	double start_t, end_t;
	struct timeval tv;
	int tam, i, maior;
	gettimeofday(&tv, NULL);
	start_t = (double) tv.tv_sec + (double) tv.tv_usec / 1000000.0;
	qtdThreads = threadNum;
	tam = vetorSize / qtdThreads;

	maiorVetor = (int *) malloc(qtdThreads * sizeof(int));

	tinfo = calloc(qtdThreads, sizeof(struct thread_info));

	for (i = 0; i < qtdThreads; i++) {
		tinfo[i].thread_num = i;
		tinfo[i].tam = tam;
		pthread_create(&tinfo[i].thread_id, NULL, &thread_busca_maior,
				&tinfo[i]);
	}

	for (i = 0; i < qtdThreads; i++) {
		pthread_join(tinfo[i].thread_id, NULL);
	}

	maior = maiorElemento(maiorVetor, qtdThreads);

	free(tinfo);
	free(maiorVetor);

	gettimeofday(&tv, NULL);
	end_t = (double) tv.tv_sec + (double) tv.tv_usec / 1000000.0;
	return end_t - start_t;

}

void quickSort(int *original, int *randomica, int inicio, int fim) {
	int meio;
	if (inicio < fim) {
		meio = separa(original, randomica, inicio, fim);
		quickSort(original, randomica, inicio, meio);
		quickSort(original, randomica, meio + 1, fim);
	}
}

int separa(int *original, int *randomica, int inicio, int fim) {
	int pivo, topo, pivoNumeros, i;
	pivo = randomica[inicio];
	pivoNumeros = original[inicio];
	topo = inicio;
	for (i = inicio + 1; i <= fim; i++)
		if (randomica[i] < pivo) {
			randomica[topo] = randomica[i];
			original[topo] = original[i];
			topo++;
			randomica[i] = randomica[topo];
			original[i] = original[topo];
		}
	randomica[topo] = pivo;
	original[topo] = pivoNumeros;
	return topo;
}

int main(int argc, char *argv[]) {
	int i, maior, teste = 0;
	int *randomica;
	struct timeval tv;
	double start_t, end_t, tempo_gasto[6];

	srand((unsigned) time(NULL));

	vetor = (int *) malloc(vetorSize * sizeof(int));
	randomica = (int *) malloc(vetorSize * sizeof(int));

	for (i = 0; i < vetorSize; i++) {
		randomica[i] = rand();
		vetor[i] = i;
	}

	quickSort(vetor, randomica, 0, vetorSize - 1);

	//serial

	gettimeofday(&tv, NULL);
	start_t = (double) tv.tv_sec + (double) tv.tv_usec / 1000000.0;

	maior = maiorElemento(vetor, vetorSize);

	gettimeofday(&tv, NULL);
	end_t = (double) tv.tv_sec + (double) tv.tv_usec / 1000000.0;
	tempo_gasto[teste++] = end_t - start_t;

	//2 threads
	tempo_gasto[teste++] = parte_serial(2);

	//4 threads
	tempo_gasto[teste++] = parte_serial(4);

	//8 threads
	tempo_gasto[teste++] = parte_serial(8);

	//16 threads
	tempo_gasto[teste++] = parte_serial(16);

	//24 threads
	tempo_gasto[teste++] = parte_serial(24);

	teste = 0;
	printf("serial      = %f x mais rapido que o serial\n",
			tempo_gasto[0] / tempo_gasto[teste]);
	printf("02 threads  = %f x mais rapido que o serial\n",
			tempo_gasto[0] / tempo_gasto[++teste]);
	printf("04 threads  = %f x mais rapido que o serial\n",
			tempo_gasto[0] / tempo_gasto[++teste]);
	printf("08 threads  = %f x mais rapido que o serial\n",
			tempo_gasto[0] / tempo_gasto[++teste]);
	printf("16 threads  = %f x mais rapido que o serial\n",
			tempo_gasto[0] / tempo_gasto[++teste]);
	printf("24 threads  = %f x mais rapido que o serial\n",
			tempo_gasto[0] / tempo_gasto[++teste]);

	free(vetor);
	free(randomica);

	exit(EXIT_SUCCESS);
}

