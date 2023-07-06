#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include "double_linked_list.h"
#include "phonebook.h"

#define ANSI_COLOR_RED      "\x1b[31m"
#define ANSI_COLOR_GREEN    "\x1b[32m"
#define ANSI_COLOR_YELLOW   "\x1b[33m"
#define ANSI_COLOR_BLUE     "\x1b[34m"
#define ANSI_COLOR_MAGENTA  "\x1b[35m"
#define ANSI_COLOR_CYAN     "\x1b[36m"
#define ANSI_COLOR_RESET    "\x1b[0m"


//static void subMenu(stNode* Node, stList* pList);
static void subMenu(stNode* Node, stList* pList, stNode *nodePtr);
static void EditMenu(stNode* Node, stList* pList, stNode *nodePtr);     // (230702) stNode *nodePtr 추가. 
static void EditName(stNode* Node);
static void EditNumber(stNode* Node);
static void EditGroup(stNode* Node);
static void EditFavorate(stNode* Node);
static void StartCalling(stNode* Node);
static int PrintShowNode(stNode* Node);

// For Share Memory
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define NAME "SharedMemory"
#define NAME2 "SharedMemory2"
#define NUM 100
#define SIZE sizeof(stNode)
#define SIZE2 sizeof(stList)

//extern int ShowNode(stNode* Node, stList* pList) {
//int main(stNode* Node, stList* pList){    
int main(int argc, char* argv[]){    

    // 매개변수 잘 받았는지 확인
        printf("argc = %d\n", argc);

    int i;
    for(i = 0; i < argc; i++){
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    // 확인 끝

    // char형으로 받은 argv[1]을 int형으로 변환. 배열의 index로 사용하기 위함.
    // int n = *argv[1] - 48;               // (230628) 리턴값을 매개변수가 아닌, 공유메모리 stList.return_value에서 받기로 변경함

    // 공유 메모리 받아오기
    int fd; 
    if ((fd = shm_open(NAME, O_CREAT | O_RDWR, 0600)) <0 ){
        perror("shm_open()");
        return EXIT_FAILURE;
    }
        
    stNode *nodePtr;
    nodePtr = (stNode *)mmap(0, NUM*SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);


    int fd2; 
    if ((fd2 = shm_open(NAME2, O_CREAT | O_RDWR, 0600)) <0 ){
        perror("shm_open()");
        return EXIT_FAILURE;
    }
        
    stList *pList;
    pList = (stList *)mmap(0, SIZE2, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
	//stList 공유 메모리 끝.
    
printf("pList->return_value = %d\n", pList->return_value);
    int n = pList->return_value;    // pList 구조체의 return_value 멤버를 전역변수 리턴값으로 활용함.
    
    printf("n = %d\n", n);    // 전역변수 리턴값 n을 잘 받았는지 확인.

    // tmp_stNode를 선언 후, n번째 구조체 배열을 가리키게 함.
    stNode* tmp_stNode;
    tmp_stNode = &nodePtr[n];    

   //system("clear");


    //printf("%5d ", Node->id);
    printf("%5d ", tmp_stNode->id);
    printf("%3d ", tmp_stNode->index);
    printf("%30s ", tmp_stNode->name);
    printf("%20s ", tmp_stNode->number);
    printf("%20s ", tmp_stNode->group);
    printf("%3d", tmp_stNode->favorite);
    printf("\n");
    //printf("%3d ", Node->index);
    //printf("%30s ", Node->name);
    //printf("%20s ", Node->number);
    //printf("%20s ", Node->group);
    //printf("%3d", Node->favorite);
    //printf("\n");

    subMenu(tmp_stNode, pList, nodePtr);    // (230702) nodePtr 추가.
  
    return 0;
}


//static void subMenu(stNode* Node, stList* pList) {
static void subMenu(stNode* Node, stList* pList, stNode *nodePtr) {         //(230702) 공유메모리 상 stNode 첫 주소인 nodePtr를 함께 넘겨줘야 함.
    
    int i=0;   
    int index = Node->index;
printf("pList->pHead2:%d\n", pList->pHead);
    while (i< 4) {
        
        printf("\n");
        printf("****Sub Menu****\n");
        printf("1: Delete \n");
        printf("2: Edit \n");
        printf("3. Calling \n");
        printf("4. Go to Main Menu \n");
        printf("Press number: ");
        scanf("%d", &i);

        
        if (i == 1) {
            
printf("pList->pHead:%d\n", pList->pHead);
// (230702) DeleteNode()를 직접 작성함. double_linked_list.c 내의 DeleteNode는 삭제.
            int n = pList->pHead;       // 원래 노드의 주소값으로 판별하던 것을 → 노드 배열의 인덱스값으로 판별해야 하므로, 인덱스를 저장할 변수 n 선언.
	        stNode* currentNode;        // 현재 stNode의 주소값 저장위한 변수
            // 메인 리스트의 노드가 끝에 도달할(-1이 될) 때까지 Head로부터 순차적으로 전진시킴.
            while(n != -1){
                currentNode = nodePtr + n;     //nodePtr(노드 배열의 처음 값 주소) + n 인덱스만큼을 더해 줌.
printf("n:%d\n", n);
printf("currentNode->id:%d\n", currentNode->id);
printf("Node->id:%d\n", Node->id);
		        if (currentNode->id == Node->id){   // (230702) id는 유니크하므로 id를 기준으로 찾음.

                // (230702) 삭제할 것이므로, 앞뒤 노드의 pNext, pPrev를 서로 연결되게 해줌.
                    //currentNode->pPrev->pNext = currentNode->pNext;
printf("currentNode->id:%d\n", currentNode->id);
printf("Node->id:%d\n", Node->id);
printf("currentNode->pNext:%d\n", currentNode->pNext);
printf("currentNode->pPrev:%d\n", currentNode->pPrev);
printf("currentNode - Node:%d\n", (int)(currentNode - Node));
printf("(Node + currentNode->pPrev)->pNext:%d\n", (Node + currentNode->pPrev)->pNext);
                    (nodePtr + currentNode->pPrev)->pNext = currentNode->pNext;				// 공유 메모리
					//currentNode->pPrev->pNext = currentNode->pNext;							// 공유 메모리
printf("currentNode->pNext:%d\n", currentNode->pNext);
printf("(Node + currentNode->pPrev)->pNext:%d\n", (Node + currentNode->pPrev)->pNext);		// 공유 메모리
                    //currentNode->pNext->pPrev = currentNode->pPrev;
                    (nodePtr + currentNode->pNext)->pPrev = currentNode->pPrev;
					//currentNode->pNext->pPrev = currentNode->pPrev;							// 공유 메모리


//                cur->pPrev->pNext = cur->pNext;
//                cur->pNext->pPrev = cur->pPrev;
//                free(cur);   // 메모리 할당 해제


                // (230702) 해당 노드 초기화 (초기화 방식 논의 필요)
		            printf("%5d ", currentNode->id = 0);
                    printf("%3d ", currentNode->index = 0);
                    printf("%30s ", strcpy(currentNode->name, ""));
                    printf("%20s ", strcpy(currentNode->number, ""));
                    printf("%20s ", strcpy(currentNode->group, ""));
                    printf("%3d", currentNode->matchedValue = 0);
                    printf("%3d", currentNode->pPrev = -1);
                    printf("%3d", currentNode->pNext = -1);
                    printf("\n"); 




                    break;
		        }
                n = currentNode->pNext;
            }
// DeleteNode() 끝.

           printf("Delete Success \n");
           //system("clear");
           
        }

       else if (i == 2) {
           EditMenu(Node,pList, nodePtr);       // (230702) nodePtr 추가.
           //SortList(pList);
       }

       else if (i == 3) {
            StartCalling(Node);
       }      

      else {
            break;
           
        }
       
    }
    return;
    
}


static void EditMenu(stNode* Node, stList* pList, stNode* nodePtr) {    // (230702) stNode* nodePtr 추가.
    int num = 0;
    int j = 0;
   

    system("clear");


    while (j < 5) {

        printf("%5d ", Node->id);
        printf("%3d ", Node->index);
        printf("%30s ", Node->name);
        printf("%20s ", Node->number);
        printf("%20s ", Node->group);
        printf("%3d", Node->favorite);
        printf("\n");

        while (num < 5) {
            printf("\n");
            printf("1: Name \n");
            printf("2: Phone Number \n");
            printf("3. Group \n");
            printf("4. Favorate \n");
            printf("5. sub Menu \n");
            printf("Press number: ");
            scanf("%d", &num);

            switch (num)
            {
            case 1:
                EditName(Node);

                break;
            case 2:
                EditNumber(Node);

                break;
            case 3:
                EditGroup(Node);

                break;
            case 4:
                EditFavorate(Node);

                break;
            default:
                break;
            }
            break;
        }
        j = num;

    }
    system("clear");
    PrintShowNode(Node);
    subMenu(Node, pList, nodePtr);      //(230702) nodePtr 추가.
    return;

}


static void EditName(stNode *Node) {

    printf("New name: ");
    scanf("%s", Node->name);                        

    system("clear");
}


static void EditNumber(stNode* Node) {

    printf("New number: ");
    scanf("%s", Node->number);    

    system("clear");
}


static void EditGroup(stNode* Node) {
    
    printf("New Group: ");
    scanf("%s", Node->group);

    system("clear");
}

static void EditFavorate(stNode* Node) {
   
    int scalar = 0;
    bool b = false;   
 
    printf("New Favorate: ");
    scanf("%d", &scalar);
    b=(bool)scalar;
    Node->favorite = b;

    system("clear");

}

static void StartCalling(stNode* Node) {
    int num;
    int i;    

    while (1) 
    {
        system("clear");
        PrintShowNode(Node);
       
        for (i = 0;i < 5; i++) {
            printf(ANSI_COLOR_RED "*Calling*" ANSI_COLOR_RESET "");
            printf(ANSI_COLOR_GREEN "*Calling*" ANSI_COLOR_RESET "");
            printf(ANSI_COLOR_YELLOW "*Calling*" ANSI_COLOR_RESET "");
            printf(ANSI_COLOR_BLUE "*Calling*" ANSI_COLOR_RESET "");
            printf(ANSI_COLOR_MAGENTA "*Calling*" ANSI_COLOR_RESET "");
            printf(ANSI_COLOR_CYAN "*Calling*" ANSI_COLOR_RESET "\n");

            sleep(1);

        };

        printf("If you want to hang up, press 1:   ");
        scanf("%d", &num);

        if (num == 1) { 
            system("clear");
            PrintShowNode(Node);
            break;
        }

     }
 }
 

static int PrintShowNode(stNode* Node) {

   
    system("clear");

    printf("%5d ", Node->id);
    printf("%3d ", Node->index);
    printf("%30s ", Node->name);
    printf("%20s ", Node->number);
    printf("%20s ", Node->group);
    printf("%3d", Node->favorite);
    printf("\n");   
    

    return 0;
}




/*
extern void DeleteNode(ostList* pList, int index) {
    ostNode* cur;
    if(IsEmpty(pList)) {
        printf("There is no member.\n");    
    } else {
        cur = pList->pHead;
        while(cur->pNext !=NULL) {
            if (index == cur->index) {   // 같은id를 가진 노드를 제거
                cur->pPrev->pNext = cur->pNext;
                cur->pNext->pPrev = cur->pPrev;
                free(cur);   // 메모리 할당 해제
                break;
            }
            cur = cur->pNext;
        }
    }
}  // index를 받아서 같은 index를 가진 노드 삭제 하기 

*/
