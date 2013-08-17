#include "define.h"
#include <algorithm>
#include <vector>


class Output_node
{
public:
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
bool v_sort(Output_node a, Output_node b)
{
	if(a.total_bottleneck_pages > b.total_bottleneck_pages)
		return true;
	else
		return false;
}


void estimate_output(DATAMAP &cr3_data)
{
	round_t i;
	unsigned long tsc = 0;

	vector<Output_node> output;

	for(i=0; i<round; i++){
		Sampled_data &sample_node = sample_result[i];
		DATAMAP::iterator all_cr3_data_it = cr3_data.begin();
		unsigned long scp;
		unsigned long total_bottleneck_pages;
		sample_node.get_value(total_bottleneck_pages, scp);
		list<cr3_t> dead_cr3_list;


		/*determine dead cr3 and estimate bottleneck pages*/
		while(all_cr3_data_it != cr3_data.end()){
			cr3_t cr3 = all_cr3_data_it->first;
			struct hash_table &cr3_data = all_cr3_data_it->second;
			round_t end_round = cr3_data.end_round;
			if(end_round < i){
				dead_cr3_list.push_back(cr3);
				total_bottleneck_pages -= sample_node.cr3_info[cr3]; 
			}
			++all_cr3_data_it;
		}


		/*check shared pages*/
		SHARED_TREE::iterator slist = sample_node.shared_tree.begin();
		while( slist != sample_node.shared_tree.end() )
		{
			Shared_page_node &shared_node = slist->second;
			if(shared_node.is_page_counted(dead_cr3_list)){
				total_bottleneck_pages += 1;
			}
			slist++;
		}
		sample_node.set_value(total_bottleneck_pages, scp);
		dead_cr3_list.clear();
		tsc += scp;	
		output.push_back(Output_node(total_bottleneck_pages, scp, tsc, i));
	}

	std::sort(output.begin(), output.end(), v_sort);

	if(global_total_change_times == 0 ){
		return;
	}
	for(int k=0; k<round; k++){
		unsigned long extra=output[k].total_bottleneck_pages;

		unsigned long remain_sc = 0;
		for(int i=0; i<round; i++){
			unsigned long tbs = output[i].total_bottleneck_pages;
			if(tbs > extra){			
				double ratio = (double)tbs/(double)(tbs - extra);
				remain_sc += (unsigned long)((double)output[i].per_swap_count/ratio);
			}
		}
		printf("AddSize:%lu[M] RSCount:%lu  Recuce:%lu%\n", 
				extra/256, remain_sc, ((global_total_change_times-remain_sc)*100)/global_total_change_times);
	}


}
