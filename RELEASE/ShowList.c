// (230627) double_linked_list.h 내부 함수와 충돌나는 이름은 모두 뒤에 "2"를 붙여줬습니다.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "double_linked_list.h"
//#include "phonebook.h"


// For Share Memory
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define NAME "SharedMemory"
#define NUM 100
#define SIZE sizeof(stNode)

#define NAME2 "SharedMemory2"
#define SIZE2 sizeof(stList)


//로컬용 구조체 선언
typedef struct ___Node {    
    int id;             // 노드 고유의 번호 10001부터 시작
    int index;         // 정렬한 후 일렬번호
    char name[NAME_LENGTH];     // 영문 이름
    char number[NUMBER_LENGTH]; // 전화 번호
    char group[GROUP_LENGTH];
    bool matchedValue; // 해당 노드가 검색조건을 만족한다면 이 flag를 1로 설정해서 다음함수로 넘겨줌 메인메뉴로 돌아갈 때 0으로 초기화 필요
    bool favorite; // 즐겨찾기
    
    struct ___Node* pPrev;
    struct ___Node* pNext;    
} localNode;  // 노드 구조체 

typedef struct ___List {
    localNode* pHead;
    localNode* pTail;    
    bool sort_needs;
    int sort_order;
    int count_node;
} localList;   // 리스트 구조체


void InitializeList2(localList* pList);              // 메모리 할당해서 리스트 만들기
localNode* MakeNode2(localNode* data);                //메모리 할당해서 노드 만들기
void AddtoTailNode2(localList* pList, localNode* data);  //Tail에 노드 추가
void AddtoHeadNode2(localList* pList, localNode* data);   // Head에 노드 추가
void DeleteNode2(localList* pList, int index);        // 현재 노드 삭제 하기 (id 기준)
void PrintList2(localList* pList);                 // 전체 리스트 출력하기
int IsEmpty2(localList *pList) ;
void CountNode2(localList *pList);

localNode* ShowList(localList* pList);
int SortList(localList* pList);
int SortPhonebook(localList* pList);
int SortFavorite(localList* pList);
void Renumbering(localList* pList);


void ChangeList(stNode* nodePtr, stList* listPtr, localNode* lNode, localList* lList); // 공유 메모리 데이터를 로컬로
void saveList(stNode* nodePtr, stList* listPtr, localNode* lNode, localList* lList); // 바뀐 인덱스로 공유메모리에 다시 저장




int main(int argc, char* argv[]){
	// 인수의 타입대로 정렬 방식 구현
	// 공용 메모리에서 불러오기
	
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
    
    stList *listPtr;
    listPtr = (stList *)mmap(0, SIZE2, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);

	//lisrPtr에 배열로 불러옴
	
	

	
	
	printf("read from shared memory... \n\n");
	for (int k=0 ; k<8 ; k++){
            printf("[Index %d] %d %d %s %s %s %d %d \n",k, nodePtr[k].id,nodePtr[k].index, nodePtr[k].name, nodePtr[k].number, nodePtr[k].group,nodePtr[k].pPrev,nodePtr[k].pNext);
        }
	
	printf("\n\nHead index : %d\n", listPtr->pHead);
	printf("Tail index : %d\n", listPtr->pTail);
	printf("Sort order is : %d\n",listPtr->sort_order);
	
	


	//localNode* lNode = NULL; //로컬로 사용될 노드는 로컬로
	localList* lList = malloc(sizeof(localList));
	localNode* lNode = NULL;
	localNode* returnNode = NULL;
	
	//스트럭처 명이 꼬인다. 찾아서 모두 바꿔주어야함.
	ChangeList(nodePtr, listPtr, lNode, lList); //로컬 메모리의 링크드 리스트를 만든다. 
	//ShowList
	
	lList->sort_needs = 1; //소트가 필요한 값을 어디선가 받아야 하는데 일단 상수로 지정
	lList->sort_order = listPtr->sort_order; //정렬을 원하는 타입을 공유메모리에서 로컬로 저장함
	
	returnNode = ShowList(lList); //리턴 밸류는 selected_node 소트 및 인덱스 넘버링 시작
	if(returnNode == 0){
		return -1;
	}else{
		listPtr->return_value = returnNode->index-1; //리턴 밸류에 인덱스 리턴-1
		
	}
	
	printf("Local result ... \n\n");
	PrintList2(lList);
	SortFavorite(lList);
	saveList(nodePtr, listPtr, lNode, lList);

	return 0;
}




void ChangeList(stNode* nodePtr, stList* listPtr, localNode* lNode, localList* lList){ //링크를 만드는 함수 데이터만 복사를 하면 될것 같음 스트럭처 변수는 전역 변수 그대로

	
	//InitializeList(lList);
	//stNode* curNode = NULL;
	
	
	int curindex = listPtr->pHead; // index 넘기기용
	//	printf("HEAD is : %d\n",curindex);
	lList->sort_order = listPtr->sort_order;
	
printf("이번 헤드 인덱스 값 : %d\n",curindex);
	
	while(curindex != -1){ //데이터를 옮기는 루틴
	
	//	printf("Currnet index is %d\n",curindex);

		
		localNode* curLocalNode = malloc(sizeof(localNode)); //노드 만들고
		curLocalNode->pNext = NULL;
		curLocalNode->pPrev = NULL;
		
		
		curLocalNode->id = nodePtr[curindex].id;

		curLocalNode->index = nodePtr[curindex].index;
		strcpy(curLocalNode->name, nodePtr[curindex].name);
		strcpy(curLocalNode->number, nodePtr[curindex].number);
		strcpy(curLocalNode->group, nodePtr[curindex].group);
		curLocalNode->matchedValue = nodePtr[curindex].matchedValue;
		curLocalNode->favorite = nodePtr[curindex].favorite;
		
		AddtoTailNode2(lList,curLocalNode);
		
		curindex = nodePtr[curindex].pNext;
		
	}
	
}

	
void saveList(stNode* nodePtr, stList* listPtr, localNode* lNode, localList* lList){// 바뀐 인덱스로 공유메모리에 다시 저장
	
	int curindex = lList->pHead->index; // index 넘기기용
		
	localNode* cur;
	
    for (cur = lList->pHead; cur !=NULL ; cur=cur->pNext) {
	
		nodePtr[cur->index-1].id = cur->id;
		nodePtr[cur->index-1].index = cur->index;
		strcpy(nodePtr[cur->index-1].name, cur->name);
		strcpy(nodePtr[cur->index-1].number, cur->number);
		strcpy(nodePtr[cur->index-1].group, cur->group);
		nodePtr[cur->index-1].matchedValue = cur->matchedValue;
		nodePtr[cur->index-1].favorite = cur->favorite;
		
		
		
	}
	
	
}




localNode* ShowList(localList* pList) {
    // 변수 선언
    localNode* selected_node, *cur;
    //int i, user_choice=0, limit_ask=3;     
    int i, user_choice, limit_ask=3;     
//printf("here2\n");    
    if(IsEmpty2(pList)==1) {   // list가 NULL 이면 NULL 리턴
        printf("This list is empty.\n");
        return NULL;  
    }     
    CountNode2(pList);
    if (pList->sort_needs==1) {  //sort_needs=1 정렬이 필요하면 정렬시작        

        //정렬기준에 따른 정렬하는 함수 콜
        //printf("*** sort by name ***\n");
        SortList(pList);    // sort_order를 통해 정렬 기준을 바꿀 수 있어야 함. 지금은 디폴트 name 만 실행
        //PrintList(pList);     //        
    }
    // 현재의 리스트를 print하고
    PrintList2(pList);    
    
    // 사용자가 index를 선택하도록 하는 기능 추가
    while(limit_ask > 0) {
        
        printf("\nWhich number would you like to select? (to main menu : '0') : ");
        scanf(" %d",&user_choice);
        
        if(user_choice == 0) {
            // printf("quit\n");
            return 0;    // q를 입력받으면 NULL을 리턴함                                 // (230628) NULL → 0
printf("here1\n");
        }        
        if(user_choice <= pList->count_node && user_choice > 0) {
            break;
        } else {
            printf("\nwrong number!\n");
            limit_ask--;
        }        
    }
    printf("\n");
    if (limit_ask <= 0) {
        return NULL; // 사용자가 index 선택을 2번 잘못했을 경우 NULL을 리턴
    }
    // 사용자가 선택한 index에 해당하는 노드의 주소값을 찾아서 리턴값 설정
    cur = pList->pHead;    
    for (i=1; i <= pList->count_node; i++) {        
        if(cur->index == user_choice) {
            selected_node = cur;
            break;
        }
        cur=cur->pNext;
printf("here\n");
printf("cur->index: %d\n",cur->index);
printf("user_choice: %d\n",user_choice);

    }    
    printf("\nselected node is %s\n", selected_node->name);
    return selected_node;          
}
	
int SortList(localList* pList) {
    SortPhonebook(pList);        
    //SortFavorite(pList);
    // 인덱스 다시 넘버링
    //printf("*** renumbering ***\n");              
    Renumbering(pList);
    return 0;
}

int SortPhonebook(localList* pList) {
// 버블정렬
    localNode* cur, *cur_next, *tmp1, *tmp2;    
    char *cur_sort, *cur_next_sort;
    int i,j;
    bool switching_position=false;
        
    CountNode2(pList); // 노드의 개수를 세기
    
    if ((*pList).count_node <= 1 ) {
        printf("The number of nodes is not enough.\n");
        return -1;
    }

    for (i=0 ; i < (*pList).count_node + 1 ; i++) {    //버블 정렬 : 이전노드tmp와 현재노드cur를 비교하여 스위치or그대로
        cur = pList->pHead;
        for (j=1 ; j < (*pList).count_node - i ; j++) { 
            cur_next = cur->pNext; 

            switch (pList->sort_order) {
                case 1:     cur_sort = cur->number;
                            cur_next_sort = cur->pNext->number;
                            break;
                case 2:     cur_sort = cur->group;
                            cur_next_sort = cur->pNext->group;
                            break;
                default:    cur_sort = cur->name;     // 0은 name, 1은 전화번호, 2는 그룹, 3 이상은 name
                            cur_next_sort = cur->pNext->name;
                            break;
            }
               
            if(cur==pList->pHead) {  // 바로 이전 노드가 head일 때는 tmp의 이전노드가 NULL
                //printf("1111\n");  
				printf("compare : %s and %s\n",cur_sort,cur_next_sort);
                if (strcmp(cur_sort, cur_next_sort)>0) {  // 자리를 스위치     
					printf("change\n");
                    
					tmp2 = cur_next->pNext; //tmp2는 cur의 다음다음 노드를 의미함.  노드2개일때는 tmp2가 NULL
                                            // cur_next와 cur의 위치를 바꿈
                    cur->pNext = tmp2;  // cur의다음주소 기존 cur다음다음노드의 주소,
                    cur->pPrev = cur_next;    // cur의 이전주소는 기존 cur다음노드의 주소   

                    cur_next->pPrev = NULL;
                    cur_next->pNext = cur;
                    if(cur_next->pNext!=NULL) {  // tmp2가 NULL이 아닐 경우에만 실행
                        tmp2->pPrev = cur;   // 기존cur다음다음노드의 이전주소는 cur  
                    }
                    pList->pHead = cur_next;

                    switching_position=true;                    
                    //PrintList2(pList);
                }                 
            } else if (cur_next==pList->pTail) {
                //printf("3333\n");
								printf("compare : %s and %s\n",cur_sort,cur_next_sort);
                if (strcmp(cur_sort, cur_next_sort)>0) {  // 자리를 스위치        
printf("change\n");
                    
                    tmp1 = cur->pPrev;    //tmp는 cur의 이전노드를 의미함
                                          // cur_next와 cur의 위치를 바꿈
                    cur->pNext = NULL;  // cur의다음주소 기존 cur다음다음노드의 주소,
                    cur->pPrev = cur_next;    // cur의 이전주소는 기존 cur다음노드의 주소   

                    cur_next->pPrev = tmp1;
                    cur_next->pNext = cur;

                    tmp1->pNext = cur_next;  // 기존 cur 이전노드의 다음주소는 기존cur의 다음노드   
                    pList->pTail = cur;                                 

                    switching_position=true; 
                    //PrintList(pList);
                }                    
            } else {
                //printf("2222\n");
								printf("compare : %s and %s\n",cur_sort,cur_next_sort);
                if (strcmp(cur_sort, cur_next_sort)>0) {  // 자리를 스위치   
                    printf("change\n");
                    
                    tmp1 = cur->pPrev;    //tmp는 cur의 이전노드를 의미함
                    tmp2 = cur_next->pNext; //tmp2는 cur의 다음다음 노드를 의미함.
                                            // cur_next와 cur의 위치를 바꿈
                    cur->pNext = tmp2;  // cur의다음주소 기존 cur다음다음노드의 주소,
                    cur->pPrev = cur_next;    // cur의 이전주소는 기존 cur다음노드의 주소   

                    cur_next->pPrev = tmp1;
                    cur_next->pNext = cur;

                    tmp1->pNext = cur_next;  // 기존 cur 이전노드의 다음주소는 기존cur의 다음노드          
                    tmp2->pPrev = cur;   // 기존cur다음다음노드의 이전주소는 cur

                    switching_position=true;
                    //PrintList(pList);
                }              
            }
            if(cur == pList->pTail) {
                //printf("Tail\n");                
            } else if (switching_position==false) { // 자리가 바뀐 경우에는 cur가 뒷쪽으로 오므로 바꿀 필요가 없음         
                cur = cur->pNext;                   // 자리가 바뀌지 않으면 다음노드로 cur를 변경해서 반복문 실행     
            }
            switching_position=false;          
        }    
    }
    printf("\n");
}

int SortFavorite(localList* pList) {
//tail부터 검색해서 head에 삽입
    localNode* cur, *tmp;
    int count;
    cur = pList->pTail;
    CountNode2(pList);
    if((*pList).count_node<2) {
        printf("The number of nodes is not enough.\n");
        return -1;
    }
//    printf("%d\n", (*pList).count_node);
    for (count=(*pList).count_node ; count > 0; count--) {
        // tail에서 거꾸로 주소타고 올라가야 하는데, cur->pPrev를 유지하기 위해 tmp이용
        tmp = cur->pPrev;   
        if(cur->favorite==1) {
           // cur의 앞과 뒤 노드를 연결 
            if(cur==pList->pTail) {
                pList->pTail = tmp;                
                tmp->pNext = NULL;       
            }    
            else {
                cur->pNext->pPrev = tmp;   
                tmp->pNext = cur->pNext;                         
            }
           // cur는 head에 삽입   (double linked list head 삽입 기능 사용)  
           AddtoHeadNode2(pList, cur);                      
        }
        //PrintList(pList);
        cur = tmp;   
    }
    return 0;
}

void Renumbering(localList* pList) {
    int count=1;   //넘버링 시작은 1번부터
    localNode* cur;
    for (cur = pList->pHead; cur !=NULL ; cur=cur->pNext) {
        cur->index = count;
        count++;
    } 
}



	
	


//double_linked_list.c에 있는 함수 들고옴
	

void InitializeList2(localList* pList) {
    localList* list = malloc(sizeof(localList));
    list->pHead = list->pTail = NULL;
    list->sort_needs = 1;
    list->sort_order = 0;
    list->count_node = 0;
}

localNode* MakeNode2(localNode* data) {
    localNode* node = malloc(sizeof(localNode));
    node = data;
    node->pPrev = node->pNext = NULL;
    // id 부여하는 방법을 추가 고민
    return node;
}  //노드를 만들기 위해 메모리 할당하고 초기화하기

void AddtoTailNode2(localList* pList, localNode* data) {   // 노드를 tail에 노드 추가
    localNode* node = MakeNode2(data);
    if(IsEmpty2(pList)) {
        pList->pHead = pList->pTail = node;   
    } else {
        node->pPrev = pList->pTail;
        node->pNext = NULL;
        pList->pTail->pNext = node;        
        pList->pTail        = node;
    }
}  //List의 뒤에 신규 노드 추가

void AddtoHeadNode2(localList* pList, localNode* data) { //노드를 Head에 추가    
    localNode* node = MakeNode2(data);
    if(IsEmpty2(pList)) {
        pList->pHead = pList->pTail = node;   
    } else {       
        node->pNext = pList->pHead;
        pList->pHead->pPrev = node; 
        pList->pHead = node;
        node->pPrev = NULL;    
    }
}  
void DeleteNode2(localList* pList, int index) {
    localNode* cur;
    if(IsEmpty2(pList)) {
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


void PrintList2(localList* pList) {
    localNode* cur;
    for (cur = pList->pHead; cur !=NULL ; cur=cur->pNext) {
        printf("%5d ", cur->id);
        printf("%3d ",cur->index);
        printf("%30s ",cur->name);
        printf("%20s ",cur->number);
        printf("%20s ",cur->group);
        printf("%3d", cur->favorite);
        printf("\n");
    }
    printf("\n");
}

int IsEmpty2(localList* pList) {
    return (pList->pHead == NULL);
} // 리스트가 비어있는 지 확인

void CountNode2(localList *pList){
	int count=0;
    localNode* cur;
    for (cur = pList->pHead; cur !=NULL ; cur=cur->pNext) {
        count++;
    } 
    (*pList).count_node = count;  
}
