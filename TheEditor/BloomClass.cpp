#include"BloomClass.h"

short Bloomclass::hashfunction_num = 8;
/***********************************************
	��������: Bloomclass::hash1(char* buffer, int command = 0)
	��������: ���ַ�������ascii���Ϊ���֣����õ���������Ա��س���ȡ�࣬���int����Ϊ1��bloomfilter��Ӧ��־λ��1������������֮���ѯ��λ���Ƿ�Ϊ1.
	�������: char*Ŀ���ַ�����int 0����ѯ��1������
	����ֵ: �����1����true������Ϊfalse��
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
	��������: Bloomclass::hash2(char* buffer, int command = 0)
	��������: ���ַ�������ascii���Ϊ���֣����õ���ͣ����Ա��س���ȡ�࣬���int����Ϊ1��bloomfilter��Ӧ��־λ��1������������֮���ѯ��λ���Ƿ�Ϊ1.
	�������: char*Ŀ���ַ�����int 0����ѯ��1������
	����ֵ: �����1����true������Ϊfalse��
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
	��������: Bloomclass::hash3(char* buffer, int command = 0)
	��������: ���ַ�������ascii���Ϊ���֣����õ�ƽ����ȡ���ĺͣ����Ա��س���ȡ�࣬���int����Ϊ1��bloomfilter��Ӧ��־λ��1������������֮���ѯ��λ���Ƿ�Ϊ1.
	�������: char*Ŀ���ַ�����int 0����ѯ��1������
	����ֵ: �����1����true������Ϊfalse��
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
	��������: Bloomclass::hash4(char* buffer, int command = 0)
	��������: ���ַ�������ascii���Ϊ���֣�����+-+-������˳�����μӼ������Ա��س���ȡ�࣬���int����Ϊ1��bloomfilter��Ӧ��־λ��1������������֮���ѯ��λ���Ƿ�Ϊ1.
	�������: char*Ŀ���ַ�����int 0����ѯ��1������
	����ֵ: �����1����true������Ϊfalse��
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
	if (Bloomclass::gethashfunction_num() < 1) {//��������ɢ�к��������󣬿����ô˽���˳���ų�
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
	if (Bloomclass::gethashfunction_num() < 2) {//��������ɢ�к��������󣬿����ô˽���˳���ų�
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
	if (Bloomclass::gethashfunction_num() < 3) {//��������ɢ�к��������󣬿����ô˽���˳���ų�
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
	if (Bloomclass::gethashfunction_num() < 4) {//��������ɢ�к��������󣬿����ô˽���˳���ų�
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
	if (Bloomclass::gethashfunction_num() < 5) {//��������ɢ�к��������󣬿����ô˽���˳���ų�
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
	if (Bloomclass::gethashfunction_num() < 6) {//��������ɢ�к��������󣬿����ô˽���˳���ų�
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
	if (Bloomclass::gethashfunction_num() < 7) {//��������ɢ�к��������󣬿����ô˽���˳���ų�
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
	if (Bloomclass::gethashfunction_num() < 8) {//��������ɢ�к��������󣬿����ô˽���˳���ų�
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
	��������: Bloomclass::AddString(char* buffer)
	��������: ���Ŀ���ַ�����bloomfilter������λΪ1
	�������: char*Ŀ���ַ���
	����ֵ: ������Ϸ���true������Ϊfalse��
************************************************/
bool Bloomclass::AddString(char* buffer)
{
	return hash5(buffer, 1) && hash6(buffer, 1) && hash7(buffer, 1) && hash8(buffer, 1) && 
		hash9(buffer, 1) && hash10(buffer, 1) && hash11(buffer, 1) && hash12(buffer, 1);
}

/***********************************************
	��������: Bloomclass::checkstring(char* buffer)
	��������: ���Ŀ���ַ����Ƿ���bloomfilter������λΪ1
	�������: char*Ŀ���ַ���
	����ֵ: ������Ϸ���true������Ϊfalse��
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
	��������: Bloomclass::exportdict(TCHAR* path)
	��������: ��bloomfilter�ṹ��Ķ�����λ���ݵ�����Ŀ��·��
	�������: TCHAR*�ļ�·��
	����ֵ: ���������ɷ��ص�bloomfilter�Ķ�����λ�ĸ���������Ϊ-1��
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
	��������: Bloomclass::importdict(TCHAR* path)
	��������: ��bloomfilter�ṹ�嵼������ָ��·���ļ��ڵĶ�����λ���ݣ�����ļ��Ŀ�ͷ�ԡ�bfd����ͷ��֮����һ��int��ʾ��ռ�õĶ�����λ�ĸ���
	�������: TCHAR*���͵�·��
	����ֵ: ���������ɷ���0������Ϊ-1��
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
	��������: Bloomclass::releasedict(int bitcount)
	��������: �ͷ�bloomfilter�ṹ����ռ�õ��ڴ�
	�������: ��
	����ֵ: ���������ɷ���0������Ϊ0��
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
	��������: Bloomclass::initspellchecker(int bitcount)
	��������: ��ʼ��bloomfilter�ṹ��
	�������: ��Ҫ��bloomfilter�Ķ�����λ������Ĭ��64
	����ֵ: �����ɹ��������뵽��bloomfilter�Ķ�����λ������
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