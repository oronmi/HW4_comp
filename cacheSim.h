/*
 * cacheSim.h
 *
 *  Created on: Jun 23, 2017
 *      Author: compm
 */

#ifndef CACHESIM_H_
#define CACHESIM_H_

#include <vector>
#include <queue>
using std::vector;
using std::queue;

enum{
	GET = 0,
	SET = 1,
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
	bool read(unsigned long int address);
	bool write(unsigned long int address,unsigned int *evictAddress);
private:
	vector<block> blocks;
	queue<unsigned int> LRU;
};

class cache {
public:
	cache(unsigned int numOfSets, unsigned int cyclesPerAccess, const unsigned int numOfWays);
private:
	vector<set> sets;
	const unsigned int cyclesPerAccess;
	const unsigned int numOfWays;
};

class MEM {
public:
	MEM(unsigned int MEM_cyc, unsigned int blockSize, bool writeAllocate,
			unsigned int L1Size, unsigned int L1Assoc, unsigned int L1Cyc,
			unsigned int L2Size, unsigned int L2Assoc, unsigned int L2Cyc);
	void read(unsigned long int address);
	void write(unsigned int address);
	void getStats(double* L1MissRate, double* L2MissRate, double* avgAccTime);
private:
	cache L1;
	cache L2;
	unsigned int L1_hits;
	unsigned int L2_hits;
	unsigned int MEM_hits;
	const unsigned int L1_cyc;
	const unsigned int L2_cyc;
	const unsigned int MEM_cyc;
};




#endif /* CACHESIM_H_ */

