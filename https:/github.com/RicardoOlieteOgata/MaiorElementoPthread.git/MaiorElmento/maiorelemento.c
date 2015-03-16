#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)

int *vetor;
int vetorSize;
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

int main(int argc, char *argv[]) {
	struct thread_info *tinfo;
	int i, tam, maior;
	srand((unsigned) time(NULL));

	qtdThreads = atoi(argv[1]);
	vetorSize = atoi(argv[2]);

	if (qtdThreads < 1)
		handle_error("threads menor que 1");
	if (vetorSize < 1 || vetorSize > 500)
		handle_error("tamanho do vetor invalido");

	tam = vetorSize / qtdThreads;

	vetor = (int *) malloc(vetorSize * sizeof(int));
	maiorVetor = (int *) malloc(qtdThreads * sizeof(int));

	for (i = 0; i < vetorSize; i++)
		vetor[i] = rand() % vetorSize + 1;

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

	maior = maiorVetor[0];
	for (i = 1; i < qtdThreads; i++)
		if (maiorVetor[i] > maior)
			maior = maiorVetor[i];
	printf("Maior = %d \n", maior);

	free(tinfo);
	free(vetor);

	exit(EXIT_SUCCESS);
}

