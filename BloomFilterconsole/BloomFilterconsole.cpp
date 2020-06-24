#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<math.h>
#define MAXLEN 64

int importdict(unsigned int *&);
int releasedict(unsigned int *&);

/************************************************************************************
function name: hash1.
function: Generate or add a character of the buffer string.
import variable: The original String, The int which 0 present check string exist and 1 present add the string
output: the buffer string whether exist 0 is not, 1 is does.
************************************************************************************/
bool hash1(char* buffer, unsigned int*& dicthash, int command=0){
	unsigned int tmp = 1;
	while (*buffer != '\0')
	{
		tmp *= (int)(*buffer);
		tmp %= MAXLEN;
		buffer++;
	}
	if (command == 1)
		*(dicthash + tmp % MAXLEN ) = 1;
	return *(dicthash+ tmp % MAXLEN ) ==1;
	
}


bool hash2(char* buffer, unsigned  int*& dicthash, int command = 0) {
	unsigned int tmp = 0;
	while (*buffer != '\0')
	{
		tmp += (int)(*buffer);

		buffer++;
	}
	if (command == 1)
		*(dicthash + tmp % MAXLEN ) = 1;
	return *(dicthash + tmp % MAXLEN ) == 1;
}

bool hash3(char* buffer, unsigned  int*& dicthash, int command = 0) {
	unsigned int tmp = 1;
	while (*buffer != '\0')
	{
		tmp *= (int)(sqrt((double)*buffer));

		buffer++;
	}
	if (command == 1)
		*(dicthash + tmp % MAXLEN ) = 1;
	return *(dicthash + tmp % MAXLEN ) == 1;
}

bool hash4(char* buffer, unsigned int*& dicthash, int command = 0) {
	unsigned int tmp = 0;
	unsigned int sign = 1;
	while (*buffer != '\0')
	{
		tmp += sign*(int)(*buffer);
		sign *= -1;
		buffer++;
	}
	if (command == 1)
		*(dicthash + tmp % MAXLEN ) = 1;
	return *(dicthash + tmp % MAXLEN ) == 1;
}

int checkstring(char *buffer, unsigned int*& dicthash)
{

	//puts(buffer);
	if (hash1(buffer,dicthash) && hash2(buffer, dicthash) && hash3(buffer, dicthash) && hash4(buffer, dicthash))
	{
		printf("%s (blond)\n", buffer);
		return 1;
	}
	else
	{
		printf("%s (normal)\n", buffer);
		return 0;
	}
		
	//return 1;
}


int create(unsigned int*& dicthash)
{
	if (importdict(dicthash) == -1)
	{
		return -1;
	}
	char path[32] = { 0 };
	FILE* fp;

	char buffer[MAXLEN];
	char endofstring = '\0';
	printf("please write the filename");
	scanf("%s", path);
	fp = fopen(path, "wb");
	if (fp == NULL)
	{
		printf("No found");
		return -1;
	}
	fflush(stdin);
	while (1)
	{
		gets_s(buffer);
		//gets_s(buffer);
		//puts(buffer);
		if (buffer[2] == '\0' && buffer[0] == ':' && buffer[1] == 'q')
		{
			break;
		}

		fwrite(buffer, sizeof(char), strlen(buffer), fp);
		fwrite(&endofstring, sizeof(char), 1, fp);
		

	}




	fclose(fp);
	//releasedict(dicthash);
	return 0;
}
int open(unsigned int*& dicthash)
{
	if (importdict(dicthash)==-1)
	{
		return -1;
	}
	char path[32] = { 0 };

	char tmp[32] = { 0 };
	FILE* fp;
	int i = 0;
	char buffer[MAXLEN] = { 0 };

	unsigned int ii;
	for (ii = 0; ii < MAXLEN; ii++)
	{
		printf("%u ", *(dicthash + ii ));
	}
	printf("\n");

	printf("please write the filename");
	scanf("%s", path);
	fp = fopen(path, "rb");
	//rewind(fp);
	//fprintf(fp, "%s", tmp);
	//printf("%s", tmp);
	if (fp == NULL)
	{
		printf("No found");
		return -1;
	}
	//printf("%d", feof(fp));
	while (!feof(fp))
	{
		i = 0;
		do {
			fread(&buffer[i], sizeof(char), 1, fp);
			//printf("%s", buffer[i]);
			i++;
		} while ( !feof(fp) && buffer[i-1] != ' '&& buffer[i-1] != '\0' && buffer[i-1]!=';');
		buffer[i-1] = '\0';
		//i++;
		//printf("%s",buffer[0]);
		//puts(buffer);
		checkstring(buffer, dicthash);
	}
	//if (i != 0)
	//{
	//	checkstring(buffer);
	//}



	fclose(fp);
	//releasedict(dicthash);
	return 0;
}

int importdict(unsigned int*& dicthash)
{
	//dicthash = (unsigned int*)calloc(MAXLEN, sizeof(unsigned int));
	
	char path[32] = "dict.bin";
	//char tmp[32] = { 0 };
	//free(dicthash);
	FILE* fp;
	//int i = 0;
	char buffer[MAXLEN] = { 0 };
	fp = fopen(path, "r");

	if (fp == NULL)
	{

		printf("No dict file found");
		
		return -1;
		

	}
	rewind(fp);
	fread(dicthash, sizeof(unsigned int), MAXLEN, fp);
	
	fclose(fp);
	return 0;

}

int releasedict(unsigned int*& dicthash)
{
	free(dicthash);
	dicthash = NULL;
	return 0;
}

int dict(unsigned int*& dicthash)
{
	//if(dicthash!=NULL)
	//	free(dicthash);
	
	//dicthash = (unsigned int*)malloc(MAXLEN* sizeof(unsigned int));
	unsigned int i;
	for (i = 0; i < MAXLEN; i++)
	{
		*(dicthash + i ) = 0;
	}

	char path[32] = "dict.bin";
	//char tmp[32] = { 0 };
	fflush(stdin);
	FILE* fp=NULL;
	//int i = 0;
	char buffer[MAXLEN] = { 0 };
	printf("dict.bin> ");
	//scanf("%s", path);
	fp = fopen(path, "r");
	
	//rewind(fp);
	//fprintf(fp, "%s", tmp);
	//printf("%s", tmp);
	if (fp == NULL)
	{
		printf("No found, try to create one.");
		fp = fopen(path, "w+");
		if (fp != NULL)
		{
			
			printf("Create success\n");

		}
		else	
		{
			printf("Create failed\n");
			return -1;
		}
		
	}
	else
	{
		
		
		unsigned int i;
	
		rewind(fp);
		//printf("sad:%u %u", sizeof(unsigned int), (unsigned int)MAXLEN);
		fread(dicthash, sizeof(unsigned int), (unsigned int)MAXLEN, fp);
		for (i = 0; i < MAXLEN; i++)
		{
			printf("%u ", *(dicthash + i));
		}
		fclose(fp);
		fp=fopen(path, "w+");
		for (i = 0; i < MAXLEN; i++)
		{
			printf("%u ", *(dicthash + i ));
		}
		printf("\n");





	}
	//printf("%d", feof(fp));
	
	while (1)
	{
		printf("Please input the keyword ");
		scanf("%s",buffer);
		//gets_s(buffer);
		//gets_s(buffer);
		//puts(buffer);
		if (buffer[2] == '\0' && buffer[0] == ':' && buffer[1] == 'q')
		{
			break;
		}
		printf("dsa");
		unsigned int i;
		for (i = 0; i < MAXLEN; i++)
		{
			printf("%u ", *(dicthash + i ));
		}
		printf("\n");

		hash1(buffer, dicthash, 1);
		hash2(buffer, dicthash, 1);
		hash3(buffer, dicthash, 1);
		hash4(buffer, dicthash, 1);
		
		
		printf("asd");
		
		for (i = 0; i < MAXLEN; i++)
		{
			printf("%u ", *(dicthash+i));
		}
		printf("\n");
	}
	//if (i != 0)
	//{
	//	checkstring(buffer);
	//}

	rewind(fp);
	//printf("here");
	fwrite(dicthash, sizeof(unsigned int), MAXLEN, fp);
	fclose(fp);
	return 0;
}

int reset(unsigned int*& dicthash)
{
	unsigned int i;
	for (i = 0; i < MAXLEN; i++)
	{
		*(dicthash + i ) = 0;
		//printf("dicthash..: %d ", dicthash + i * sizeof(unsigned int));
		//printf("- %d \n", *(dicthash + i * sizeof(unsigned int)));
	}
	return 0;
}


int show(unsigned int*& dicthash)
{
	unsigned int i;
	for (i = 0; i < MAXLEN; i++)
	{
	 
		printf("dicthash..: %d ", dicthash + i );
		printf("- %d \n", *(dicthash + i ));
	}
	return 0;
}

int main()
{

	unsigned int* dicthash=NULL;
	//dicthash = (unsigned int*)malloc((MAXLEN ) * sizeof(unsigned int));
	dicthash = (unsigned int*)malloc((MAXLEN) * sizeof(unsigned int));
	//printf("dicthash:%d ", dicthash);
	//printf("dicthash:%d ", dicthash+ (MAXLEN) * sizeof(unsigned int));
	unsigned int i;
	for (i = 0; i < MAXLEN; i++)
	{
		*(dicthash + i ) = 0;
	/*	printf("dicthash..: %d " , dicthash + i * sizeof(unsigned int));
		printf("- %d \n", *(dicthash + i * sizeof(unsigned int)));*/
	}

	int choice=0;
	
		do {
			printf("0:exit\n1:create\n2:open\n3:edit dict\n4:clear status\n");
			printf("choice: ");
			scanf("%d", &choice);
			switch (choice)
			{
			case 0: return 0;
			case 1:create(dicthash); break;
			case 2:open(dicthash); break;
			case 3:dict(dicthash); break;
			case 4:reset(dicthash); break;
			case 5:show(dicthash); break;
			}

		} while (choice != 0);
		releasedict(dicthash);
	return 0;
}
