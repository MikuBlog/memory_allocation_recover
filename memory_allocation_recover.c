#include<stdio.h>  
#include<conio.h>
#include<string.h>
#include<stdlib.h> 

#define SIZE 1
#define ERROR 0 // 出错   
typedef int Status; 

// 定义一个空闲区说明表结构
typedef struct free_table
{     
    int num; // 分区序号    
    long begin; // 起始地址    
    long size; // 分区大小    
    int status; // 分区状态 
}ElemType;

// 线性表的双向链表存储结构 
typedef struct Node
{    
    ElemType data;     
    struct Node *prior; // 前趋指针   
    struct Node *next;  // 后继指针 
}Node, *LinkList;    

LinkList first; // 头结点 
LinkList end; // 尾结点  
int flag; // 记录要删除的分区序号   

// 开创带头结点的内存空间链表
Status Initblock() {     
    first=(LinkList)malloc(sizeof(Node));   
    end=(LinkList)malloc(sizeof(Node));   
    first->prior=NULL;   
    first->next=end;   
    end->prior=first;
    end->next=NULL; 
    end->data.num=1;   
    end->data.begin=120;
    end->data.size=1000; // 给对应的尾节点分配1000KB的内存大小   
    end->data.status=0;  // 状态为0，表示内存处于可分配状态
    return SIZE; 
}

// 菜单
void menu() {
    printf("\n                 |*************内存分配和回收***********|\n");
    printf("                 |======================================|\n");
    printf("                 |              0.退出                  |\n");
    printf("                 |              1.首次适应算法          |\n");
    printf("                 |              2.最佳适应算法          |\n");
    printf("                 |======================================|\n");
}

//分区序号重新排序
void sort() {    
    Node *p = first->next,*q;   
    q = p->next;    
    for(; p != NULL; p = p->next)   
    {     
        for(q = p->next; q; q = q->next)    
        {       
            if(p->data.num >= q->data.num)       
            {          
                q->data.num += 1; // 将分区序号向后移动一位        
            }    
        }     
    } 
 } 

void initial() {
	Node *p = first;     
    p->data.num = 0;     
    p->data.begin = 0;     
    p->data.size = 120;     
    p->data.status = 1; 
}

// 显示主存分配情况 
void show() {  
    int flag = 0; // 用来记录分区序号  
	Node *p = first;
	// 初始化第一块内存的信息
	initial();
	// 每次显示的时候，都对内存分区序号进行一次排序
    sort(); 
    printf("\n\t\t主存空间分配情况\n");      
    printf("**********************************************************\n\n");     
    printf("分区序号\t起始地址\t分区大小\t分区状态\n\n");     
    while(p) {        
        printf("%d\t\t%d\t\t%d",p->data.num,p->data.begin,p->data.size);         
        if(p->data.status == 0) 
			printf("\t\t空闲\n\n");         
        else 
			printf("\t\t已分配\n\n"); 
         p = p->next;      
    }      
    printf("**********************************************************\n\n"); 
}

// 首次适应算法  
Status First_fit(int request) {   
    // 为申请作业开辟新空间且初始化    
    Node *p = first->next;    
    LinkList temp = (LinkList)malloc(sizeof(Node));    
    temp->data.size = request;   
    temp->data.status = 1;      
    while(p) {     
        if((p->data.status == 0) && (p->data.size == request)) {
            // 有大小恰好合适的空闲块       
            p->data.status = 1;       
            return SIZE;        
            break;  
        }     
        else if((p->data.status == 0) && (p->data.size > request)) {
            // 有空闲块能满足需求且有剩余，在分配内存的同时，将该节点插入到该空闲块对应节点的前面        
            temp->prior = p->prior;        
            temp->next = p;         
            temp->data.begin = p->data.begin;        
            temp->data.num = p->data.num;        
            p->prior->next = temp;         
            p->prior = temp;         
            p->data.begin = temp->data.begin + temp->data.size;        
            p->data.size -= request;        
            p->data.num += 1;        
            return SIZE;        
            break;     
        }     
        p = p->next;  
    }
	// 如果都没有内存块能满足分配的内存要求，返回错误标志
    return ERROR; 
}   

//最佳适应算法  
Status Best_fit(int request) {    
    int ch; // 记录最小剩余空间    
    Node *p = first;     
    Node *q = NULL; // 记录最佳插入位置     
    LinkList temp = (LinkList)malloc(sizeof(Node)); 
    temp->data.size = request;    
    temp->data.status = 1;     
    p->data.num = 1;
	//初始化最小空间和最佳位置
    while(p) {      
        if((p->data.status == 0) && (p->data.size >= request)) {          
			if(q == NULL) {              
                q = p;               
                ch = p->data.size - request;             
			}          
            else if(q->data.size > p->data.size) {           
                q = p;            
                ch = p->data.size - request;          
            }      
         } 
         p = p->next;   
     }    
	// 没有找到空闲块   
    if(q == NULL) 
		return ERROR;
    else if(q->data.size == request) {         
        q->data.status = 1;         
        return SIZE;     
    }  
    else {       
		// 在空闲块之前插入该节点
        temp->prior = q->prior;      
        temp->next = q;       
        temp->data.begin = q->data.begin;      
        temp->data.num = q->data.num;      
        q->prior->next = temp;      
        q->prior = temp;        
        q->data.begin += request;      
        q->data.size = ch;      
        q->data.num += 1; 
        return SIZE;   
     }   
     return SIZE; 
}   

// 分配主存  
Status allocation(int a) {    
	// 申请内存大小
    int request;    
    printf("请输入申请分配的内存大小(单位:KB):");    
    scanf("%d", &request);   
    if(request < 0 || request == 0) {        
        printf("分配的内存不能小于等于0，请重试!");       
        return ERROR;     
    }    
    switch(a) { 
		// 首次适应算法 
		case 1: 
			  if(First_fit(request) == SIZE) 
				  printf("\n\n\t****分配成功！****\n\n");             
			  else 
				  printf("\n\n\t****内存不足，分配失败！****\n\n");             
			  return SIZE;    
			  break;  
		// 最佳适应算法
		case 2:               
			if(Best_fit(request) == SIZE) 
				printf("\n\n\t****分配成功！****\n\n");             
			else 
				printf("\n\n\t****内存不足，分配失败！****\n\n");             
			return SIZE;             
			break;  
    } 
}

//处理回收空间
Status deal1(Node *p) {     
    Node *q = first;     
    for(; q != NULL; q = q->next) {       
        if(q == p) {    
			// 如果前一个节点为空闲块，后一个节点不为空闲块，直接合并为前一块空闲块
            if(q->prior->data.status == 0 && q->next->data.status != 0){
                q->prior->data.size += q->data.size;         
                q->prior->next = q->next;         
                q->next->prior = q->prior;         
                q = q->prior;         
                q->data.status = 0;          
                q->data.num = flag - 1;       
            }        
			// 如果前一个节点为空闲块，后一个节点为空闲块，直接合并为前一块空闲块
			if(q->prior->data.status == 0 && q->next->data.status == 0) {    
                q->prior->data.size += q->data.size;         
                q->prior->next = q->next;         
                q->next->prior = q->prior; 
                q = q->prior;         
                q->data.status = 0;          
                q->data.num = flag - 1;      
            }  
			// 如果后一块代码为空闲块，那么后一块空闲块合并到当前回收的空闲块中
            if(q->prior->data.status != 0 && q->next->data.status == 0) {         
                q->data.size += q->next->data.size;     
                q->next=q->next->next;         
                q->next->next->prior = q;        
                q->data.status = 0;         
                q->data.num = flag;      
            }               
            if(q->prior->data.status != 0 && q->next->data.status != 0) {    
                q->data.status = 0;      
            }      
        }    
    }     
    return SIZE; 
}   

// 处理回收空间
Status deal2(Node *p) 
{     
    Node *q = first;
    for(; q != NULL; q = q->next) {       
        if(q == p) {      
			// 如果前一个分区为空闲块，合并为前一个空闲块
            if(q->prior->data.status == 0) {    
                q->prior->data.size += q->data.size;         
                q->prior->next = q->next; 
                q->next->prior = q->prior;         
                q = q->prior;         
                q->data.status = 0;          
                q->data.num = flag - 1;       
			}        
			// 如果后一个为空闲块且不为空闲块，那么直接回收，不作合并
            if(q->prior->data.status != 0 && q->next->data.status == 0) {         
                q->data.status = 0;     
			}        
            if(q->prior->data.status != 0 && q->next->data.status != 0) {    
                q->data.status=0;      
            }      
        }    
    }     
    return SIZE; 
}   

// 主存回收  
Status recovery(int flag) 
{     
    Node *p = first;    
    for(; p != NULL; p = p->next) {      
        if(p->data.num == flag) {     
            if(p->prior == first) { 
				// 当前P指向的下一个不是最后一个时
                if(p->next != end) {        
					// 与后面的空闲块相连
                    if(p->next->data.status == 0) {                  
                        p->data.size += p->next->data.size;                 
                        p->next->next->prior = p;                 
                        p->next=p->next->next;  
                        p->data.status = 0;                 
                        p->data.num = flag;         
                    }        
                    else 
						p->data.status = 0;       
                }      
				// 当前P指向的下一个是最后一个时
                if(p->next == end) {               
                    p->data.status = 0;         
                }    
            }
            // 结束if(p->prior==block_first)的情况    
            else if(p->prior != first) {      
                if(p->next != end)       
                    deal1(p);            
                else    
                    deal2(p);       
            }//结束if(p->prior!=block_first)的情况   
        }//结束if(p->data.num==flag)的情况   
	}    
    printf("\n\n\t****回收成功****\n\n");    
    return SIZE;    
}  

void main() 
{      
    int i;  // 操作选择标记     
    int a; // 算法选择标记           
    while(1) {
		menu();
		printf("\n\n请选择输入所使用的内存分配算法 (0~2):");  
        scanf("%d",&a);   
    while(a<0||a>3) {        
        printf("\n输入错误，请重新选择输入所使用的内存分配算法 (0~2):");       
        scanf("%d",&a);      
    }
    switch(a) {    
        case 1:printf("\n\t****使用首次适应算法：****\n");
            break;       
        case 2:printf("\n\t****使用最佳适应算法：****\n");
            break;       
    } 
    Initblock(); //开创空间表     
    while(1) {           
        show();          
            printf("\t1: 分配内存\t2: 回收内存\t0: 退出当前内存分配算法\n");         
            printf("\n\n请输入您的操作：");         
            scanf("%d",&i);         
            if(i==1)            
                allocation(a); // 分配内存         
            else if(i == 2) {            
				// 内存回收
                printf("请输入您要释放的分区号：");             
                scanf("%d",&flag);             
                recovery(flag); 
            }          
            else if(i == 0) {    
                printf("\n退出当前内存分配算法，返回主菜单\n");    
                break; //退出   
            }          
            else {
				//输入操作有误              
                printf("输入有误，请重新输入！");            
				continue;         
            }     
        } 
     }
}