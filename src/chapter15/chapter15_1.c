#include "chapter15/chapter15_1.h"

#include <stdlib.h>

#include "macros.h"
#include "util.h"

struct rod_price {
	int *price;
	int *solution;
	int *cut_length;
	int count;
};

// 自底向上的钢条切割的动态规划问题
static inline void bottom_up_cut_rod(struct rod_price *price) {
	int p;
	int s;

	price->price[0] = 0;
	price->solution[0] = 0;
	for (int j = 1; j <= price->count; ++j) {
		p = -1;
		s = 0;
		for (int i = 1; i <= j; ++i) {
			if (p <= price->price[i] + price->solution[j - i]) {
				p = price->price[i] + price->solution[j - i];
				s = i;
			}
		}
		price->solution[j] = p;
		price->cut_length[j] = s;
	}
}

static void print_cut_rod_solution(struct rod_price *price, 
								   int n) {
	DCHECK(n >= 0 && n <= price->count);
	fprintf(stdout, "%2d :", price->solution[n]);
	while (n > 0) {
		fprintf(stdout, " %d", price->cut_length[n]);
		n -= price->cut_length[n];
	}
	fputc('\n', stdout);
}

static void free_rod_price(struct rod_price *price) {
	free(price->price);
	free(price->solution);
	free(price->cut_length);
}

static void init_rod_price(struct rod_price *price, int n) {
	price->price = (int*) malloc(sizeof(int) * (n+1));
	price->solution = (int*) malloc(sizeof(int) * (n+1));
	price->cut_length = (int*) malloc(sizeof(int) * (n+1));
	price->count = n;
}

void chapter15_1_tutorial() {
	int price[] = {1, 5, 8, 9, 10, 17, 17, 20, 24, 30};
	struct rod_price rod_price;

	init_rod_price(&rod_price, NARRAY(price));
	for (int i = 0; i < NARRAY(price); ++i) {
		rod_price.price[i+1] = price[i];
	}
	bottom_up_cut_rod(&rod_price);
	for (int i = 0; i <= NARRAY(price); ++i) {
		print_cut_rod_solution(&rod_price, i);
	}
	free_rod_price(&rod_price);
}

void chapter15_1_main() {
	chapter15_1_tutorial();
}
