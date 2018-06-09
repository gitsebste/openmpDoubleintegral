// integral.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <cmath>
#include <ctime>
#include <chrono>
#include <omp.h>

using namespace std;

void waitSomeTime(double t) {/*
							 chrono::steady_clock sc; // create an object of `steady_clock` class
							 auto start = sc.now(); // start timer
							 // #pragma omp parallel for
							 for (int i = 0;; i++) {
							 auto end = sc.now();
							 auto time_span = static_cast<chrono::duration<double>>(end - start); // measure time span between start & end
							 if (time_span.count() * 100000>t)break;

							 }
							 */
}

class My {
public:
	double a_arr[100], b_arr[100], c_arr[100];

	void integrateParallel1(double a, double b, double ay, double by, int ki, int kf) {
		double st = omp_get_wtime();
		//cout << "Calka I(k) z " << "f(x)=cos(xyk^0.5)/(1+x^2+y^2)" << " od a = " << a << " do b = " << b << endl;
		int numOfPartsX = (int)((b - a) * 3000);
		double dx = (b - a) / (numOfPartsX);

		int numOfPartsY = (int)((by - ay) * 3000);
		double dy = (by - ay) / (numOfPartsY);

		double sum = 0;  int i = 0; int j = 0; int k;
		//#pragma omp parallel for private(k)  reduction(+:sum)
#pragma omp parallel for private(k,j,i) schedule(dynamic) reduction(+:sum)
		for (k = kf; k >= ki; k--)
		{
			sum = 0;
			//#pragma omp parallel for private(j)  reduction(+:sum)
			for (j = 0; j<numOfPartsY; j++)
			{
				//#pragma omp parallel for private(i)  reduction(+:sum)
				for (i = 0; i<numOfPartsX; i++)
					sum += fun(a + i*dx + dx / 2, ay + j*dy + dy / 2, k)*dx*dy;
			}
			b_arr[k - 1] = sum;
		}
		double en = omp_get_wtime();
		printf("Parallel1 %lf\n", en - st);
	}
	void integrateParallel2(double a, double b, double ay, double by, int ki, int kf) {
		//cout << "Calka I(k) z " << "f(x)=cos(xyk^0.5)/(1+x^2+y^2)" << " od a = " << a << " do b = " << b << endl;
		double st = omp_get_wtime();
		int numOfPartsX = (int)((b - a) * 3000);
		double dx = (b - a) / (numOfPartsX);

		int numOfPartsY = (int)((by - ay) * 3000);
		double dy = (by - ay) / (numOfPartsY);

		double sum = 0;  int i = 0; int j = 0; int k;
#pragma omp parallel for private(k,j,i) schedule(static) reduction(+:sum)
		for (k = kf; k >= ki; k--)
		{
			sum = 0;
			for (j = 0; j<numOfPartsY; j++)
			{
				for (i = 0; i<numOfPartsX; i++)
					sum += fun(a + i*dx + dx / 2, ay + j*dy + dy / 2, k)*dx*dy;
			}
			c_arr[k - 1] = sum;
		}
		double en = omp_get_wtime();
		printf("Parallel2 %lf\n", en - st);
	}
	void integrateNoParallelism(double a, double b, double ay, double by, int ki, int kf) {
		//cout << "Calka I(k) z " << "f(x)=cos(xyk^0.5)/(1+x^2+y^2)" << " od a = " << a << " do b = " << b << endl;
		double st = omp_get_wtime();
		int numOfPartsX = (int)((b - a) * 3000);
		double dx = (b - a) / (numOfPartsX);

		int numOfPartsY = (int)((by - ay) * 3000);
		double dy = (by - ay) / (numOfPartsY);

		double sum = 0;  int i = 0; int j = 0;
		for (int k = kf; k >= ki; k--)
		{
			sum = 0;
			for (j = 0; j<numOfPartsY; j++)
			{
				for (i = 0; i<numOfPartsX; i++)
					sum += fun(a + i*dx + dx / 2, ay + j*dy + dy / 2, k)*dx*dy;
			}
			a_arr[k - 1] = sum;
		}
		double en = omp_get_wtime();
		printf("NoParalleism %lf\n", en - st);
	}

	double fun(double x, double y, int k)
	{
		return cos(x*y*pow(k, 0.5)) / (1 + x*x + y*y);
	}
	void check(int n) {
		bool ok = true;
		for (int i = 0; i < n; i++) {
			if (!(a_arr[i] == b_arr[i] && a_arr[i] == c_arr[i])) { ok = false; break; }
			printf("%f\t%f\t%f\n", a_arr[i], b_arr[i], c_arr[i]);
		}
		if (ok)printf("OK.\n"); else printf("NOT OK.\n");
	}
};

void lockScreen()
{
	char c;
	cin >> c;
}



int main() {
	My m; int k_min = 1; int k_max = 2;
	m.integrateNoParallelism(1, 1.5, 0, 1, k_min, k_max); m.integrateParallel1(1, 1.5, 0, 1, k_min, k_max); m.integrateParallel2(1, 1.5, 0, 1, k_min, k_max);
	m.check(k_max);
	lockScreen();

}
/*
k=1		0.137862	http://www.wolframalpha.com/input/?i=integrate+cos(x*y*1%5E0.5)%2F(1%2Bx%5E2%2By%5E2)+dxdy,+x%3D1..1.5,+y%3D0..1
k=2		0.105488	http://www.wolframalpha.com/input/?i=integrate+cos(x*y*2%5E0.5)%2F(1%2Bx%5E2%2By%5E2)+dxdy,+x%3D1..1.5,+y%3D0..1
k=3		0.0781729	http://www.wolframalpha.com/input/?i=integrate+cos(x*y*3%5E0.5)%2F(1%2Bx%5E2%2By%5E2)+dxdy,+x%3D1..1.5,+y%3D0..1
k=4		0.0553134	http://www.wolframalpha.com/input/?i=integrate+cos(x*y*4%5E0.5)%2F(1%2Bx%5E2%2By%5E2)+dxdy,+x%3D1..1.5,+y%3D0..1
k=5		0.0363662	http://www.wolframalpha.com/input/?i=integrate+cos(x*y*5%5E0.5)%2F(1%2Bx%5E2%2By%5E2)+dxdy,+x%3D1..1.5,+y%3D0..1
k=6		0.0208417	http://www.wolframalpha.com/input/?i=integrate+cos(x*y*6%5E0.5)%2F(1%2Bx%5E2%2By%5E2)+dxdy,+x%3D1..1.5,+y%3D0..1
k=7		0.00829944	http://www.wolframalpha.com/input/?i=integrate+cos(x*y*7%5E0.5)%2F(1%2Bx%5E2%2By%5E2)+dxdy,+x%3D1..1.5,+y%3D0..1
k=8		-0.00165624	http://www.wolframalpha.com/input/?i=integrate+cos(x*y*8%5E0.5)%2F(1%2Bx%5E2%2By%5E2)+dxdy,+x%3D1..1.5,+y%3D0..1
k=9		-0.00937962	http://www.wolframalpha.com/input/?i=integrate+cos(x*y*9%5E0.5)%2F(1%2Bx%5E2%2By%5E2)+dxdy,+x%3D1..1.5,+y%3D0..1
k=10	-0.0151877	http://www.wolframalpha.com/input/?i=integrate+cos(x*y*10%5E0.5)%2F(1%2Bx%5E2%2By%5E2)+dxdy,+x%3D1..1.5,+y%3D0..1
.
.
.
k=100	-0.00204909 http://www.wolframalpha.com/input/?i=integrate+cos(x*y*100%5E0.5)%2F(1%2Bx%5E2%2By%5E2)+dxdy,+x%3D1..1.5,+y%3D0..1

NoParalleism 68.832490
Parallel1 23.125618
Parallel2 23.494542
0.137862        0.137862        0.137862
0.105488        0.105488        0.105488
0.078173        0.078173        0.078173
0.055313        0.055313        0.055313
0.036366        0.036366        0.036366
0.020842        0.020842        0.020842
0.008299        0.008299        0.008299
-0.001656       -0.001656       -0.001656
-0.009380       -0.009380       -0.009380
-0.015187       -0.015187       -0.015187
-0.019362       -0.019362       -0.019362
-0.022155       -0.022155       -0.022155
-0.023789       -0.023789       -0.023789
-0.024460       -0.024460       -0.024460
-0.024344       -0.024344       -0.024344
-0.023590       -0.023590       -0.023590
-0.022334       -0.022334       -0.022334
-0.020691       -0.020691       -0.020691
-0.018762       -0.018762       -0.018762
-0.016632       -0.016632       -0.016632
-0.014375       -0.014375       -0.014375
-0.012055       -0.012055       -0.012055
-0.009724       -0.009724       -0.009724
-0.007425       -0.007425       -0.007425
-0.005194       -0.005194       -0.005194
-0.003061       -0.003061       -0.003061
-0.001047       -0.001047       -0.001047
0.000829        0.000829        0.000829
0.002556        0.002556        0.002556
0.004124        0.004124        0.004124
0.005528        0.005528        0.005528
0.006767        0.006767        0.006767
0.007841        0.007841        0.007841
0.008752        0.008752        0.008752
0.009506        0.009506        0.009506
0.010107        0.010107        0.010107
0.010563        0.010563        0.010563
0.010882        0.010882        0.010882
0.011072        0.011072        0.011072
0.011144        0.011144        0.011144
0.011105        0.011105        0.011105
0.010965        0.010965        0.010965
0.010736        0.010736        0.010736
0.010425        0.010425        0.010425
0.010042        0.010042        0.010042
0.009598        0.009598        0.009598
0.009099        0.009099        0.009099
0.008556        0.008556        0.008556
0.007976        0.007976        0.007976
0.007366        0.007366        0.007366
0.006735        0.006735        0.006735
0.006088        0.006088        0.006088
0.005433        0.005433        0.005433
0.004774        0.004774        0.004774
0.004117        0.004117        0.004117
0.003467        0.003467        0.003467
0.002829        0.002829        0.002829
0.002205        0.002205        0.002205
0.001600        0.001600        0.001600
0.001016        0.001016        0.001016
0.000457        0.000457        0.000457
-0.000077       -0.000077       -0.000077
-0.000582       -0.000582       -0.000582
-0.001058       -0.001058       -0.001058
-0.001503       -0.001503       -0.001503
-0.001917       -0.001917       -0.001917
-0.002299       -0.002299       -0.002299
-0.002649       -0.002649       -0.002649
-0.002966       -0.002966       -0.002966
-0.003252       -0.003252       -0.003252
-0.003505       -0.003505       -0.003505
-0.003728       -0.003728       -0.003728
-0.003920       -0.003920       -0.003920
-0.004083       -0.004083       -0.004083
-0.004217       -0.004217       -0.004217
-0.004324       -0.004324       -0.004324
-0.004404       -0.004404       -0.004404
-0.004459       -0.004459       -0.004459
-0.004491       -0.004491       -0.004491
-0.004500       -0.004500       -0.004500
-0.004488       -0.004488       -0.004488
-0.004457       -0.004457       -0.004457
-0.004407       -0.004407       -0.004407
-0.004341       -0.004341       -0.004341
-0.004259       -0.004259       -0.004259
-0.004163       -0.004163       -0.004163
-0.004055       -0.004055       -0.004055
-0.003935       -0.003935       -0.003935
-0.003806       -0.003806       -0.003806
-0.003667       -0.003667       -0.003667
-0.003521       -0.003521       -0.003521
-0.003369       -0.003369       -0.003369
-0.003212       -0.003212       -0.003212
-0.003051       -0.003051       -0.003051
-0.002886       -0.002886       -0.002886
-0.002720       -0.002720       -0.002720
-0.002552       -0.002552       -0.002552
-0.002384       -0.002384       -0.002384
-0.002216       -0.002216       -0.002216
-0.002049       -0.002049       -0.002049
OK.
*/