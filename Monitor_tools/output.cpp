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
class Times_to_num
{
public:
	Times_to_num(int _times, unsigned long _size){
		times = _times;
		size = _size;
	}
	int times;
	unsigned long size;
};
bool v_sort(Output_node a, Output_node b)
{
	if(a.total_bottleneck_pages > b.total_bottleneck_pages)
		return true;
	else
		return false;
}
bool v_sort1(Times_to_num a, Times_to_num b)
{
	if(a.size > b.size)
		return true;
	else
		return false;
}
bool v_sort2(Times_to_num a, Times_to_num b)
{
	if(a.size < b.size)
		return true;
	else
		return false;
}

double calculate_percent(map<byte, unsigned int> &times_to_num, unsigned long reduce_size, int flag)
{
	unsigned long reduce_swap_count = 0;
	unsigned long per_total_swap_count = 0;

	vector<Times_to_num> times_to_num_v;

	for(byte k=0; k<127; k++){
		if(times_to_num.count(k) > 0 ){
			per_total_swap_count += times_to_num[k]*k;
			times_to_num_v.push_back(Times_to_num(k, times_to_num[k]));
		}
	}



	if(flag == 0){
		for(byte k=127; k>0; k--){
	//	for(byte k=0; k<127; k++){
	//	for(vector<Times_to_num>::iterator it=times_to_num_v.begin(); it!=times_to_num_v.end(); ++it){
			if(reduce_size > 0){
				if(reduce_size > times_to_num[k]){
					reduce_size -= times_to_num[k];
					reduce_swap_count += times_to_num[k] * k;
				}
				else{
					reduce_swap_count += reduce_size*k;
					reduce_size = 0;
				}
			}
		}
	}
	else if(flag == 1){
	//	for(byte k=127; k>0; k--){
		for(byte k=0; k<127; k++){
	//	for(vector<Times_to_num>::iterator it=times_to_num_v.begin(); it!=times_to_num_v.end(); ++it){
			if(reduce_size > 0){
				if(reduce_size > times_to_num[k]){
					reduce_size -= times_to_num[k];
					reduce_swap_count += times_to_num[k] * k;
				}
				else{
					reduce_swap_count += reduce_size*k;
					reduce_size = 0;
				}
			}
		}
	}
	//	for(byte k=127; k>0; k--){
	//	for(byte k=0; k<127; k++){
	else if(flag == 2){
		std::sort(times_to_num_v.begin(), times_to_num_v.end(), v_sort1);
		for(vector<Times_to_num>::iterator it=times_to_num_v.begin(); it!=times_to_num_v.end(); ++it){
			Times_to_num tmp = *it;
			if(reduce_size > 0){
				if(reduce_size > tmp.size){
					reduce_size -= tmp.size;
					reduce_swap_count += tmp.size * tmp.times;
				}
				else{
					reduce_swap_count += reduce_size*tmp.times;
					reduce_size = 0;
				}
			}
		}
	}
	else if(flag == 3){
		std::sort(times_to_num_v.begin(), times_to_num_v.end(), v_sort2);
		for(vector<Times_to_num>::iterator it=times_to_num_v.begin(); it!=times_to_num_v.end(); ++it){
			Times_to_num tmp = *it;
			if(reduce_size > 0){
				if(reduce_size > tmp.size){
					reduce_size -= tmp.size;
					reduce_swap_count += tmp.size * tmp.times;
				}
				else{
					reduce_swap_count += reduce_size*tmp.times;
					reduce_size = 0;
				}
			}
		}
	}


	return (double)per_total_swap_count/(double)(per_total_swap_count-reduce_swap_count);
}

void show_result( vector<Output_node> &output)
{
	std::sort(output.begin(), output.end(), v_sort);
	int percent=100;

	if(global_total_change_times == 0 ){
		return;
	}
	unsigned long extra = 0;
	for(int k=0; k<10000; k++){
		unsigned long remain_sc[5] = {0};

		for(int i=0; i<round; i++){
			unsigned long tbs = output[i].total_bottleneck_pages;
			if(tbs > extra){
				for(int g=0; g<4; g++){
					double ratio = calculate_percent((sample_result[output[i].round]).swap_count_tims_to_num, extra, g);
					remain_sc[g] += (unsigned long)(((double)output[i].per_swap_count)/ratio);
				}
				double ratio = (double)(output[i].total_bottleneck_pages)/(double)(output[i].total_bottleneck_pages-extra);
				remain_sc[4] += (unsigned long)(((double)output[i].per_swap_count)/ratio);
			}
		}

		bool over = false;
		for(int g=0; g<5; g++){
			int now_percent = ((global_total_change_times-remain_sc[g])*100)/global_total_change_times; 
			if(extra > output[0].total_bottleneck_pages){
				printf("Type:%d AddSize:%lu[M] RSCount:%lu  Recuce:%lu%\n", 
						g, output[0].total_bottleneck_pages/256, 0, 100);
				over = true;
			}
			else{
				printf("Type:%d AddSize:%lu[M] RSCount:%lu  Recuce:%lu%\n", 
					g, extra/256, remain_sc[g], now_percent);
			}
		}
		if(over){
			break;
		}
		printf("\n");
		extra += 64*256;
	}
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
			}
			++all_cr3_data_it;
		}

		/*check shared pages*/
		SHARED_TREE::iterator slist = sample_node.shared_tree.begin();
		while( slist != sample_node.shared_tree.end() )
		{
			Shared_page_node &shared_node = slist->second;
			unsigned long rnum;
			if( (rnum = shared_node.is_page_counted(dead_cr3_list)) > 0){
				printf("get shared page\n");
				total_bottleneck_pages += rnum - 1;
			}
			slist++;
		}

		/*estimate bottleneck pages*/
		list<cr3_t>::iterator dcr3t= dead_cr3_list.begin();
		while(dcr3t != dead_cr3_list.end()){
			cr3_t cr3 = *dcr3t;
			if( total_bottleneck_pages < sample_node.cr3_info[cr3])
				printf("error t:%lu c:%lu's cr3:%lx\n", total_bottleneck_pages, sample_node.cr3_info[cr3]);
			total_bottleneck_pages -= sample_node.cr3_info[cr3]; 
			++dcr3t;
		}

		sample_node.set_value(total_bottleneck_pages, scp);
		dead_cr3_list.clear();
		tsc += scp;	
		output.push_back(Output_node(total_bottleneck_pages, scp, tsc, i));
	}
	show_result(output);
}
