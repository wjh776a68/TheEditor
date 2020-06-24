#include"BloomClass.h"

short Bloomclass::hashfunction_num = 8;
/***********************************************
	函数名称: Bloomclass::hash1(char* buffer, int command = 0)
	函数功能: 将字符串按照ascii码变为数字，并得到其积，最后对比特长度取余，如果int参数为1则将bloomfilter对应标志位置1，否则跳过。之后查询该位置是否为1.
	传入参数: char*目标字符串，int 0：查询，1：更改
	返回值: 如果是1返回true，否则为false。
************************************************/
bool Bloomclass::hash1(char* buffer, int command = 0) {//Deprecated
	unsigned int tmp = 1;
	while (*buffer != '\0')
	{
		tmp *= (int)(*buffer);
		tmp %= bloomfilter.BLOOMFILTERBITCOUNT;
		buffer++;
	}
	if (command == 1)
		return turnbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
	return checkbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
		//*(bloomfilter.dicthash + tmp % bloomfilter.BLOOMFILTERBITCOUNT) = 1;
	//return *(bloomfilter.dicthash + tmp % bloomfilter.BLOOMFILTERBITCOUNT) == 1;

}

/***********************************************
	函数名称: Bloomclass::hash2(char* buffer, int command = 0)
	函数功能: 将字符串按照ascii码变为数字，并得到其和，最后对比特长度取余，如果int参数为1则将bloomfilter对应标志位置1，否则跳过。之后查询该位置是否为1.
	传入参数: char*目标字符串，int 0：查询，1：更改
	返回值: 如果是1返回true，否则为false。
************************************************/
bool Bloomclass::hash2(char* buffer, int command = 0) {//Deprecated
	unsigned int tmp = 0;
	while (*buffer != '\0')
	{
		tmp += (int)(*buffer);

		buffer++;
	}
	if (command == 1)
		return turnbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
	return checkbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
		//*(bloomfilter.dicthash + tmp % bloomfilter.BLOOMFILTERBITCOUNT) = 1;
	//return *(bloomfilter.dicthash + tmp % bloomfilter.BLOOMFILTERBITCOUNT) == 1;
}

/***********************************************
	函数名称: Bloomclass::hash3(char* buffer, int command = 0)
	函数功能: 将字符串按照ascii码变为数字，并得到平方根取整的和，最后对比特长度取余，如果int参数为1则将bloomfilter对应标志位置1，否则跳过。之后查询该位置是否为1.
	传入参数: char*目标字符串，int 0：查询，1：更改
	返回值: 如果是1返回true，否则为false。
************************************************/
bool Bloomclass::hash3(char* buffer, int command = 0) {//Deprecated
	unsigned int tmp = 1;
	while (*buffer != '\0')
	{
		tmp *= (int)(sqrt((double)*buffer));

		buffer++;
	}
	if (command == 1)
		return turnbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
	return checkbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
		//*(bloomfilter.dicthash + tmp % bloomfilter.BLOOMFILTERBITCOUNT) = 1;
	//return *(bloomfilter.dicthash + tmp % bloomfilter.BLOOMFILTERBITCOUNT) == 1;
}

/***********************************************
	函数名称: Bloomclass::hash4(char* buffer, int command = 0)
	函数功能: 将字符串按照ascii码变为数字，并按+-+-。。。顺序依次加减，最后对比特长度取余，如果int参数为1则将bloomfilter对应标志位置1，否则跳过。之后查询该位置是否为1.
	传入参数: char*目标字符串，int 0：查询，1：更改
	返回值: 如果是1返回true，否则为false。
************************************************/
bool Bloomclass::hash4(char* buffer, int command = 0) {//Deprecated
	unsigned int tmp = 0;
	unsigned int sign = 1;
	while (*buffer != '\0')
	{
		tmp += sign * (int)(*buffer);
		sign *= -1;
		buffer++;
	}
	
	if (command == 1)
		return turnbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
	return checkbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
		//*(bloomfilter.dicthash + tmp % bloomfilter.BLOOMFILTERBITCOUNT) = 1;
	//return *(bloomfilter.dicthash + tmp % bloomfilter.BLOOMFILTERBITCOUNT) == 1;
}

// BKDR Hash Function
bool Bloomclass::hash5(char* buffer, int command = 0) {
	if (Bloomclass::gethashfunction_num() < 1) {//当设置完散列函数个数后，可以用此进行顺序排除
		return true;
	}
	
		unsigned int tmp = 0;
		unsigned int seed = 131; // 31 131 1313 13131 131313 etc..

		while (*buffer)
		{
			tmp = tmp * seed + (*buffer++);
		}

		tmp = tmp & 0x7FFFFFFF;

		if (command == 1)
			return turnbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
		return checkbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
	
	
	
}

// AP Hash Function
bool Bloomclass::hash6(char* buffer, int command = 0) {
	if (Bloomclass::gethashfunction_num() < 2) {//当设置完散列函数个数后，可以用此进行顺序排除
		return true;
	}

	unsigned int tmp = 0;

	int i;

	for (i = 0; *buffer; i++){
		if ((i & 1) == 0){
			tmp ^= ((tmp << 7) ^ (*buffer++) ^ (tmp >> 3));
		}
		else{
			tmp ^= (~((tmp << 11) ^ (*buffer++) ^ (tmp >> 5)));
		}
	}

	tmp = tmp & 0x7FFFFFFF;

	if (command == 1)
		return turnbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
	return checkbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
}

// DJB Hash Function
bool Bloomclass::hash7(char* buffer, int command = 0) {
	if (Bloomclass::gethashfunction_num() < 3) {//当设置完散列函数个数后，可以用此进行顺序排除
		return true;
	}

	unsigned int tmp = 5381;

	while (*buffer)
	{
		tmp += (tmp << 5) + (*buffer++);
	}

	tmp = tmp & 0x7FFFFFFF;

	if (command == 1)
		return turnbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
	return checkbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
}

//JSHash Function
bool Bloomclass::hash8(char* buffer, int command = 0) {
	if (Bloomclass::gethashfunction_num() < 4) {//当设置完散列函数个数后，可以用此进行顺序排除
		return true;
	}

	
	unsigned int tmp = 1315423911;

	while (*buffer)
	{
		tmp ^= ((tmp << 5) + (*buffer++) + (tmp >> 2));
	}

	tmp = tmp & 0x7FFFFFFF;

	if (command == 1)
		return turnbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
	return checkbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
}

// RS Hash Function
bool Bloomclass::hash9(char* buffer, int command = 0) {
	if (Bloomclass::gethashfunction_num() < 5) {//当设置完散列函数个数后，可以用此进行顺序排除
		return true;
	}

	unsigned int tmp = 0;
	unsigned int b = 378551;
	unsigned int a = 63689;

	while (*buffer)
	{
		tmp = tmp * a + (*buffer++);
		a *= b;
	}

	tmp = tmp & 0x7FFFFFFF;

	if (command == 1)
		return turnbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
	return checkbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
}

//SDBM hash function
bool Bloomclass::hash10(char* buffer, int command = 0) {
	if (Bloomclass::gethashfunction_num() < 6) {//当设置完散列函数个数后，可以用此进行顺序排除
		return true;
	}

	unsigned int tmp = 0;

	while (*buffer)
	{
		// equivalent to: hash = 65599*hash + (*str++);
		tmp = (*buffer++) + (tmp << 6) + (tmp << 16) - tmp;
	}

	tmp = tmp & 0x7FFFFFFF;

	if (command == 1)
		return turnbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
	return checkbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
}

//FNVHash function
bool Bloomclass::hash11(char* buffer, int command = 0) {
	if (Bloomclass::gethashfunction_num() < 7) {//当设置完散列函数个数后，可以用此进行顺序排除
		return true;
	}

	unsigned int tmp = 0;

	int fnvprime = 0x811C9DC5;

	while (*buffer) {
		tmp *= fnvprime;
		tmp ^= (int)(*buffer++);
	}

	tmp = tmp & 0x7FFFFFFF;

	if (command == 1)
		return turnbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
	return checkbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
}

//JAVA string hash function
bool Bloomclass::hash12(char* buffer, int command = 0) {
	if (Bloomclass::gethashfunction_num() < 8) {//当设置完散列函数个数后，可以用此进行顺序排除
		return true;
	}

	unsigned int tmp = 0;

	while (*buffer) {
		tmp = tmp * 31 + (*buffer++);
	}


	tmp = tmp & 0x7FFFFFFF;

	if (command == 1)
		return turnbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
	return checkbit(tmp % bloomfilter.BLOOMFILTERBITCOUNT);
}

/***********************************************
	函数名称: Bloomclass::AddString(char* buffer)
	函数功能: 添加目标字符串到bloomfilter的特征位为1
	传入参数: char*目标字符串
	返回值: 如果符合返回true，否则为false。
************************************************/
bool Bloomclass::AddString(char* buffer)
{
	return hash5(buffer, 1) && hash6(buffer, 1) && hash7(buffer, 1) && hash8(buffer, 1) && 
		hash9(buffer, 1) && hash10(buffer, 1) && hash11(buffer, 1) && hash12(buffer, 1);
}

/***********************************************
	函数名称: Bloomclass::checkstring(char* buffer)
	函数功能: 检查目标字符串是否在bloomfilter的特征位为1
	传入参数: char*目标字符串
	返回值: 如果符合返回true，否则为false。
************************************************/
bool Bloomclass::checkstring(char* buffer)
{
	if (Bloomclass::excludeString(buffer) || buffer==NULL || *buffer=='\0') {
		return false;
	}
	strcpy_s(llllltmpbuffer, lllltmpbuffer);
	strcpy_s(lllltmpbuffer, llltmpbuffer);
	strcpy_s(llltmpbuffer, lltmpbuffer);
	strcpy_s(lltmpbuffer, ltmpbuffer);
	strcpy_s(ltmpbuffer, tmpbuffer);
	strcpy_s(tmpbuffer , buffer);
	
	
	return hash5(buffer, 0) && hash6(buffer, 0) && hash7(buffer, 0) && hash8(buffer, 0) &&
		hash9(buffer, 0) && hash10(buffer, 0) && hash11(buffer, 0) && hash12(buffer, 0);
}

/***********************************************
	函数名称: Bloomclass::exportdict(TCHAR* path)
	函数功能: 将bloomfilter结构体的二进制位数据导出到目标路径
	传入参数: TCHAR*文件路径
	返回值: 如果操作完成返回的bloomfilter的二进制位的个数，否则为-1。
************************************************/
int Bloomclass::exportdict(TCHAR* GLOBALDICTFILENAME)
{

	FILE* fp;
	_wfopen_s(&fp, GLOBALDICTFILENAME, L"w+");

	if (fp != NULL)
	{
		char checklegal[] = "bfd";
		fwrite(checklegal, sizeof(char), 4, fp);
		fwrite(&(bloomfilter.BLOOMFILTERBITCOUNT), sizeof(int), 1, fp);

		fwrite(bloomfilter.dicthash, sizeof(unsigned short), bloomfilter.allocatesize, fp);
		fclose(fp);
		return bloomfilter.BLOOMFILTERBITCOUNT;
	}
	return -1;
}

/***********************************************
	函数名称: Bloomclass::importdict(TCHAR* path)
	函数功能: 向bloomfilter结构体导入来自指定路径文件内的二进制位数据，这个文件的开头以“bfd”开头，之后以一个int表示所占用的二进制位的个数
	传入参数: TCHAR*类型的路径
	返回值: 如果操作完成返回0，否则为-1。
************************************************/
int Bloomclass::importdict(TCHAR* path)
{

	FILE* fp;

	int ret = _wfopen_s(&fp, path, _T("r"));
	if (ret == 2)
	{
		MessageBox(NULL, L"NO specific dict file found!", _T("Error"), NULL);
		return -1;
	}
	if (fp == NULL)
	{

		//wprintf(publicbuffer, );
		MessageBox(NULL, L"No dict file found", _T("Error"), NULL);
		return -1;
	}
	rewind(fp);
	char checklegal[4] = { 0 };
	fread(checklegal, sizeof(char), 4, fp);
	if (strcmp(checklegal, "bfd"))
	{
		MessageBox(NULL, L"Please choose the dict file.", L"Error", NULL);
		return -1;
	}
	fread(&(bloomfilter.BLOOMFILTERBITCOUNT), sizeof(int), 1, fp);
	initspellchecker(bloomfilter.BLOOMFILTERBITCOUNT);
	fread(bloomfilter.dicthash, sizeof(unsigned short), bloomfilter.allocatesize, fp);

	fclose(fp);
	return 0;

}


/***********************************************
	函数名称: Bloomclass::releasedict(int bitcount)
	函数功能: 释放bloomfilter结构体所占用的内存
	传入参数: 无
	返回值: 如果操作完成返回0，否则不为0。
************************************************/
int Bloomclass::releasedict()
{
	free(bloomfilter.dicthash);
	bloomfilter.dicthash = NULL;
	bloomfilter.BLOOMFILTERBITCOUNT = 0;
	bloomfilter.allocatesize = 0;
	return bloomfilter.BLOOMFILTERBITCOUNT;
}


/***********************************************
	函数名称: Bloomclass::initspellchecker(int bitcount)
	函数功能: 初始化bloomfilter结构体
	传入参数: 需要的bloomfilter的二进制位个数，默认64
	返回值: 操作成功返回申请到的bloomfilter的二进制位个数。
************************************************/

int Bloomclass::initspellchecker(int bitcount = 64)
{
	bloomfilter.BLOOMFILTERBITCOUNT = bitcount;
	bloomfilter.allocatesize = bloomfilter.BLOOMFILTERBITCOUNT / (sizeof(unsigned short)*8) + 1;
	bloomfilter.dicthash = (unsigned short*)malloc(bloomfilter.allocatesize * sizeof(unsigned short));

	int i;
	for (i = 0; i < bloomfilter.allocatesize; i++)
	{
		*(bloomfilter.dicthash + i) = 0;
	}
	return bloomfilter.BLOOMFILTERBITCOUNT;
}

int Bloomclass::GetBloomlen() {
	return bloomfilter.BLOOMFILTERBITCOUNT;
}

int Bloomclass::SetBloomlen(int len) {
	Bloomclass::releasedict();
	return Bloomclass::initspellchecker(len);
}


bool Bloomclass::checkbit(int position) {
	if (position < bloomfilter.BLOOMFILTERBITCOUNT) {
		unsigned short comparepart = *(bloomfilter.dicthash + position / (sizeof(unsigned short) * 8));
		unsigned short comparepart_offset = position % (sizeof(unsigned short) * 8);

		comparepart >>= 4 * (3 - comparepart_offset / 4);
		comparepart &= 15/*1111*/;
		return comparepart & (unsigned short)pow(2, 3 - comparepart_offset % 4);

	}
	else {
		printf("Error in Bloomclass::checkbit");

		return false;
	}
}



bool Bloomclass::turnbit(int position) {
	if (position < bloomfilter.BLOOMFILTERBITCOUNT) {
		unsigned short comparepart = *(bloomfilter.dicthash + position / (sizeof(unsigned short) * 8));
		unsigned short comparepart_offset = position % (sizeof(unsigned short) * 8);

		comparepart >>= 4 * (3 - comparepart_offset / 4);
		comparepart &= 15/*1111*/;
		comparepart |= (unsigned short)pow(2, 3 - comparepart_offset % 4);
		comparepart <<= 4 * (3 - comparepart_offset / 4);

		*(bloomfilter.dicthash + position / (sizeof(unsigned short) * 8)) &= (unsigned short)(65535 - 15*pow(2, 4*(3 - comparepart_offset / 4)));
		*(bloomfilter.dicthash + position / (sizeof(unsigned short) * 8)) |= comparepart;
		
		return Bloomclass::checkbit(position);

	}
	else {
		printf("Error in Bloomclass::turnbit");

		return false;
	}
}



void Bloomclass::echodicthash() {
	for (int i = 0; i < bloomfilter.allocatesize; i++) {
		printf("%d ", *(bloomfilter.dicthash + i));
		int ret = *(bloomfilter.dicthash + i);
	}
}

bool Bloomclass::excludeString(char* buffer) {

	return false;
}


bool Bloomclass::turnbit_v1(int position) {//Deprecated
	if (position < bloomfilter.BLOOMFILTERBITCOUNT) {
		unsigned short comparepart = *(bloomfilter.dicthash + position / (sizeof(unsigned short) * 8));
		unsigned short comparepart_offset = position % (sizeof(unsigned short) * 8);

		switch (comparepart_offset / 4) {
		case 0: {
			comparepart >>= 12;
			//comparepart &= 15/*1111*/;
			switch (comparepart_offset % 4) {
			case 0:comparepart |= 8/*1000*/; break;
			case 1:comparepart |= 4/*0100*/; break;
			case 2:comparepart |= 2/*0010*/; break;
			case 3:comparepart |= 1/*0001*/; break;
			default:printf("warning: turnbit1");
			}
			comparepart <<= 12;
			*(bloomfilter.dicthash + position / (sizeof(unsigned short) * 8)) &= 4095/*0000 1111 1111 1111*/;
			*(bloomfilter.dicthash + position / (sizeof(unsigned short) * 8)) |= comparepart;

			break;
		}
		case 1: {
			comparepart >>= 8;
			comparepart &= 15/*0000 1111*/;
			switch (comparepart_offset % 4) {
			case 0:comparepart |= 8/*1000*/; break;
			case 1:comparepart |= 4/*0100*/; break;
			case 2:comparepart |= 2/*0010*/; break;
			case 3:comparepart |= 1/*0001*/; break;
			default:printf("warning: turnbit2");
			}
			comparepart <<= 8;
			*(bloomfilter.dicthash + position / (sizeof(unsigned short) * 8)) &= 61695/*1111 0000 1111 1111*/;
			*(bloomfilter.dicthash + position / (sizeof(unsigned short) * 8)) |= comparepart;
			break;
		}
		case 2: {
			comparepart >>= 4;
			comparepart &= 15/*0000 0000 1111*/;
			switch (comparepart_offset % 4) {
			case 0:comparepart |= 8/*1000*/; break;
			case 1:comparepart |= 4/*0100*/; break;
			case 2:comparepart |= 2/*0010*/; break;
			case 3:comparepart |= 1/*0001*/; break;
			default:printf("warning: turnbit3");
			}
			comparepart <<= 4;
			*(bloomfilter.dicthash + position / (sizeof(unsigned short) * 8)) &= 65295/*1111 1111 0000 1111*/;
			*(bloomfilter.dicthash + position / (sizeof(unsigned short) * 8)) |= comparepart;

			break;
		}
		case 3: {
			//comparepart >>= 0;
			comparepart &= 15/*0000 0000 0000 1111*/;
			switch (comparepart_offset % 4) {
			case 0:comparepart |= 8/*1000*/; break;
			case 1:comparepart |= 4/*0100*/; break;
			case 2:comparepart |= 2/*0010*/; break;
			case 3:comparepart |= 1/*0001*/; break;
			default:printf("warning: turnbit4");
			}
			//comparepart <<= 0;
			*(bloomfilter.dicthash + position / (sizeof(unsigned short) * 8)) &= 65520/*1111 1111 1111 0000*/;
			*(bloomfilter.dicthash + position / (sizeof(unsigned short) * 8)) |= comparepart;


			break;
		}

		default:printf("warning: offset turnbit05"); return false;
		}
		//Bloomclass::echodicthash();
		return Bloomclass::checkbit(position);

	}
	printf("Warning: Some error happened in turnbit06\n");
	return false;
}

bool Bloomclass::checkbit_v1(int position) {//Deprecated
	if (position < bloomfilter.BLOOMFILTERBITCOUNT) {
		unsigned short comparepart = *(bloomfilter.dicthash + position / (sizeof(unsigned short) * 8));
		unsigned short comparepart_offset = position % (sizeof(unsigned short) * 8);
		switch (comparepart_offset / 4) {
		case 0: {
			comparepart >>= 12;
			//comparepart &= 15/*1111*/;
			switch (comparepart_offset % 4) {
			case 0:return comparepart & 8/*1000*/;
			case 1:return comparepart & 4/*0100*/;
			case 2:return comparepart & 2/*0010*/;
			case 3:return comparepart & 1/*0001*/;
			default:printf("warning: offset dealbit01");
			}
			break;
		}
		case 1: {
			comparepart >>= 8;
			comparepart &= 15/*0000 1111*/;
			switch (comparepart_offset % 4) {
			case 0:return comparepart & 8/*1000*/;
			case 1:return comparepart & 4/*0100*/;
			case 2:return comparepart & 2/*0010*/;
			case 3:return comparepart & 1/*0001*/;
			default:printf("warning: offset dealbit02");
			}
			break;
		}
		case 2: {
			comparepart >>= 4;
			comparepart &= 15/*0000 0000 1111*/;
			switch (comparepart_offset % 4) {
			case 0:return comparepart & 8/*1000*/;
			case 1:return comparepart & 4/*0100*/;
			case 2:return comparepart & 2/*0010*/;
			case 3:return comparepart & 1/*0001*/;
			default:printf("warning: offset dealbit03");
			}
			break;
		}
		case 3: {
			//comparepart >>= 0;
			comparepart &= 15/*0000 0000 0000 1111*/;
			switch (comparepart_offset % 4) {
			case 0:return comparepart & 8/*1000*/;
			case 1:return comparepart & 4/*0100*/;
			case 2:return comparepart & 2/*0010*/;
			case 3:return comparepart & 1/*0001*/;
			default:printf("warning: offset dealbit04");
			}
			break;
		}

		default:printf("warning: offset dealbit05"); return false;
		}

	}
	printf("Warning: Some error happened in dealbit06\n");
	return false;
}

Bloomclass::Bloomclass(int income) {
	initspellchecker(income);
}

Bloomclass::~Bloomclass() {
	releasedict();
}