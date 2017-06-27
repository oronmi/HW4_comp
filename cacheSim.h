/*
 * cacheSim.h
 *
 *  Created on: Jun 23, 2017
 *      Author: compm
 */

#include <iostream>
using std::cout;

#ifndef CACHESIM_H_
#define CACHESIM_H_

#include <vector>
#include <deque>
using std::vector;
using std::deque;
using std::endl;

enum{
	GET = 0,
	SET = 1,
};

enum{
	CLEAN = 0,
	DIRTY = 1,
};

enum{
	INVALID = 0,
	VALID = 1,
};

enum{
	NOT_EVICTED = 0,
	EVICTED = 1,
};

class block {
public:
	block();
	unsigned long int set_get_address(unsigned long int address, bool set_get_);
	bool set_get_valid(bool valid, bool set_get_);
	bool set_get_dirty(bool dirty, bool set_get_);

private:
	unsigned long int address;
	bool valid;
	bool dirty;
};

class set {
public:
	set(unsigned int numOfWays);
	void read(unsigned long int address);
	void write(unsigned long int address);
	bool allocate(unsigned long int address, block *evictBlock);
	long int find(unsigned long int address);
	void update(block *evictBlock);
	void updateLRU(long int index);
	void erase(unsigned long int address);
private:
	vector<block> blocks;
	deque<unsigned int> LRU;
	unsigned int numOfWays;
};

class cache {
public:
	cache();
	cache(unsigned int numOfSets, unsigned int numOfWays, unsigned int bSize);
	bool find(unsigned long int address);
	void read(unsigned long int address);
	void write(unsigned long int address);
	void update(block *evictBlock);
	bool allocate(unsigned long int address, block *evictBlock);
	void erase(unsigned long int address);
	unsigned int extractSet(unsigned long int address);
private:
	vector<set> sets;
	unsigned int numOfWays;
	unsigned int setsMask;
	unsigned int bSize;
};

class MEM {
public:
	MEM(unsigned int MEMCyc, unsigned int blockSize, bool writeAllocate,
			unsigned int L1Size, unsigned int L1Assoc, unsigned int L1Cyc,
			unsigned int L2Size, unsigned int L2Assoc, unsigned int L2Cyc);
	void execute(unsigned long int address, char operation);
	void getStats(double* L1MissRate, double* L2MissRate, double* avgAccTime);
private:
	cache L1;
	cache L2;
	unsigned int L1_hits;
	unsigned int L2_hits;
	unsigned int MEM_hits;
	unsigned int L1_cyc;
	unsigned int L2_cyc;
	unsigned int MEM_cyc;
	unsigned long int blockMask;
	bool write_allocate;
};




#endif /* CACHESIM_H_ */

