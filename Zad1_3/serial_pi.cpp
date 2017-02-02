#include <stdio.h>
#include <time.h>
#include <omp.h>

#define MILIARD 1000000000
#define STO_MILIONOW 100000000


long long num_steps = MILIARD;
int thread_no = 4;
double step;

void clean() {
	num_steps = MILIARD;
	clock_t start, stop;
	double x, pi, sum = 0.0;
	int i;
	step = 1. / (double)num_steps;
	start = clock();

	for (i = 0; i<num_steps; i++) {
		x = (i + .5)*step;
		sum = sum + 4.0 / (1. + x*x);
	}

	pi = sum*step;
	stop = clock();

	printf("Wartosc liczby PI wynosi %15.12f\n", pi);
	printf("Czas przetwarzania wynosi %f sekund\n", ((double)(stop - start) / 1000.0));
}

void atomic() {
	num_steps = STO_MILIONOW;
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
	num_steps = MILIARD;
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

	printf("Threads no. : %d \n", thread_no);
	printf("Wartosc liczby PI wynosi %15.12f\n", pi);
	printf("Czas przetwarzania wynosi %f sekund\n", ((double)(stop - start) / 1000.0));
}

void partial_sum() {
	num_steps = MILIARD;
	clock_t start, stop;
	double x, pi, sum = 0.0;
	int i;
	volatile double * sum_tab = new double[thread_no];
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

void partial_sum_eks_di(int offset) {
	thread_no = 2;
	omp_set_num_threads(thread_no);
	num_steps = 2 * STO_MILIONOW;
	clock_t start, stop;
	double x, pi, sum = 0.0;
	int i;
	volatile double sum_tab[100];
	step = 1. / (double)num_steps;
	start = clock();

#pragma omp parallel for
	for (i = 0; i<num_steps; i++) {
		x = (i + .5)*step;
		sum_tab[omp_get_thread_num() + offset] += 4.0 / (1. + x*x);
	}

	for (i = 0; i < thread_no; i++) {
		sum += sum_tab[i];
	}

	pi = sum*step;
	stop = clock();

	printf("Threads no. : %d \n", thread_no);
	printf("Offset. : %d \n", offset);
	printf("Wartosc liczby PI wynosi %15.12f\n", pi);
	printf("Czas przetwarzania wynosi %f sekund\n\n", ((double)(stop - start) / 1000.0));
}

void zad8(int threadNo) {
	omp_set_num_threads(threadNo);
	num_steps = MILIARD;
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

	printf("Threads no. : %d \n", threadNo);
	printf("Wartosc liczby PI wynosi %15.12f\n", pi);
	printf("Czas przetwarzania wynosi %f sekund\n", ((double)(stop - start) / 1000.0));
}

int main(int argc, char* argv[]) {
	thread_no = omp_get_max_threads();
	omp_set_num_threads(thread_no);

	//atomic();
	//reduction();
	//for (int i = 0; i < 2; i++) {
	//	for (int o = 0; o < 20; o++) {
	//		partial_sum_eks_di(o);
	//	}
	//}

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
// Threads no. : 4
// Wartosc liczby PI wynosi  3.141745762011
// Czas przetwarzania wynosi 19.380000 sekund
//
// volatile
// Threads no. : 4
// Wartosc liczby PI wynosi  3.141550914210
// Czas przetwarzania wynosi 19.018000 sekund
	////
	//Threads no. : 2
	//Offset. : 0
	//Wartosc liczby PI wynosi  3.141080899888
	//Czas przetwarzania wynosi 3.149000 sekund

	//Threads no. : 2
	//Offset. : 1
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 3.131000 sekund

	//Threads no. : 2
	//Offset. : 2
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 3.151000 sekund

	//Threads no. : 2
	//Offset. : 3
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 3.284000 sekund

	//Threads no. : 2
	//Offset. : 4
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 4.200000 sekund

	//Threads no. : 2
	//Offset. : 5
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 4.619000 sekund

	//Threads no. : 2
	//Offset. : 6
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 5.028000 sekund

	//Threads no. : 2
	//Offset. : 7
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 4.989000 sekund

	//Threads no. : 2
	//Offset. : 8
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 5.197000 sekund

	//Threads no. : 2
	//Offset. : 9
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 5.068000 sekund

	//Threads no. : 2
	//Offset. : 10
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 4.873000 sekund

	//Threads no. : 2
	//Offset. : 11
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 3.258000 sekund

	//Threads no. : 2
	//Offset. : 12
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 5.135000 sekund

	//Threads no. : 2
	//Offset. : 13
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 5.101000 sekund

	//Threads no. : 2
	//Offset. : 14
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 5.067000 sekund

	//Threads no. : 2
	//Offset. : 15
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 5.024000 sekund

	//Threads no. : 2
	//Offset. : 16
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 5.192000 sekund

	//Threads no. : 2
	//Offset. : 17
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 5.024000 sekund

	//Threads no. : 2
	//Offset. : 18
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 5.081000 sekund

	//Threads no. : 2
	//Offset. : 19
	//Wartosc liczby PI wynosi  4.953897094141
	//Czas przetwarzania wynosi 3.446000 sekund

	//Threads no. : 2
	//Offset. : 0
	//Wartosc liczby PI wynosi  8.095734222101
	//Czas przetwarzania wynosi 3.158000 sekund

	//Threads no. : 2
	//Offset. : 1
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 3.132000 sekund

	//Threads no. : 2
	//Offset. : 2
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 3.119000 sekund

	//Threads no. : 2
	//Offset. : 3
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 3.694000 sekund

	//Threads no. : 2
	//Offset. : 4
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 4.931000 sekund

	//Threads no. : 2
	//Offset. : 5
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 5.331000 sekund

	//Threads no. : 2
	//Offset. : 6
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 5.147000 sekund

	//Threads no. : 2
	//Offset. : 7
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 5.272000 sekund

	//Threads no. : 2
	//Offset. : 8
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 5.111000 sekund

	//Threads no. : 2
	//Offset. : 9
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 5.184000 sekund

	//Threads no. : 2
	//Offset. : 10
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 4.952000 sekund

	//Threads no. : 2
	//Offset. : 11
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 3.356000 sekund

	//Threads no. : 2
	//Offset. : 12
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 5.155000 sekund

	//Threads no. : 2
	//Offset. : 13
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 4.772000 sekund

	//Threads no. : 2
	//Offset. : 14
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 4.683000 sekund

	//Threads no. : 2
	//Offset. : 15
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 4.636000 sekund

	//Threads no. : 2
	//Offset. : 16
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 4.997000 sekund

	//Threads no. : 2
	//Offset. : 17
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 4.201000 sekund

	//Threads no. : 2
	//Offset. : 18
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 4.783000 sekund

	//Threads no. : 2
	//Offset. : 19
	//Wartosc liczby PI wynosi  9.936259465580
	//Czas przetwarzania wynosi 3.098000 sekund