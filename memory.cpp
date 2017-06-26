/*
 * memory.cpp
 */

#include "cacheSim.h"

block::block(){
	valid=false;
	dirty=false;
	address=0; //doesn't matter as valid is false
}

bool block::set_get_valid(bool valid, bool set_get_){
	if(set_get_==GET) return this->valid;
	this->valid=valid;
	return true;
}

bool block::set_get_dirty(bool dirty, bool set_get_){
	if(set_get_==GET) return this->dirty;
	this->dirty=dirty;
	return true;
}

unsigned long int block::set_get_address(unsigned long int address, bool set_get_){
	if(set_get_==GET) return this->address;
	this->address=address;
	return address;
}

set::set(unsigned int numOfWays){
	this->numOfWays=numOfWays;
	blocks = vector<block>(numOfWays,block());
	LRU = deque<unsigned int>();
	for(unsigned int i=0; i<(numOfWays); i++)
		LRU.push_front(i);
}

void set::read(unsigned long int address)
{
	updateLRU(find(address));
}

void set::write(unsigned long int address)
{
	long int way=find(address);
	updateLRU(way);
	blocks[way].set_get_dirty(DIRTY,SET);
}

void set::erase(unsigned long int address)
{
	long int way=find(address);
	blocks[way].set_get_valid(INVALID,SET);
	for(deque<unsigned int>::iterator it=LRU.begin(); it != LRU.end(); it++)
			if(*it == way) {
				LRU.erase(it);
				LRU.push_front(way);
			}
}

long int set::find(unsigned long int address)
{
	for(vector<block>::iterator it = blocks.begin(); it !=blocks.end(); it++)
	{
		cout<<(it-blocks.begin())<<" "<<"address: "<<(it->set_get_address(0,GET))<<" valid: "<<it->set_get_valid(0,GET)<<endl;
		if((it->set_get_address(0,GET) == address) &&  it->set_get_valid(0,GET))
		{
			return it-blocks.begin();
		}
	}
	return -1;
}

bool set::allocate(unsigned long int address, unsigned long int *evictAddress)
{
	bool isEvicted=false;
	long int way=LRU.front();
	if(blocks[way].set_get_valid(0,GET) && blocks[way].set_get_dirty(0,GET)) {
		*evictAddress=blocks[way].set_get_address(0,GET);
		isEvicted = true;
	}
	blocks[way].set_get_dirty(CLEAN,SET);
	blocks[way].set_get_valid(VALID,SET);
	blocks[way].set_get_address(address,SET);
	return isEvicted;
}

void set::update(unsigned long int address)
{
	blocks[find(address)].set_get_dirty(DIRTY,SET);
}

void set::updateLRU(long int way)
{
	for(deque<unsigned int>::iterator it=LRU.begin(); it != LRU.end(); it++)
		if(*it == way) {
			LRU.erase(it);
			LRU.push_back(way);
		}
}

cache::cache()							//just because of compilation errors
{
	numOfWays=0;
	setsMask=0;
	bSize=0;
}

cache::cache(unsigned int numOfSets, unsigned int numOfWays, unsigned int bSize)
{
	this->bSize=bSize;
	this->numOfWays=numOfWays;
	sets = vector<set>(numOfSets,set(numOfWays));
	setsMask = 0;
	while(!(numOfSets & 0x1)){
		setsMask = (setsMask<<1) | 1;
		numOfSets = numOfSets>>1;
	}
	setsMask = setsMask << bSize;
}

bool cache::find(unsigned long int address)
{
	cout<<"set::find="<<sets[extractSet(address)].find(address)<<endl;
	return ((sets[extractSet(address)].find(address) < 0 ) ? false : true);
}

void cache::read(unsigned long int address)
{
	sets[extractSet(address)].read(address);
}

void cache::write(unsigned long int address)
{
	sets[extractSet(address)].write(address);
}

void cache::erase(unsigned long int address)
{
	sets[extractSet(address)].erase(address);
}

void cache::update(unsigned long int address)
{
	sets[extractSet(address)].update(address);
}

bool cache::allocate(unsigned long int address, unsigned long int *evictAddress)
{
	return sets[extractSet(address)].allocate(address, evictAddress);
}

unsigned int cache::extractSet(unsigned long int address)
{
	return (setsMask & address)>>bSize;
}

MEM::MEM(unsigned int MEMCyc, unsigned int blockSize, bool writeAllocate,
		unsigned int L1Size, unsigned int L1Assoc, unsigned int L1Cyc,
		unsigned int L2Size, unsigned int L2Assoc, unsigned int L2Cyc)
{
	unsigned int L1Ways=1<<L1Assoc;
	unsigned int L2Ways=1<<L2Assoc;
	unsigned int L1Sets=1<<(L1Size-(blockSize+L1Assoc));
	unsigned int L2Sets=1<<(L2Size-(blockSize+L2Assoc));
	L1=cache(L1Sets,L1Ways,blockSize);
	L2=cache(L2Sets,L2Ways,blockSize);
	L1_hits=0;
	L2_hits=0;
	MEM_hits=0;
	L1_cyc=L1Cyc;
	L2_cyc=L2Cyc;
	MEM_cyc=MEMCyc;
	unsigned long int ones=-1;
	blockMask=ones<<blockSize;
	write_allocate=writeAllocate;
}

void MEM::execute(unsigned long int address, char operation)
{
	address=address & blockMask;
	cout<< "adress: " << address<<endl;
	unsigned long int evictAddress;

	if(L1.find(address)){					//hit in L1
		cout << "L1: "<< ++L1_hits<< endl;
		if(operation=='w')
			L1.write(address);
		else
			L1.read(address);
	}
	else if(L2.find(address)){				//hit in L2
		cout << "L2: "<< ++L2_hits<< endl;
		if(operation == 'r' || write_allocate)
		{
			if(EVICTED == L1.allocate(address,&evictAddress))
				L2.update(evictAddress);
			if(operation=='w')
				L1.write(address);
			else
				L1.read(address);
		}
		else L2.write(address);
	}
	else									//hit in RAM
	{
		cout << "MEM: "<< ++MEM_hits<< endl;
		if(operation == 'r' || write_allocate)
		{
			if(EVICTED == L2.allocate(address, &evictAddress))
			{
				if(L1.find(evictAddress))
					L1.erase(evictAddress);
			}
			if(EVICTED == L1.allocate(address, &evictAddress))
				L2.update(evictAddress);
			if(operation=='w')
				L1.write(address);
			else
				L1.read(address);
		}
	}
}

void MEM::getStats(double* L1MissRate, double* L2MissRate, double* avgAccTime)
{
	cout<<L1_hits<<" "<<L1_cyc<<" "<<L2_hits<<" "<<L2_cyc<<" "<<MEM_hits<<" "<<MEM_cyc<<"\n";
	*avgAccTime = (L1_hits*L1_cyc+L2_hits*L2_cyc+MEM_hits*MEM_cyc)/(L1_hits+L2_hits+MEM_hits);
	*L2MissRate=MEM_hits/(L2_hits+MEM_hits);
	*L1MissRate=(L2_hits+MEM_hits)/(L1_hits+L2_hits+MEM_hits);
}
