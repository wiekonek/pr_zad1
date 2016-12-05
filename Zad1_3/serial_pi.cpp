#include <stdio.h>
#include <time.h>
#include <omp.h>

long long num_steps = 1000000000;
int thread_no = 4;
double step;

void atomic() {
	num_steps = 100000000;
	clock_t start, stop;
	double x, pi, sum = 0.0;
	int i;
	step = 1. / (double)num_steps;
	start = clock();
#pragma omp parallel for
	for (i = 0; i<num_steps; i++) {
		x = (i + .5)*step;
#pragma omp atomic
		sum += 4.0 / (1. + x*x);
	}

	pi = sum*step;
	stop = clock();

	printf("Wartosc liczby PI wynosi %15.12f\n", pi);
	printf("Czas przetwarzania wynosi %f sekund\n", ((double)(stop - start) / 1000.0));
}

void reduction() {
	num_steps = 1000000000;
	clock_t start, stop;
	double x, pi, sum = 0.0;
	int i;
	step = 1. / (double)num_steps;
	start = clock();

#pragma omp parallel for reduction(+:sum)
	for (i = 0; i<num_steps; i++) {
		x = (i + .5)*step;
		sum += 4.0 / (1. + x*x);
	}

	pi = sum*step;
	stop = clock();

	printf("Wartosc liczby PI wynosi %15.12f\n", pi);
	printf("Czas przetwarzania wynosi %f sekund\n", ((double)(stop - start) / 1000.0));
}

void partial_sum() {
	num_steps = 1000000000;
	clock_t start, stop;
	double x, pi, sum = 0.0;
	int i;
	double * sum_tab = new double[thread_no];
	step = 1. / (double)num_steps;
	start = clock();

#pragma omp parallel for
	for (i = 0; i<num_steps; i++) {
		x = (i + .5)*step;
		sum_tab[omp_get_thread_num()] += 4.0 / (1. + x*x);
	}

	for (i = 0; i < thread_no; i++) {
		sum += sum_tab[i];
	}

	pi = sum*step;
	stop = clock();

	printf("Threads no. : %d \n", thread_no);
	printf("Wartosc liczby PI wynosi %15.12f\n", pi);
	printf("Czas przetwarzania wynosi %f sekund\n", ((double)(stop - start) / 1000.0));
}

int main(int argc, char* argv[]) {
	thread_no = omp_get_max_threads();
	omp_set_num_threads(thread_no);

	//atomic();
	//reduction();
	partial_sum();
	return 0;
}

// Sekwencyjnie:
// Wartosc liczby PI wynosi  3.141592653590
// Czas przetwarzania wynosi 5.519000 sekund

// parallel for (1 000 000 000);
// Wartosc liczby PI wynosi  1.105530989547
// Czas przetwarzania wynosi 5.226000 sekund

// atomic (100 000 000)
// Wartosc liczby PI wynosi  3.141592653590
// Czas przetwarzania wynosi 14.013000 sekund


// reduction (100 000 000)
// omp tworzy kopie zmiennych sum i na koniec sumuje je ze sob¹
// Wartosc liczby PI wynosi  3.141592653590
// Czas przetwarzania wynosi 0.373000 sekund
//
// (1 000 000 000)
// Wartosc liczby PI wynosi  3.141592653590
// Czas przetwarzania wynosi 3.526000 sekund

// sumy czêsciowe (1 000 000 000)
// bez volatile 
// Wartosc liczby PI wynosi  0.976557038046
// Czas przetwarzania wynosi 20.238000 sekund