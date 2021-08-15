#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_VERTEX 10000
#define MAX_EDGES 3000

typedef struct node
{
    int nodeNumber;
    char status;
    struct node* next;
}node;

typedef struct nodeHead
{
    struct node* Head;
}nodeHead;

typedef struct graph
{
    struct node* head[MAX_VERTEX];
}graph;

void create_graph(graph* Graph,int vertices,int maxEdges);
void printEdges(graph* Graph,int vertices);

void CreateInit_Infecteds(node* headInfec,int N);
void printlist(node* headInfec);

typedef struct event
{
    int time_prio;
    char action;
    node* Node;
    struct event *NEXT;
}event;

typedef struct prio_queue
{
    struct event* HEAD;
}prio_queue;

void insertInQueue(prio_queue* q, event* Event);
void printEvent(prio_queue* q);

int expo_variate(float rate);
void trans_SIR(prio_queue* Q,int T,float TransmitRate,event Source,int target,int Tmax,nodeHead* S);
void TransProcess_SIR(graph* G,event* EVENT,float recoveryRate,float transmitRate,int* t,nodeHead* S,nodeHead* I,nodeHead* R,prio_queue* Queue,int tmax);
void RecovProcess_SIR(event* EVENT,int* t,nodeHead* S,nodeHead* I,nodeHead* R,prio_queue* Queue);

int main()
{
    srand(time(NULL));
    int Vertices = rand() % MAX_VERTEX;
    int MaxEdges = rand() % MAX_EDGES;
    
    graph* GRAPH = (graph *)malloc(sizeof(graph));
    create_graph(GRAPH,Vertices,MaxEdges);
    printEdges(GRAPH,Vertices);
    
    node* infected = (node *)malloc(sizeof(node));
    CreateInit_Infecteds(infected,Vertices);
    
    nodeHead* I_Head = (nodeHead *)malloc(sizeof(nodeHead));
    nodeHead* R_Head = (nodeHead *)malloc(sizeof(nodeHead));
    nodeHead* S_Head = (nodeHead *)malloc(sizeof(nodeHead));
    for(int i=0;i<Vertices;++i)
    {
        node* NewNode = (node *)malloc(sizeof(node));
        NewNode->nodeNumber = i;
        NewNode->status = 'S';
        if (S_Head->Head == NULL) 
        {
            S_Head->Head = NewNode;
            NewNode->next=NULL;
        }
        else 
        {
            node *tempo = S_Head->Head;
            while(tempo->next != NULL)
                tempo=tempo->next;
            NewNode->next=tempo->next;
            tempo->next=NewNode;
        } 
    }//all nodes are initially succeptible
    
    /randomly infected nodes are sent into queue Q/
    prio_queue* Q = (prio_queue *)malloc(sizeof(prio_queue));
    node* temp = infected;
    while(temp != NULL)
    {
        event* NewEvent = (event *)malloc(sizeof(event));
        NewEvent->time_prio = 0;
        NewEvent->action = 'T';
        NewEvent->Node = temp;
        insertInQueue(Q,NewEvent);
        
        temp = temp->next;
    }
    printf("Infected ");
    printEvent(Q);
    
    /the loop runs untill queue is not empty/
    int times = 0;
    event* Event = Q->HEAD;
    while(Event != NULL)
    {
        if(Event->action == 'T')
        {
            if(Event->Node->status == 'S')
                TransProcess_SIR(GRAPH,Event,0.5,0.2,&times,S_Head,I_Head,R_Head,Q,300);
        }
        else
            RecovProcess_SIR(Event,&times,S_Head,I_Head,R_Head,Q);
        
        if(Q->HEAD != NULL)
            Event = Q->HEAD;
        else
            break;
    }
    
    printf("\nOutput :\n");
    printf("S :");
    printlist(S_Head->Head);
    printf("I :");
    printlist(I_Head->Head);
    printf("R :");
    printlist(R_Head->Head);
}

void create_graph(graph* Graph,int vertices,int maxEdges)
{
    for(int i=0;i<vertices;++i)
    {
        int edges = rand() % (maxEdges+1);
        for(int j=0;j<edges;++j)
        {
            int Nodenumber = rand() % vertices;
            //edge must be between two different nodes
            while(Nodenumber == i)
                Nodenumber = rand() % vertices;
            
            /checking if the edge already exists/
            int check = 0;
            node* temp = Graph->head[i];
            while(temp != NULL)
            {
                if(temp->nodeNumber == Nodenumber)
                    ++check;
                temp = temp->next;
            }
            
            if(check == 0)
            {
                //adding the new node to head[i]
                node* newnode = (node *)malloc(sizeof(node));
                newnode->nodeNumber = Nodenumber;
                newnode->status = 'S';
                if(Graph->head[i] == NULL)
                {
                    Graph->head[i] = newnode;
                    newnode->next = NULL;
                }
                else
                {
                    node* temp = Graph->head[i];
                    while(temp->next != NULL)
                        temp = temp->next;
                    newnode->next = temp->next;
                    temp->next = newnode;
                }
                
                /because its undirected, it should also have an opposite edge/
                node* oppnode = (node *)malloc(sizeof(node));
                oppnode->nodeNumber = i;
                oppnode->status = 'S';
                if(Graph->head[Nodenumber] == NULL)
                {
                    Graph->head[Nodenumber] = oppnode;
                    oppnode->next = NULL;
                }
                else
                {
                    node* temp = Graph->head[Nodenumber];
                    while(temp->next != NULL)
                        temp = temp->next;
                    oppnode->next = temp->next;
                    temp->next = oppnode;
                }
            }
        }
    }
}


void printEdges(graph* Graph,int vertices)
{
    printf("Edges -\n");
    for(int i=0;i<vertices;++i)
    {
        
        printf("\t%d: ",i);
        node* temp = Graph->head[i];
        while(temp != NULL)
        {
            printf("%d ",temp->nodeNumber);
            temp = temp->next;
        }
        printf("\n");
    }
}


void CreateInit_Infecteds(node* headInfec,int N)
{
    int n = rand() % N, i = 1;
    if(n==0)
        ++n;//atleast one infected node
    
    while(i < n)
    {
        int Node = rand() % n,check = 0;
        
        //checks if the node is already infected
        node* prevInfec = headInfec;
        while(prevInfec != NULL)
        {
            if(prevInfec->nodeNumber == Node)
                ++check;
            prevInfec = prevInfec->next;
        }
        
        if(check == 0)
        {
            if(i == 1)
            {
                headInfec->nodeNumber = Node;
                headInfec->status = 'S';
                headInfec->next = NULL;
            }
            else
            {
                node* temp = (node *)malloc(sizeof(node));
                temp->nodeNumber = Node;
                temp->status = 'S';
                /adding temp in infected node/
                node* tempo = headInfec;
                while(tempo->next != NULL)
                    tempo = tempo->next;
                temp->next=tempo->next;
                tempo->next=temp;
            }
            ++i;
        }
    }
}

void printlist(node* headInfec)
{
    node* temp = headInfec;
    while(temp != NULL)
    {
        printf("%d ",temp->nodeNumber);
        temp = temp->next;
    }
    printf("\n");
}

void insertInQueue(prio_queue* q, event* Event)
{/inserting in priority queue,here time is the priority/
    if(q->HEAD == NULL)
    {
        q->HEAD = Event;
        q->HEAD->NEXT = NULL;
    }
    else
    {
        event* temp = q->HEAD;
        if(q->HEAD->time_prio > Event->time_prio)
        {
            Event->NEXT = q->HEAD;
            q->HEAD = Event;
        }
        else 
        {
            while(temp->NEXT != NULL && temp->NEXT->time_prio <= Event->time_prio)
                temp = temp->NEXT;
            Event->NEXT=temp->NEXT;
            temp->NEXT=Event;
        }
    }
}

void printEvent(prio_queue* q)
{
    printf("Queue : ");
    if(q->HEAD == NULL)
        printf("NULL\n");
    else
    {
        event* Event = q->HEAD;
        while(Event != NULL)
        {
            printf("%d-%c-%d ",Event->time_prio,Event->action,Event->Node->nodeNumber);
            Event = Event->NEXT;
        }
    }
    printf("\n");
}

int expo_variate(float rate)
{
    int x = 0,outcome = rand() % 10;
    //probability is rate 
    while(outcome >= rate*10) 
    {
        ++x;
        outcome = rand() % 10;
    }
    return x;
}

void trans_SIR(prio_queue* Q,int T,float TransmitRate,event Source,int target,int Tmax,nodeHead* S)
{
    node* temp = S->Head;
    while(temp != NULL)
    {
        int check = 0;
        if(temp->nodeNumber == target)
        {/if target node is succeptible it enters the loop/
        
            /checks if target node is already in prio_queue/
            event* Event = Q->HEAD;
            while(Event != NULL)
            {
                if(Event->Node->nodeNumber == target)
                    ++check;
                Event = Event->NEXT;
            }
            
            if(check == 0)
            {
                int y = expo_variate(TransmitRate);
                int Time = *T + y;
                
                /minTime is minimum of maximum time and time required for transmitting node to recover/
                int minTime = Source->time_prio;
                if(Source->time_prio > Tmax)
                    minTime = Tmax;
                
                if(Time <= minTime)
                {/if taget node can be transmitted before souce recovers/
                    event* NewEvent = (event *)malloc(sizeof(event));
                    NewEvent->time_prio = Time;
                    NewEvent->action = 'T';
                    NewEvent->Node = temp;
                    insertInQueue(Q,NewEvent);
                }
            }
        }
        temp = temp->next;//checks for next node
    }
}


void TransProcess_SIR(graph* G,event* EVENT,float recoveryRate,float transmitRate,int* t,nodeHead* S,nodeHead* I,nodeHead* R,prio_queue* Queue,int tmax)
{
    /t is measure of the days/
    *t = EVENT->time_prio ;//event time is added to t
    EVENT->Node->status = 'I';
    
    //node deletion from S list
    node* temp = S->Head;
    node* NODE = (node *)malloc(sizeof(node));
    if(temp->nodeNumber == EVENT->Node->nodeNumber)
    {
        NODE = S->Head;
        S->Head = S->Head->next;
        NODE->next = NULL;
    }
    else
    {
        while(temp->next != NULL)
        {
            if(temp->next->nodeNumber == EVENT->Node->nodeNumber)
                break;
            temp = temp->next;
        }
        NODE = temp->next;
        temp->next = NODE->next;
    }
    
    //node insertion into I list
    NODE->status = 'I';
    if (I->Head == NULL) 
    {
        I->Head = NODE;
        NODE->next=NULL;
    }
    else 
    {
        node *tempo=I->Head;
        while(tempo->next != NULL)
            tempo=tempo->next;
        NODE->next=tempo->next;
        tempo->next=NODE;
    }
    
    /note that recovery time is (time passed) + (time required for recovery)/
    int rec_time = *t + expo_variate(recoveryRate);
    
    if(rec_time < tmax)
    {//if node recovers before max time
        event* NewEvent = (event *)malloc(sizeof(event));
        NewEvent->time_prio = rec_time;
        NewEvent->action = 'R';
        NewEvent->Node = EVENT->Node;
        insertInQueue(Queue,NewEvent);
    }
    
    //event is deleted from the queue
    Queue->HEAD = Queue->HEAD->NEXT;
    
    //transmission for neighbour node happen here
    node* tempo = G->head[EVENT->Node->nodeNumber];
    while(tempo != NULL)
    {
        trans_SIR(Queue,t,transmitRate,EVENT,tempo->nodeNumber,tmax,S);
        tempo = tempo->next;
    }
}


void RecovProcess_SIR(event* EVENT,int* t,nodeHead* S,nodeHead* I,nodeHead* R,prio_queue* Queue)
{
    *t = EVENT->time_prio; 
    EVENT->Node->status = 'R';
    
    //node deletion from I list
    node* temp = I->Head;
    node* NODE = (node *)malloc(sizeof(node));
    if(temp->nodeNumber == EVENT->Node->nodeNumber)
    {
        NODE = I->Head;
        I->Head = I->Head->next;
        NODE->next = NULL;
    }
    else
    {
        while(temp->next != NULL)
        {
            if(temp->next->nodeNumber == EVENT->Node->nodeNumber)
                break;
            temp = temp->next;
        }
        NODE = temp->next;
        temp->next = NODE->next;
    }
    
    //node insertion into R list
    NODE->status = 'R';
    if (R->Head == NULL) 
    {
        R->Head = NODE;
        NODE->next=NULL;
    }
    else 
    {
        node *tempo=R->Head;
        while(tempo->next != NULL)
            tempo=tempo->next;
        NODE->next=tempo->next;
        tempo->next=NODE;
    }
    
    
    Queue->HEAD = Queue->HEAD->NEXT;
}
