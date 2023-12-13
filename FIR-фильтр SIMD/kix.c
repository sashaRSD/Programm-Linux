#include <x86intrin.h>
#include <emmintrin.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

//Разностное уравнение КИХ фильтра:
//out[j] = SUM(H[i] * in[j + i]) i=0 to m
//КИХ фильтр порядка N имеет N + 1 коэффициент.
//Длинна импульсная хар-ки совпадает с кол-вом коэффициентов.

void filter(); //реализация фильтра без SIMD
void simd_filter(); //реализация фильтра с SIMD

int sizeIn = 180; //число отсчетов
int N = 20;	  //Длина фильтра

//коэффициенты фильтра
double H[] = {-0.000000, 0.000357, 0.001565, 0.003931, 0.007688, 0.012719, 
		0.018426, 0.023798, 0.027699, 0.029231, 0.028045, 0.024457, 0.019322,
		0.013737, 0.008690, 0.004800, 0.002235, 0.000807, 0.000166, -0.0000000};


void filter(){
	double in[sizeIn];
	for(int i = 0; i <= sizeIn; i++)
		in[i] = sin(2*M_PI*i);

	clock_t t;
	t = clock();

	//фильтрация 
	double out[sizeIn];
	for(int j = 0; j < sizeIn; j++){
		double s = 0;
		for(int i = 0; i <= N; i++)
			if(i > j) s += H[i] * in[i - j];
			
		out[j] = s;
	}


	t = clock() - t;	
	printf("without SIMD: %li clocks\n", t);
}


void simd_filter(){
	double in[sizeIn];
	for(int i = 0; i <= sizeIn; i++)
		in[i] = sin(2*M_PI*i);

	clock_t t;
	t = clock();
	
	__m128d m_out[sizeIn];
	for(int j = 0; j < sizeIn; j++){
		__m128d s = _mm_set_pd1(0);
		for(int i = 0; i <= N; i++)
			if(i > j){
				__m128d p0= _mm_loadu_pd(&H[i]);
				__m128d p1= _mm_loadu_pd(&in[i - j]);
				s =_mm_add_pd(s, _mm_mul_pd(p0, p1));
			}
			
		m_out[j] = s;
	}


	t = clock() - t;
	printf("------- SIMD: %li clocks\n", t);
}	
	
	
int main(){

	filter();
	simd_filter();
	
	return 0;
}
