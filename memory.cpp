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

}
