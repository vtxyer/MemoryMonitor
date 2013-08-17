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

int main(int argc, char *argv[])  
{

	vector<Output_node> output;

	fin.open("ex_data", ios::in);
	string str;
	int count = 0;
	unsigned long tot_sc = 0;
	while(getline(fin, str)){
		Output_node tmp_node;
		unsigned long reduce_sc;
		sscanf(str.c_str(), "Round:%u tbs:%lu psc:%lu sc:%lu reduce_sc:%lu\n",
				&tmp_node.round, &tmp_node.total_bottleneck_pages, &tmp_node.per_swap_count, &tmp_node.swap_count, &reduce_sc);
		output.push_back(tmp_node);
		count++;
		tot_sc += tmp_node.per_swap_count;
	}


	for(int k=0; k<count; k++){
		unsigned long extra=output[k].total_bottleneck_pages;

		unsigned long remain_sc = 0;
		for(int i=0; i<count; i++){
			unsigned long tbs = output[i].total_bottleneck_pages;
			if(tbs > extra){
				double ratio = (double)tbs/(double)(tbs - extra);
				remain_sc += (unsigned long)((double)output[i].per_swap_count/ratio);
			}
//			printf("Round:%u tbs:%lu[M] psc:%lu sc:%lu reduce_sc:%lu\n", 
//					output[i].round, output[i].total_bottleneck_pages/256, output[i].per_swap_count, output[i].swap_count,
//					reduce_sc);
		}
		printf("Size:%lu[M] RSCount:%lu  Recuce:%lu%\n", extra/256 + 512, remain_sc, ((tot_sc-remain_sc)*100)/tot_sc);
	}



	fin.close();
	return 0;
}


