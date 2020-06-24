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
	Bloomclass(int);
	~Bloomclass();

	int GetBloomlen();
	int SetBloomlen(int);
	bool checkstring(char*);
	bool AddString(char*); //Add string
	int initspellchecker(int);
	void echodicthash();
	int exportdict(TCHAR*);
	int importdict(TCHAR*);
	int releasedict();
	void sethashfunction_num(short num) {
		hashfunction_num = num;
	}
	short gethashfunction_num() {
		return hashfunction_num;
	}
private:
	bloom bloomfilter;
	char tmpbuffer[MAXLEN]= "";
	char ltmpbuffer[MAXLEN] = "";
	char lltmpbuffer[MAXLEN] = "";
	char llltmpbuffer[MAXLEN] = "";
	char lllltmpbuffer[MAXLEN] = "";
	char llllltmpbuffer[MAXLEN] = "";

	static short hashfunction_num;

	bool excludeString(char* buffer);
	bool checkbit(int position);
	bool turnbit(int position);
	bool checkbit_v1(int position);//Deprecated
	bool turnbit_v1(int position);//Deprecated
	bool hash1(char*, int);//Deprecated
	bool hash2(char*, int);//Deprecated
	bool hash3(char*, int);//Deprecated
	bool hash4(char*, int);//Deprecated
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