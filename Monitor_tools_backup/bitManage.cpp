#include "define.h"

//1~7 bits represent change number
int add_change_number(byte &value)
{
	byte number;
	number = (value>>1) & 0x7f;
	if(number<0x7f)
	{
		number += 1;
		number <<= 1;
		number |= (value&1);
		value = number;
	}
}
byte get_change_number(byte value)
{
	byte number;
	number = (value>>1) & 0x7f;
	return number;
} 
unsigned long get_bit(unsigned long entry, int num, int position)
{
	unsigned long mask = 0;
	unsigned long i, value;
	for(i=0; i<num; i++){
		mask<<=(unsigned long)1;
		mask+=1;
	}
	value = (entry&(mask<<(unsigned long)position));
	value >>= (unsigned long)position;
	return value;
}
void clear_bit(unsigned long &addr, int start, int end){
	unsigned long mask = 0;
	unsigned long tmp = -1;
	for(int i=start; i<=end; i++){
		mask<<=1;
		mask|=1;
	}
	mask <<= start;
	mask ^= tmp;
	addr &= mask;
}

int page_size_flag (uint64_t entry){
	return get_bit(entry, 1, 7);
}
int entry_present(uint64_t entry){
	return get_bit(entry, 1, 0);
}
int get_access_bit(uint64_t entry){
	return get_bit(entry, 1, 5);
}
int get_huge_bit(uint64_t entry)
{
	return get_bit(entry, 1, 0);
}
unsigned long get_paddr(uint64_t addr)
{
	return get_bit(addr, 36, 28);
}
unsigned long get_swap_id(uint64_t addr){
	return get_bit(addr, 63, 1);
}
void save_paddr(unsigned long &addr, unsigned long val){
	clear_bit(addr, 28, 63);
	addr |= (val<<28);
}
void save_swap_paddr(unsigned long &addr, unsigned long val){
	clear_bit(addr, 1, 27);
	addr |= ((val&0xeffffff)<<1);
}
void save_huge_bit(unsigned long &addr, unsigned long val){
	clear_bit(addr, 0, 0);
	addr |= (val & 0x1);
}



unsigned long get_vaddr(unsigned long l1offset, unsigned long l2offset, unsigned long l3offset, unsigned long l4offset)
{
	unsigned long va = 0, tmp;

	tmp = 0;
	tmp |= (l1offset<<12);
	va |= tmp;
	tmp = 0;
	tmp |= (l2offset<<21);
	va |= tmp;tmp = 0;
	tmp |= (l3offset<<30);
	va |= tmp;tmp = 0;
	tmp |= (l4offset<<39);
	va |= tmp;

	return va;
}
int entry_valid(unsigned long entry)
{
	int flag = entry & 0xfff;
	uint32_t gflags, mflags, iflags, rc = 0;

	//	if( (entry&1) == 0 || (get_access_bit(entry) == 0)){
	if( (entry&1) == 0 ){
		return 0; //invalid
	}
	else
		return 1; //valid
}
int pte_entry_valid(unsigned long entry)
{
	return entry_present(entry);
}

