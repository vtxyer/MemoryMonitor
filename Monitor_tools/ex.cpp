#include "define.h"
#include <set>
#include <fstream>
#include <iostream>
#include <csignal>
#include <vector>
using namespace std;

fstream fin;

class Output_node
{
public:
	Output_node(){}
	Output_node(unsigned long tbs, unsigned long psc, unsigned long sc, round_t r){
		total_bottleneck_pages = tbs;
		per_swap_count = psc;
		swap_count = sc;
		round = r;
	}
	unsigned long total_bottleneck_pages;
	unsigned long per_swap_count;
	unsigned long swap_count;
	round_t round;
};


int remove_redundant(cr3_t *cr3_list, int list_size)
{
	cr3_t new_cr3_list[9999];
	int new_size = 0;
	for(int i=0; i<list_size; i++){
		cr3_t now_cr3 = cr3_list[i];
		if(now_cr3 == 0 ){
			continue;
		}
		bool dflag = false;
		for(int k=i+1; k<list_size; k++){
			if(now_cr3 == cr3_list[k]){
				cr3_list[k] = 0;
				dflag = true;
			}
		}
		new_cr3_list[new_size] = now_cr3;
		new_size++;
	}
	for(int i=0; i<list_size; i++){
		if(i < new_size){
			cr3_list[i] = new_cr3_list[i];
		}
		else	
			cr3_list[i] = 0;
	}
	return new_size;
}

int main(int argc, char *argv[])  
{
	unsigned long a[14] = {1,1, 3, 4, 2, 1 ,2, 5,16, 3, 3, 4, 26, 361};
	int k = remove_redundant(a, 14);
	for(int i=0; i<k; i++){
		printf(" %lu\n", a[i]);
	}
	printf("size:%d\n", k);

	return 0;
}


