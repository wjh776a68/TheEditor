#pragma once
#ifndef BLOOMCLASS_H
#define BLOOMCLASS_H

#include"TheEditorcommon.h"

typedef struct bloomstruct {
	unsigned short* dicthash = NULL;
	int BLOOMFILTERBITCOUNT = 0;
	int allocatesize = 0;
}bloom;

class Bloomclass {
public:

	int GetBloomlen();
	int SetBloomlen(int);
	bool checkstring(char*);
	bool AddString(char*); //Add string
	int initspellchecker(int);
	void echodicthash();
	int exportdict(TCHAR*);
	int importdict(TCHAR*);
	int releasedict();

private:
	bloom bloomfilter;
	
	bool excludeString(char* buffer);
	bool checkbit(int position);
	bool turnbit(int position);
	bool hash1(char*, int);//록폴
	bool hash2(char*, int);//록폴
	bool hash3(char*, int);//록폴
	bool hash4(char*, int);//록폴
	bool hash5(char*, int);
	bool hash6(char*, int);
	bool hash7(char*, int);
	bool hash8(char*, int);
	bool hash9(char*, int);
	bool hash10(char*, int);
	bool hash11(char*, int);
	bool hash12(char*, int);



};

#endif