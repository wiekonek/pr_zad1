#include <stdio.h>
#include <omp.h>
#include <windows.h>
#include <intrin.h>

#pragma intrinsic(__rdtsc)

int main() {
	omp_set_num_threads(4);
	int i;
	printf("Zad1_2\n");

#pragma omp parallel
	for (i = 0; i < 1; i++) {
		unsigned long long a1, a2;
		a1 = __rdtsc();
		int th_id = omp_get_thread_num();

		HANDLE thread_handle = GetCurrentThread();

		DWORD_PTR mask = (1 << (th_id % 4));
		DWORD_PTR result = SetThreadAffinityMask(thread_handle, mask);

		if (result == 0) printf("blad SetThreadAffnityMask \n");

		else {
			printf("maska poprzednia dla watku %d : %d\n", th_id, result);
			printf("maska nowa dla watku %d : %d\n", th_id, SetThreadAffinityMask(
				thread_handle, mask));
		}

		a2 = __rdtsc();
		printf("%d :: Czas przetwarzania = %llu ns \n", th_id, (a2 - a1) / 3);
	}
}
