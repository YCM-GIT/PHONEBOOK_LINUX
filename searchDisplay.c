#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
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

// searchDisplay 함수 내부에서 쓰이는 함수를 위에 선언. ?? 아래에 선언 및 정의해도 컴파일이 잘 되셨나요? (to YTY)
stSearchedList* SearchingValue(int mode, stNode* dataNode, char* searchingValue, stList* listPtr);
int stringFind(char* a, char* b); 


/* 검색 화면으로 이동해서 검색한 1개의 stNode값을 리턴한다. */
int main(){
    system("clear");
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

    char searchingValue[256]; // 검색할 값을 저장할 변수

    stNode* currentNode;
/*
    for(int k=0; nodePtr[k].pNext !=-1; k++){
        printf("[Index %d] %d %d %s %s %s %d %d \n",k, nodePtr[k].id,nodePtr[k].index, nodePtr[k].name, nodePtr[k].number, nodePtr[k].group,nodePtr[k].pPrev,nodePtr[k].pNext);
    }
*/
    //currentNode = &nodePtr[listPtr->pHead];
    int k=0;
    int index = listPtr->pHead;
    while(index != -1){
        currentNode = &nodePtr[index];
        printf("[%d] %d %d %s %s %s %d\n", k, currentNode->index, currentNode->id, currentNode->name, currentNode->number, currentNode->group, currentNode->pNext);
        k++;
        //currentNode = &nodePtr[currentNode->pNext];
        index = currentNode->pNext;
    }
    printf("\n\n               Searching               \n\n");
    printf("Input any values to search");
    printf("\n(If you want to come back Main Menu, Input <!q>)....");
    scanf("%s", searchingValue); // 검색할 값을 입력 받는다.

    stSearchedList* sList = NULL; // 검색 결과 stNode를 연결한 리스트의 Head 값 초기화
    stSearchedList* cList = NULL; // 검색 결과를 탐색하기 위해 사용할 현재 검색 결과 리스트를 가르키는 값 초기화
    stNode* selectedNode = NULL; // 최종 선택된 stNode 값 초기화
   
    int selectedIndex; // 어떤 필드값과 매칭되었는지를 저장하는 변수

    if(strcmp(searchingValue,"!q") == 0){
        selectedNode = NULL;
        return 0;
    }

    sList = SearchingValue(0, nodePtr, searchingValue, listPtr); //datalist 중 검색값과 매칭되는 stNode들의 리스트를 리턴 받음
    cList = sList; // 검색 결과를 탐색하기 위해 현재 리스트에 검색 결과 리스트의 Head 값으로 설정
    system("clear");
    if(cList != NULL){
        printf("Index\t ID\t\t  Name\t\t   Number\t\t    Group\n");
    }
    else{
        printf("\nNo Mached Value\n");
    }

    while(cList != NULL){
        stNode* tmp = cList->node;
        printf("%3d\t%5d%20s%20s%20s %3d\n",tmp->index,tmp->id,tmp->name,tmp->number,tmp->group,cList->matchedWith);
        cList = cList->NextNode;
    } // 검색된 결과 값들을 표시

    cList = sList; // 검색 결과를 탐색하기 위해 현재 리스트에 검색 결과 리스트의 Head 값으로 설정

    if(cList != NULL){
        printf("\nSelect index that you searched...."); // 검색 결과 리스트 중 1개의 index값을 입력 받음
        printf("\n(If you want to come back Main Menu, Input '0')....");
        scanf("%d",&selectedIndex);
        if(selectedIndex == 0){
            selectedNode = NULL;
            printf("Return index : -1\n");
            return -1;
        }
        else{
            while(cList != NULL){
                if(selectedIndex == (cList->node)->index){
                    printf("Searched Success with ");
                    switch (cList->matchedWith)
                    {
                    case 1 :
                        /* code */
                        printf("Index\n");
                        break;
                    case 2 :
                        printf("Index\n");
                        break;

                    case 3 :
                        printf("Name\n");
                        break;

                    case 4 :
                        printf("Number\n");
                        break;

                    case 5 :
                        printf("Group\n");
                        break;
                    default:
                        printf("Nothing\n");
                        break;
                    }
                }
                cList = cList->NextNode;
            }    
        }        
    }
    else{
        selectedNode = NULL;
        printf("Searched Fail\n\n"); // 입력 값이 검색 결과 중 index값과 매칭이 안될 시
        printf("Return index : -1\n");
        return -1; 
    }
    /*
    nodePtr[selectedNode->index-1].matchedValue = (int)cList->matchedWith;
    selectedNode->matchedValue = cList->matchedWith;
    printf("cList->matchedWith = %d\n", cList->matchedWith);
    printf("selectedNode->matchedValue = %d\n",selectedNode->matchedValue);
    printf("Node->matchedValue = %d\n", nodePtr[selectedNode->index-1].matchedValue);
    //printf("selected Index : %d %d & matched with %d %d\n",selectedNode->index, nodePtr[selectedNode->index-1].index, cList->matchedWith, nodePtr[selectedNode->index-1].matchedValue);
    */
    int returnValue = -1;
    int indexVal = listPtr->pHead;
    while(indexVal != -1){
        currentNode = &nodePtr[indexVal];
        if(selectedIndex == currentNode->index){
            returnValue = indexVal;
        }
        indexVal = currentNode->pNext;
    }

    printf("Return index : %d\n", returnValue);
    return returnValue;

    //return 0; // 최종 선택된 stNode 값을 리턴
}

/* 데이터 리스트 중 검색 값을 입력하면 해당되는 stNode들의 리스트를 리턴*/
/* mode 값이 1이면 index 필드에서만 검색 */
/* mode 값이 2이면 id 필드에서만 검색 */
/* mode 값이 3이면 name 필드에서만 검색 */
/* mode 값이 4이면 number 필드에서만 검색 */
/* mode 값이 5이면 group 필드에서만 검색 */
/* mode 값이 위에 해당되는 값이 아니면 전체 필드에서 검색 */
stSearchedList* SearchingValue(int mode, stNode* dataNode, char* searchingValue, stList* listPtr){
    
    int searchedFlag = 0;
    
    stNode* currentNode; // 현재 stNode의 주소값 저장위한 변수
    stSearchedList* HeadList = NULL; // 검색 결과 stNode들만 연결할 리스트의 Head 변수
    stSearchedList* CurrentList = NULL; // 검색 결과 리스트의 현재 가르키는 리스트 변수

    int index = 0;

    printf("Head : %d, Tail : %d\n",listPtr->pHead, listPtr->pTail);

    /* mode 1, 2, 3, 4, 5의 설명은 else문 전체 필드 검색에서 참조 바람 */
    if(mode == 1){
        int index = listPtr->pHead;
        while(index != -1){
            currentNode = &dataNode[index];            
            if(currentNode->index == atoi(searchingValue)){
                if(searchedFlag == 0){
                    searchedFlag = 1;
                }            
            }
            if(searchedFlag > 0){ // 검색 값이 어느 필드의 값과 매칭이 되었다면...
                stSearchedList* newList = (stSearchedList*) malloc(sizeof(stSearchedList)); //검색 결과 임의의 리스트의 메모리 할당
                newList->NextNode = NULL; // 초기화
                newList->PrevNode = NULL; // 초기화
                newList->node = NULL; // 초기화
                newList->matchedWith = 0; //초기화
                    
                if(HeadList == NULL){ // Head 값이 없다면
                    HeadList = newList; // 새로운 리스트를 헤드값으로 
                    CurrentList = HeadList; 
                    CurrentList->node = currentNode; // 검색된 stNode의 주소값을 저장
                    CurrentList->matchedWith = searchedFlag; // 어떤 검색 필드와 매칭이 되었는지 저장
                    searchedFlag = 0; // 검색 필드 값 리셋
                }
                else{ // 검색 결과 리스트를 연결
                    if(CurrentList->NextNode != NULL){
                        CurrentList = CurrentList->NextNode;
                    }
                    CurrentList->NextNode = newList;
                    newList->PrevNode = CurrentList;
                    newList->node = currentNode; // 검색된 stNode의 주소값을 저장
                    newList->matchedWith = searchedFlag; // 어떤 검색 필드와 매칭이 되었는지 저장
                    searchedFlag = 0; // 검색 필드 값 리셋
                }
            }
            searchedFlag = 0;
            index = currentNode->pNext;
         }
    }
    else if(mode == 2){
        int index = listPtr->pHead;
        while(index != -1){
            currentNode = &dataNode[index];
            if (currentNode->id == atoi(searchingValue)){
                if(searchedFlag == 0){
                    searchedFlag = 2;
                }
            }
            if(searchedFlag > 0){ // 검색 값이 어느 필드의 값과 매칭이 되었다면...
                stSearchedList* newList = (stSearchedList*) malloc(sizeof(stSearchedList)); //검색 결과 임의의 리스트의 메모리 할당
                newList->NextNode = NULL; // 초기화
                newList->PrevNode = NULL; // 초기화
                newList->node = NULL; // 초기화
                newList->matchedWith = 0; //초기화
                    
                if(HeadList == NULL){ // Head 값이 없다면
                    HeadList = newList; // 새로운 리스트를 헤드값으로 
                    CurrentList = HeadList; 
                    CurrentList->node = currentNode; // 검색된 stNode의 주소값을 저장
                    CurrentList->matchedWith = searchedFlag; // 어떤 검색 필드와 매칭이 되었는지 저장
                    searchedFlag = 0; // 검색 필드 값 리셋
                }
                else{ // 검색 결과 리스트를 연결
                    if(CurrentList->NextNode != NULL){
                        CurrentList = CurrentList->NextNode;
                    }
                    CurrentList->NextNode = newList;
                    newList->PrevNode = CurrentList;
                    newList->node = currentNode; // 검색된 stNode의 주소값을 저장
                    newList->matchedWith = searchedFlag; // 어떤 검색 필드와 매칭이 되었는지 저장
                    searchedFlag = 0; // 검색 필드 값 리셋
                }
            }
            searchedFlag = 0;
            index = currentNode->pNext;
         }
    }
    else if(mode == 3){
        int index = listPtr->pHead;
        while(index != -1){
            currentNode = &dataNode[index];
            if (stringFind(currentNode->name, searchingValue) >= 0){
                if(searchedFlag == 0){
                    searchedFlag = 3;
                }
            }
            if(searchedFlag > 0){ // 검색 값이 어느 필드의 값과 매칭이 되었다면...
                stSearchedList* newList = (stSearchedList*) malloc(sizeof(stSearchedList)); //검색 결과 임의의 리스트의 메모리 할당
                newList->NextNode = NULL; // 초기화
                newList->PrevNode = NULL; // 초기화
                newList->node = NULL; // 초기화
                newList->matchedWith = 0; //초기화
                    
                if(HeadList == NULL){ // Head 값이 없다면
                    HeadList = newList; // 새로운 리스트를 헤드값으로 
                    CurrentList = HeadList; 
                    CurrentList->node = currentNode; // 검색된 stNode의 주소값을 저장
                    CurrentList->matchedWith = searchedFlag; // 어떤 검색 필드와 매칭이 되었는지 저장
                    searchedFlag = 0; // 검색 필드 값 리셋
                }
                else{ // 검색 결과 리스트를 연결
                    if(CurrentList->NextNode != NULL){
                        CurrentList = CurrentList->NextNode;
                    }
                    CurrentList->NextNode = newList;
                    newList->PrevNode = CurrentList;
                    newList->node = currentNode; // 검색된 stNode의 주소값을 저장
                    newList->matchedWith = searchedFlag; // 어떤 검색 필드와 매칭이 되었는지 저장
                    searchedFlag = 0; // 검색 필드 값 리셋
                }
            }
            searchedFlag = 0;
            index = currentNode->pNext;
        }
    }
    else if(mode == 4){
        int index = listPtr->pHead;
        while(index != -1){
            currentNode = &dataNode[index];
            if (stringFind(currentNode->number, searchingValue) >= 0){
                if(searchedFlag == 0){
                    searchedFlag = 4;
                }
            }
            if(searchedFlag > 0){ // 검색 값이 어느 필드의 값과 매칭이 되었다면...
                stSearchedList* newList = (stSearchedList*) malloc(sizeof(stSearchedList)); //검색 결과 임의의 리스트의 메모리 할당
                newList->NextNode = NULL; // 초기화
                newList->PrevNode = NULL; // 초기화
                newList->node = NULL; // 초기화
                newList->matchedWith = 0; //초기화
                    
                if(HeadList == NULL){ // Head 값이 없다면
                    HeadList = newList; // 새로운 리스트를 헤드값으로 
                    CurrentList = HeadList; 
                    CurrentList->node = currentNode; // 검색된 stNode의 주소값을 저장
                    CurrentList->matchedWith = searchedFlag; // 어떤 검색 필드와 매칭이 되었는지 저장
                    searchedFlag = 0; // 검색 필드 값 리셋
                }
                else{ // 검색 결과 리스트를 연결
                    if(CurrentList->NextNode != NULL){
                        CurrentList = CurrentList->NextNode;
                    }
                    CurrentList->NextNode = newList;
                    newList->PrevNode = CurrentList;
                    newList->node = currentNode; // 검색된 stNode의 주소값을 저장
                    newList->matchedWith = searchedFlag; // 어떤 검색 필드와 매칭이 되었는지 저장
                    searchedFlag = 0; // 검색 필드 값 리셋
                }
            }
            searchedFlag = 0;
            index = currentNode->pNext;
        }
    }
    else if(mode == 5){
        int index = listPtr->pHead;
        while(index != -1){
            currentNode = &dataNode[index];
            if (stringFind(currentNode->group,searchingValue) >= 0){
                if(searchedFlag == 0){
                    searchedFlag = 5;
                }
            }
            if(searchedFlag > 0){ // 검색 값이 어느 필드의 값과 매칭이 되었다면...
                stSearchedList* newList = (stSearchedList*) malloc(sizeof(stSearchedList)); //검색 결과 임의의 리스트의 메모리 할당
                newList->NextNode = NULL; // 초기화
                newList->PrevNode = NULL; // 초기화
                newList->node = NULL; // 초기화
                newList->matchedWith = 0; //초기화
                    
                if(HeadList == NULL){ // Head 값이 없다면
                    HeadList = newList; // 새로운 리스트를 헤드값으로 
                    CurrentList = HeadList; 
                    CurrentList->node = currentNode; // 검색된 stNode의 주소값을 저장
                    CurrentList->matchedWith = searchedFlag; // 어떤 검색 필드와 매칭이 되었는지 저장
                    searchedFlag = 0; // 검색 필드 값 리셋
                }
                else{ // 검색 결과 리스트를 연결
                    if(CurrentList->NextNode != NULL){
                        CurrentList = CurrentList->NextNode;
                    }
                    CurrentList->NextNode = newList;
                    newList->PrevNode = CurrentList;
                    newList->node = currentNode; // 검색된 stNode의 주소값을 저장
                    newList->matchedWith = searchedFlag; // 어떤 검색 필드와 매칭이 되었는지 저장
                    searchedFlag = 0; // 검색 필드 값 리셋
                }
            }
            searchedFlag = 0;
            index = currentNode->pNext;
        }
    }
    else{
        int index = listPtr->pHead;
        while(index != -1){
            currentNode = &dataNode[index];
            if(currentNode->index == atoi(searchingValue)){ // 검색 값이 현재 stNode의 index와 매칭 여부 확인
                if(searchedFlag == 0){
                    searchedFlag = 1; // 검색 값이 어느 필드와 매칭이 되었는지 저장
                }            
            }
            else if (currentNode->id == atoi(searchingValue)){ // 검색 값이 현재 stNode의 id와 매칭 여부 확인
                if(searchedFlag == 0){
                    searchedFlag = 2; // 검색 값이 어느 필드와 매칭이 되었는지 저장
                }
            }
            else if (stringFind(currentNode->name, searchingValue) >= 0){ // 검색 값이 현재 stNode의 name과 매칭 여부 확인
                if(searchedFlag == 0){
                    searchedFlag = 3; // 검색 값이 어느 필드와 매칭이 되었는지 저장
                }
            }
            else if (stringFind(currentNode->number, searchingValue) >= 0){ // 검색 값이 현재 stNode의 number와 매칭 여부 확인
                if(searchedFlag == 0){
                    searchedFlag = 4; // 검색 값이 어느 필드와 매칭이 되었는지 저장
                }
            }
            else if (stringFind(currentNode->group,searchingValue) >= 0){ // 검색 값이 현재 stNode의 group와 매칭 여부 확인
                if(searchedFlag == 0){
                    searchedFlag = 5; // 검색 값이 어느 필드와 매칭이 되었는지 저장
                }
            }
            else{
                searchedFlag = 0;
            }
            if(searchedFlag > 0){ // 검색 값이 어느 필드의 값과 매칭이 되었다면...
                stSearchedList* newList = (stSearchedList*) malloc(sizeof(stSearchedList)); //검색 결과 임의의 리스트의 메모리 할당
                newList->NextNode = NULL; // 초기화
                newList->PrevNode = NULL; // 초기화
                newList->node = NULL; // 초기화
                newList->matchedWith = 0; //초기화
                    
                if(HeadList == NULL){ // Head 값이 없다면
                    HeadList = newList; // 새로운 리스트를 헤드값으로 
                    CurrentList = HeadList; 
                    CurrentList->node = currentNode; // 검색된 stNode의 주소값을 저장
                    CurrentList->matchedWith = searchedFlag; // 어떤 검색 필드와 매칭이 되었는지 저장
                    searchedFlag = 0; // 검색 필드 값 리셋
                }
                else{ // 검색 결과 리스트를 연결
                    if(CurrentList->NextNode != NULL){
                        CurrentList = CurrentList->NextNode;
                    }
                    CurrentList->NextNode = newList;
                    newList->PrevNode = CurrentList;
                    newList->node = currentNode; // 검색된 stNode의 주소값을 저장
                    newList->matchedWith = searchedFlag; // 어떤 검색 필드와 매칭이 되었는지 저장
                    searchedFlag = 0; // 검색 필드 값 리셋
                }
            }
            searchedFlag = 0;
            index = currentNode->pNext;
        }
    }
    return HeadList;
}

/* 문자열 2개를 입력받아 각 문자열의 포함 여부 확인, 1이상이면 포함, -1이면 미포함*/
int stringFind(char* a, char* b){
    int i;
    bool ok;
    int returnValue;

    for(i=0 ; a[i] != '\0' && !ok ; i++){
        ok = true;
        for(int j=0 ; b[j] != '\0' && ok ; j++){
            if(a[i+j] != b[j]) ok = false;
        }
    }
    return ok ? i-1 : -1;
}
