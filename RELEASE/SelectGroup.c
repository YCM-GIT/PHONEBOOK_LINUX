#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include "double_linked_list.h"

// For Share Memory
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define NAME "SharedMemory"
#define NUM 100
#define SIZE sizeof(stNode)

#define NAME2 "SharedMemory2"
#define SIZE2 sizeof(stList)

//extern void SelectGroup(stList* pList, char* group_name){
int main(){

    // 공유 메모리 받아오기. 
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
	
    // 리스트가 없을 때 처리: 함수 종료.
    //if(pList->pHead == NULL){
    if(pList->pHead == -1){
        //return 0;
        return 0;
    }

    
    // 역할??
    //stNode* tmp_stNode;
    //tmp_stNode = &nodePtr[n];    

    // 메인 리스트와 다른 그룹 리스트 필요함.
    // : group들을 중복 없이, 오름차순으로 정렬할 sub 리스트임. stSearchedList 구조체 활용.
    // 그룹 리스트의 head를 정의해 주고, 초기화.
        stSearchedList* p_group_list_head = (stSearchedList*)malloc(sizeof(stSearchedList));
        p_group_list_head->NextNode = NULL;
        p_group_list_head->PrevNode = NULL;
        //p_group_list_head->node = pList->pHead;                 // 이 head 노드의 node 멤버는 메인 리스트의 Head Node를 가리키게 함.
        p_group_list_head->node = nodePtr + pList->pHead;

        stSearchedList* p_group_list_tail = p_group_list_head;  // 초기화한 현 시점에는 1개 node이므로 이 노드를 역시 tail로 지정해 줌. (이 함수에서는 순차적으로만 전진해서, tail은 쓰지 않는다.)
        
    
    
    //stNode* current_node = pList->pHead;    // 메인 리스트를 전진시킬 current_node 선언.
    //stNode* current_node = nodePtr + pList->pHead;
    stNode* current_node;
    stSearchedList* current_group_node = p_group_list_head; // 그룹 리스트를 전진시킬 current_group_node 선언.

    

    // 원래 노드의 주소값으로 판별하던 것을 → 노드 배열의 인덱스값으로 판별해야 하므로, 인덱스를 저장할 변수 n 선언.
    int n = pList->pHead;

    //while(current_node!=NULL){                                                      // 메인 리스트의 노드가 끝에 도달할(NULL이 될) 때까지 Head로부터 순차적으로 전진시킴.
    while(n != -1){
        current_node = nodePtr + n;     //nodePtr(노드 배열의 처음 값 주소) + n 인덱스만큼을 더해 줌.

        for(current_group_node = p_group_list_head; current_group_node != NULL;     // 그룹 리스트의 노드가 끝에 도달할(NULL이 될) 때까지 head로부터 순차적으로 전진시킴.
        current_group_node = current_group_node->NextNode){                         // 메인 리스트의 노드가 전진하면, 그룹 리스트의 head부터 다시 검색하기 위해 초기화. while로도 구현할 수 있음. for문이 그나마 시인성이 좋은 듯.

            if(strcmp(current_node->group, current_group_node->node->group) == 0){  // 메인 리스트의 노드의 group이, 그룹 리스트의 노드의 group과 같으면, 아무 액션 없이, for문을 나감(메인 리스트 노드 전진).
                break;
	        }
            else if(strcmp(current_node->group, current_group_node->node->group) < 0){  // 메인 리스트의 노드의 group이, 그룹 리스트의 노드의 group보다 작으면,       
                stSearchedList* tmp = (stSearchedList*)malloc(sizeof(stSearchedList));  // 그룹 리스트의 노드를 하나를 더 만들고 현재 그룹 리스트의 전에 위치시킴. 즉, 작은 것을 전으로 = 오름차순으로 저장.
                tmp->NextNode = current_group_node;
                tmp->PrevNode = current_group_node->PrevNode;
                tmp->node = current_node;
                
                if(current_group_node->PrevNode == NULL){                               // 그룹 리스트의 노드가 head였다면, 새로 만든 노드를 head로 만듦.
                    p_group_list_head = tmp;  
                }
                else{                                                                   // 아니라면, 일반적인 경우로서 사이에 끼워줌. 
                    current_group_node->PrevNode->NextNode = tmp;
                }
                current_group_node->PrevNode = tmp;                                     // 순서 중요함!! current_group_node는 맨 마지막에 수정해 주자.
                break;
	        }
            else if(strcmp(current_node->group, current_group_node->node->group) > 0){  // 메인 리스트의 노드의 group이, 그룹 리스트의 노드의 group보다 크면,       
                                                                                        // 또 2경우로 나눠서 생각해야 함. 맨 마지막인 경우와, 아닌(다음 노드가 있는) 경우.
                if(current_group_node->NextNode == NULL){                               // 맨 마지막인 경우라면, 

                    stSearchedList* tmp = (stSearchedList*)malloc(sizeof(stSearchedList));  // 그룹 리스트의 노드를 하나를 더 만들고 현재 그룹 리스트의 후에 위치시킴.
                    tmp->PrevNode = current_group_node;
                    tmp->NextNode = current_group_node->NextNode;
                    tmp->node = current_node;
                    p_group_list_tail = tmp; 
                    current_group_node->NextNode = tmp;
                    break;
                }
                else{                                                                   // 아니라면, 아무 액션 없이, for문으로 귀환(그룹 리스트 노드 전진).
                    continue;
                }
               
	        }   
            else {                                                                      // 정상적인 경우라면, 들어올 일 없음.
                printf("error\n");

            }
            
        }
        
        
        //current_node = current_node->pNext;                                             // 메인 리스트 노드 전진.
        //current_node = nodePtr + current_node->pNext;
        n = current_node->pNext;

        
        //printf("current_node-nodePtr = %d\n", (int)(current_node-nodePtr));
    }
        
   
    printf("------- Group list -------\n");                                             // 그룹 리스트의 head부터 시작해서, NULL에 도달할 때까지 1줄에 1개씩 (중복 없는 그룹 리스트) group을 보여줌.
	stSearchedList* p = p_group_list_head;
	while(p!=NULL){
		printf("%s\n", p->node->group);		
		p = p->NextNode;
	}
	printf("--------------------------\n");
    printf("Type Group name correctly:! ");
    printf("\n(If you want to come back Main Menu, Input '0')....");
    
	//scanf("%s", group_name);               // 리스트 중에 group_name을 타이핑하도록 함.
	scanf("%s", pList->group_name);
        if(pList->group_name == 0){
            return 0;
        }
    //printf("%s\n", group_name);
    printf("pList->group_name: %s\n", pList->group_name);
    return 1;
}


