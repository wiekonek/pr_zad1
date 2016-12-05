#include <stdio.h>
#include <omp.h>

int main() {
	omp_set_num_threads(2);
	int i;
	printf("Zad1_1\n");
#pragma omp parallel
	for (i = 0; i<6; i++)
		printf("%d :: Iter:%d\n", omp_get_thread_num(), i);

	printf("GoodBye World\n");
}