#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "double_linked_list.h"
//#include "phonebook.h"

// For Share Memory
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define NAME "SharedMemory"
#define NUM 100
#define SIZE sizeof(stNode)


void print_screen();
int select_sub();
int Import(stNode* nPtr);
void Export(stNode* nPtr, int cNode);
int CSV_search(char *, char [10][1024]);

int main (){

    int fd; 
    if ((fd = shm_open(NAME, O_CREAT | O_RDWR, 0600)) <0 ){
        perror("shm_open()");
        return EXIT_FAILURE;
    }
    
    ftruncate(fd, NUM*SIZE);
    
    stNode *nodePtr;
    nodePtr = (stNode *)mmap(0, NUM*SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);


	stList* export_list;

	print_screen();

    int mode;
	mode = select_sub();

    int cNode = 0;

	if (mode == 1)
	{
		cNode = Import(nodePtr);
        printf("Print Read %d\n", cNode);
        for (int k=0;k<cNode;k++){
            printf("[Index %d] %d %d %s %s %s %d %d \n",k, nodePtr[k].id,nodePtr[k].index, nodePtr[k].name, nodePtr[k].number, nodePtr[k].group,nodePtr[k].pPrev,nodePtr[k].pNext);
        }
	}	
	else if (mode == 2)
		Export(nodePtr,cNode);
	else
		printf("Select Wrong Number \n");
}

void print_screen()
{
	printf("========== SUB  MENU ==========\n");
	printf("========== 1. Import ==========\n");
	printf("========== 2. Export ==========\n");
	printf("===============================\n");
}

int select_sub()
{
	int mode;
	printf("Please select sub menu (1 | 2) :  ");
	scanf("%d", &mode);

	return mode;
}

int CSV_search(char *path, char list[10][1024])
{

	DIR * dir_ptr = NULL;
	struct dirent *file = NULL;
	struct stat buf;
	char *ext;
	char filename[1024];
	int i = 0;

	if((dir_ptr = opendir(path)) == NULL)
	{
		return -1;
	}
	
	while((file = readdir(dir_ptr)) != NULL)
	{
		if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
		{
			continue;
		}

		sprintf(filename, "%s/%s", path, file->d_name);

		if(stat(filename, &buf) == -1)
		{
			continue;
		}

		if(S_ISDIR(buf.st_mode))
		{
			CSV_search(filename,list);
		}
		else if(S_ISREG(buf.st_mode))
		{
			if((ext = strrchr(filename, '.')) == NULL)
			{
				continue;
			}

			if(strcmp(ext, ".csv") == 0)
			{
				sprintf(list[i],"%s",filename);
				i++;
			}
		}
	}

	closedir(dir_ptr);

	return 0;		
}

char *GetNextString(char* src, char token, char* buf)
{
	while(*src && *src != token)
	{
		*buf++ = *src++;
	}

	if(*(buf-1) == '\n') 
	{
		*(buf-1) = 0;
	}
	else
	{
		*buf = 0;
	}

	if(*src == token)
	{
		src++;
	}

	return src;
}

int Import(stNode* node)
{
	int i=0;
	int num;
	char path[100];
	char csv[10][1024];

	printf("File Path (default :pwd) : ");
	printf("\n");
	//TODO: Input file Path to User

	getcwd(path,sizeof(path));
	CSV_search(path,csv);

	printf("Select CSV file : \n");

	while(strlen(csv[i]) > 0)
	{
		printf("%d : %s\n",i+1, csv[i]);
		i++;
	}

	scanf("%d",&num);

	FILE* fp;
	if((fp = fopen(csv[num-1],"r")) == NULL)
	{
		printf("Cannot open the file\n");
	}
	
	char *p;
	char temp[100];
	char line[100];
	int j = 0;

    int currentnode = 0;

	node[currentnode].pPrev = -1;
	
    while(fgets(line,sizeof(line),fp) !=NULL)
	{
		p = GetNextString(line, ',', temp);
		node[currentnode].id = atoi(temp);

		p = GetNextString(p, ',', temp);
		node[currentnode].index = atoi(temp);
		
		p = GetNextString(p, ',', temp);
		strcpy(node[currentnode].name,temp);

		p = GetNextString(p, ',', temp);
		strcpy(node[currentnode].number,temp);
		
		p = GetNextString(p, ',', temp);
		strcpy(node[currentnode].group,temp);

		p = GetNextString(p, ',', temp);
		node[currentnode].matchedValue = atoi(temp);

		p = GetNextString(p, ',', temp);
		node[currentnode].favorite = atoi(temp);
   		
		if(currentnode == 0) node[currentnode].pPrev = -1;
		else node[currentnode].pPrev = currentnode-1;

		node[currentnode].pNext = currentnode+1;
        
		currentnode++;
	}
	node[currentnode-1].pNext = -1;
	
	return currentnode;
}
	

void Export(stNode* pList, int currentnode)
{
	FILE * fp;
	char name[100];
	stNode* cur;

	printf("Please write new filename : "); //Just name not ext
	scanf("%s",name);
	strcat(name,".csv"); // name.csv

	if((fp = fopen(name,"w")) == NULL)
	{
		printf("Cannot create the file\n");
	}
    int k;
	for (k=0; k<currentnode; k++)
	{
		fprintf(fp,"%d,%d,%s,%s,%s,%d\n",pList[k].id, pList[k].index, pList[k].name, pList[k].number, pList[k].group, pList[k].favorite);
	}
	fclose(fp);
}
