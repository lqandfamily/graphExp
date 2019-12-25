#include <string.h>

#define INF 65535 //定义无穷大
#define MaxVerNum 1000//定义最大顶点个数
#define TRUE 1
#define FALSE 0

#include <iostream>

using namespace std;
typedef char elementType;//定义图中顶点数据类型
typedef int eInfoType;//定义eInfo 数据类型 即权值类型
typedef enum {
    UDG, UDN, DG, DN
} GraphKind;//枚举图的类型

typedef struct eNode//定义边链表的结点结构
{
    int adjVer;//邻接顶点信息,此处为编号
    eInfoType eInfo;//边链表中表示边的相关信息,比如边的权值
    struct eNode *next;//指向边链表的下一个顶点
} EdgeNode;//边链表结点类型

typedef struct vNode//定义顶点表的结点结构
{
    elementType data;//存放图中顶点的数据值
    EdgeNode *firstEdge;//插向此顶点关联的第一条边的指针,即边链表的头指针
} VerNode;

typedef struct GraphAdjLinkList//定义图的整体结构
{
    VerNode VerList[MaxVerNum];//顶点表
    int verNum;//顶点数
    int ArcNum;//边数
    GraphKind gKind;//图的类型
} Graph;//图的类型名


//-----------------------将文件读取创建图的邻接表无需看--------------------------

//删除字符串、字符数组左边空格
void strLTrim(char *str) {
    int i, j;
    int n = 0;
    n = strlen(str) + 1;
    for (i = 0; i < n; i++) {
        if (str[i] != ' ')  //找到左起第一个非空格位置
            break;
    }
    //以第一个非空格字符为手字符移动字符串
    for (j = 0; i <= n; j++) {
        str[j] = str[i];
        i++;
    }
}

//从文本创建邻接表表示的图
int CreateGraphFromFile(char fileName[], Graph &G) {
    FILE *pFile;         //定义文件指针
    char str[1000];        //存放读出一行文本的字符串
    char strTemp[10];      //判断是否注释行
    int edgeNum = 0;        //边的数量
    eInfoType eWeight;     //边的信息，常为边的权值
    GraphKind graphType;  //图类型枚举变量
    pFile = fopen(fileName, "r");
    system("cls");
    if (!pFile) {
        printf("错误：文件%s打开失败。\n", fileName);
        return false;
    }
    while (fgets(str, 1000, pFile) != NULL)  //跳过空行和注释行
    {
        strLTrim(str);     //删除字符串左边空格，这是一个自定义函数
        if (str[0] == '\n')    //空行，继续读取下一行
            continue;
        strncpy(strTemp, str, 2);
        if (strstr(strTemp, "//") != NULL)    //跳过注释行
            continue;
        if (strstr(strTemp, "\n") != NULL)
            continue;
        else                          //非注释行、非空行，跳出循环
            break;
    }
    //循环结束，str中应该已经是图的标识Graph，判断标识是否正确
    if (strstr(str, "Graph") == NULL) {
        printf("错误：打开的文件格式错误！\n");
        fclose(pFile);                //关闭文件
        return false;
    }
    //读取图的类型，跳过空行及注释行
    while (fgets(str, 1000, pFile) != NULL) {
        strLTrim(str);     //删除字符串左边空格，这是一个自定义函数
        if (str[0] == '\n')    //空行，继续读取下一行
            continue;
        strncpy(strTemp, str, 2);
        if (strstr(strTemp, "//") != NULL)  //注释行，跳过，继续读取下一行
            continue;
        else                       //非空行，也非注释行，即图的类型标识
            break;
    }
    //设置图的类型
    if (strstr(str, "UDG"))
        graphType = UDG;    //无向图
    else if (strstr(str, "UDN"))
        graphType = UDN;    //无向网
    else if (strstr(str, "DG"))
        graphType = DG;     //有向图
    else if (strstr(str, "DN"))
        graphType = DN;     //有向网
    else {
        printf("错误：读取图的类型标记失败！\n");
        fclose(pFile);       //关闭文件
        return false;
    }
    //读取顶点数据到str。跳过空行
    while (fgets(str, 1000, pFile) != NULL) {
        strLTrim(str);     //删除字符串左边空格，这是一个自定义函数
        if (str[0] == '\n')    //空行，继续读取下一行
            continue;
        strncpy(strTemp, str, 2);
        if (strstr(strTemp, "//") != NULL)  //注释行，跳过，继续读取下一行
            continue;
        else                       //非空行，也非注释行，即图的顶点元素行
            break;
    }
    //顶点数据放入图的顶点数组
    char *token = strtok(str, " ");
    int nNum = 0;
    while (token != NULL) {
        G.VerList[nNum].data = *token;
        G.VerList[nNum].firstEdge = NULL;
        token = strtok(NULL, " ");
        nNum++;
    }
    //循环读取边（顶点对）数据
    int nRow = 0;          //矩阵行下标
    int nCol = 0;           //矩阵列下标
    EdgeNode *eR;       //边链表尾指针
    EdgeNode *p;
    elementType Nf, Ns;   //边或弧的2个相邻顶点
    while (fgets(str, 1000, pFile) != NULL) {
        strLTrim(str);     //删除字符串左边空格，这是一个自定义函数
        if (str[0] == '\n' || str[0] == '\r')    //空行，继续读取下一行
            continue;1
        strncpy(strTemp, str, 2);
        if (strstr(strTemp, "//") != NULL)  //注释行，跳过，继续读取下一行
            continue;
        char *token = strtok(str, " ");     //以空格为分隔符，分割一行数据
        if (token == NULL)            //分割为空串，失败退出
        {
            printf("错误：读取图的边数据失败！\n");
            fclose(pFile);            //关闭文件
            return false;
        }
        Nf = *token;                //获取边的第一个顶点
        token = strtok(NULL, " ");   //读取下一个子串，即第二个顶点
        if (token == NULL)          //分割为空串，失败退出
        {
            printf("错误：读取图的边数据失败！\n");
            fclose(pFile);          //关闭文件
            return false;
        }
        Ns = *token;    //获取边的第二个顶点
        //从第一个顶点获取行号
        for (nRow = 0; nRow < nNum; nRow++) {
            if (G.VerList[nRow].data == Nf)  //从顶点列表找到第一个顶点的编号
                break;
        }
        //从第二个顶点获取列号
        for (nCol = 0; nCol < nNum; nCol++) {
            if (G.VerList[nCol].data == Ns)  //从顶点列表找到第二个顶点的编号
                break;
        }
        //如果为网，读取权值
        if (graphType == UDN || graphType == DN) {            //读取下一个子串，即边的附加信息，常为边的权重
            token = strtok(NULL, " ");
            if (token == NULL)    //分割为空串，失败退出
            {
                printf("错误：读取图的边数据失败！\n");
                fclose(pFile);    //关闭文件
                return false;
            }
            eWeight = atoi(token);  //取得边的附加信息，即权值
        }
        eR = G.VerList[nRow].firstEdge;
        while (eR != NULL && eR->next != NULL) {
            eR = eR->next;        //后移边链表指针，直至尾节点
        }
        p = new EdgeNode;        //申请一个边链表结点
        p->adjVer = nCol + 1;       //顶点的编号，从1开始
        //边的附加信息（权值），对有权图保存权值，无权图为1
        if (graphType == UDN || graphType == DN)
            p->eInfo = eWeight;
        else
            p->eInfo = 1;
        p->next = NULL;
        if (G.VerList[nRow].firstEdge == NULL) {
            G.VerList[nRow].firstEdge = p;
            eR = p;
        } else {
            eR->next = p;
            eR = p;       //新的尾指针
        }
        edgeNum++;     //边数加1
    }
    G.verNum = nNum;    //图的顶点数
    if (graphType == UDG || graphType == UDN)
        G.ArcNum = edgeNum / 2;   //无向图或网的边数等于统计的数字除2
    else
        G.ArcNum = edgeNum;
    G.gKind = graphType;           //图的类型
    fclose(pFile);                 //关闭文件
    for (int i = G.verNum; i >= 1; i--) {
        G.VerList[i] = G.VerList[i - 1];
    }
    return true;
}

int firstAdj(Graph *G, int v)//寻找v的第一个相邻结点 若无则返回0
{
    int w = 0;
    if (G->VerList[v].firstEdge)//第一个邻接点存在
    {
        w = G->VerList[v].firstEdge->adjVer;
    }
    return w;
}

int nextAdj(Graph *G, int v, int w)//寻找G中V的位于w之后的下一个邻接顶点的下标 若不存在则返回0
{
    int k = 0;//记录顶点下标
    EdgeNode *p;//进行遍历边所用指针
    p = G->VerList[v].firstEdge;
    while (p && p->adjVer != w) {
        p = p->next;
    }
    p = p->next;
    if (p) {
        k = p->adjVer;
    }
    return k;
}

void visit(Graph *G, int v)//访问该结点
{
    printf("%c ", G->VerList[v].data);
}

void dfs(Graph *G, int v, int visited[])//数组用来标记已访问结点 depth first search 深度优先搜索
{
    int w;//记录相邻结点下标
    visit(G, v);//访问该结点
    visited[v] = 1;//标记为已读
    w = firstAdj(G, v);//找v的第一个相邻结点
    while (w != 0)//v存在相邻结点
    {
        if (visited[w] == 0)//w当前还未访问过
        {
            dfs(G, w, visited);
        }
        w = nextAdj(G, v, w);//寻找v的位于w后面的下一个相邻结点
    }
}

int DFS(Graph *G, int visited[])//通用的dfs算法,包括了森林
{
    int cnt = 0;//顺便求图的连通分量个数
    for (int i = 1; i < G->verNum + 1; i++) {
        if (visited[i] == 0) {
            dfs(G, i, visited);
            cnt++;
        }
    }
    return cnt;
}

void DfsCountEdge(Graph *G, int v, int visited[], int *edge) {
    int w;
    visited[v] = 1;
    w = firstAdj(G, v);
    while (w != 0) {
        (*edge)++;
        if (visited[w] == 0) {
            DfsCountEdge(G, w, visited, edge);
        }
        w = nextAdj(G, v, w);
    }
}

void DfsCountEdge1(Graph *G, int visited[], int *edge) {
    int i = 1;
    while (i <= G->verNum) {
        if (visited[i] == 0)
            DfsCountEdge(G, i, visited, edge);
        i++;
    }
    if (G->gKind == UDN || G->gKind == UDG)
        (*edge) /= 2;
}
//额外题 判断无向树是否是一颗树
//两方面 1.判断它是不是不连通(利用dfs算法) 2.判断他的边数是不是n - 1(同样利用dfs算法)

//对给定的图G及出发点v0，设计算法从V0出发深度优先遍历图G，并构造出相应的生成树或生成森林
//这道题其实是想让你再写一遍dfs然后把路径打印出来
void dfs1(Graph *G, int v, int visited[]) {
    int w;
    visited[v] = 1;
    w = firstAdj(G, v);
    while (w != 0) {
        if (visited[w] == 0) {
            printf("%c -> %c \n", G->VerList[v].data, G->VerList[w].data);
            dfs1(G, w, visited);
        }
        w = nextAdj(G, v, w);
    }
}

void DFS1(Graph *G, int v, int visited[]) {
    int i = 1;
    dfs1(G, v, visited);
    printf("\n");
    while (i <= G->verNum) {
        if (visited[i] == 0) {
            dfs1(G, i, visited);
            printf("\n");
        }
        i++;
    }
}

//广度优先搜索算法 BFS
//设置队列的基本操作
#define MAXLEN 100
typedef struct q {
    int data[MAXLEN];
    int front;
    int rear;
} Queue;

void initialQueue(Queue *Q) {
    Q->front = Q->rear = 0;
}

int queueEmpty(Queue *Q) {
    return Q->front == Q->rear;
}

void enQueue(Queue *Q, int x) {
    if ((Q->rear + 1) % MAXLEN == Q->front) {
        printf("Error,Queue is Full!\n");
    } else {
        Q->rear = (Q->rear + 1) % MAXLEN;
        Q->data[Q->rear] = x;
    }
}

int outQueue(Queue *Q)//出队同时返回队头元素
{
    Q->front = (Q->front + 1) % MAXLEN;
    return Q->data[Q->front];
}

void bfs(Graph *G, int v, int visited[], Queue *Q) {
    int w, j;
    enQueue(Q, v);
    visited[v] = 1;
    while (!queueEmpty(Q)) {
        w = outQueue(Q);
        j = firstAdj(G, w);
        while (j != 0) {
            if (visited[j] == 0) {
                printf("%c -> %c \n", G->VerList[w].data, G->VerList[j].data);
                visited[j] = 1;
                enQueue(Q, j);
            }
            j = nextAdj(G, w, j);
        }
    }
}

void BFS(Graph *G, int v, int visited[]) {
    int i = 1;
    Queue Q;
    initialQueue(&Q);
    bfs(G, v, visited, &Q);
    printf("\n");
    while (i <= G->verNum) {
        if (visited[i] == 0) {
            bfs(G, i, visited, &Q);
            printf("\n");
        }
        i++;
    }
}

//额外题  不用看
//改造dfs算法判断I和j之间是否有路径
void hasPath(Graph *G, int i, int j, int visited[]) {
    int w;
    visit(G, i);
    visited[i] = 1;
    if (i == j) {
        printf("\n路径存在!\n");
        return;
    }
    w = firstAdj(G, i);
    while (w != 0) {
        if (visited[w] == 0) {
            hasPath(G, w, j, visited);
        }
        w = nextAdj(G, i, w);
    }
}

//额外题
//改造bfs算法判断i和j之间是否有路径
void hasPath1(Graph *G, int i, int j, int visited[]) {
    int w, k;
    Queue Q;
    initialQueue(&Q);
    visit(G, i);
    visited[i] = 1;
    enQueue(&Q, i);
    while (queueEmpty(&Q) == 0) {
        k = outQueue(&Q);
        w = firstAdj(G, k);
        while (w != 0) {
            if (visited[w] == 0) {
                enQueue(&Q, w);
                visit(G, w);
                visited[w] = 1;
                if (w == j) {
                    printf("\n路径存在!\n");
                    return;
                }
            }
            w = nextAdj(G, k, w);
        }
    }
}

//实现Prim算法，求解下列给定图G的最小生成树
typedef struct minEdgeType {
    int v;
    int eWeight;
} MinEdgeType;

int HasEdge(Graph *G, int vBegin, int vEnd, eInfoType *eWeight) {
    EdgeNode *p;
    int f = 0;
    *eWeight = INF;
    p = G->VerList[vBegin].firstEdge;
    while (p) {
        if (p->adjVer == vEnd) {
            f = 1;
            *eWeight = p->eInfo;
            break;
        }
        p = p->next;

    }
    return f;
}

void initialTE(Graph &G, MinEdgeType TE[], int vID) {
    int i;
    eInfoType eWeight;
    for (i = 1; i <= G.verNum; i++) {
        if (HasEdge(&G, vID, i, &eWeight)) {
            TE[i].v = vID;
            TE[i].eWeight = eWeight;
        } else
            TE[i].eWeight = INF;
    }
}

int GetMinEdge(Graph &G, MinEdgeType TE[], int visited[]) {
    eInfoType eMin = INF;
    int i, j = 0;
    for (i = 1; i <= G.verNum; i++) {
        if (visited[i] == 0 && TE[i].eWeight < eMin) {
            j = i;
            eMin = TE[i].eWeight;
        }
    }
    return j;
}

void UpdateTE(Graph &G, MinEdgeType TE[], int vID, int visited[]) {
    int i;
    eInfoType eWeight;
    for (i = 1; i <= G.verNum; i++) {
        if (visited[i] == 0) {
            if (HasEdge(&G, vID, i, &eWeight) && eWeight < TE[i].eWeight) {
                TE[i].v = vID;
                TE[i].eWeight = eWeight;
            }
        }
    }
}

void prim(Graph *G, int vID, int visited[]) {
    MinEdgeType TE[MaxVerNum];
    int i, j;
    int curID;
    initialTE(*G, TE, vID);
    visited[vID] = 1;
    for (i = 1; i < G->verNum; i++) {
        curID = GetMinEdge(*G, TE, visited);
        j = TE[curID].v;
        printf("%c -> %c\n", G->VerList[j].data, G->VerList[curID].data);
        visited[curID] = 1;
        UpdateTE(*G, TE, curID, visited);
    }
}

//kruskal算法
typedef struct edgetype {
    int vBegin;
    int vEnd;
    eInfoType eWeight;
} EdgeType;
EdgeType edges[100];

void GetEdges(Graph &G, EdgeType edges[]) {
    int i;
    int k = 1;               //作为边数组的下标
    EdgeNode *p;    //边链表的头指针
    for (i = 1; i <= G.verNum; i++) {                          //读取当前顶点的边链表的头指针
        p = G.VerList[i].firstEdge;
        while (p) {                       //由顶点数据获取顶点编号
            edges[k].vBegin = i;
            edges[k].vEnd = p->adjVer;  //读取另一顶点编号到vEnd
            edges[k].eWeight = p->eInfo;
            p = p->next;
            k++;
        }
    }
}

EdgeType GetMinEdge(Graph &G, EdgeType edges[], int edgeUsed[], int &n) {          //函数值返回读取的最小边
    //n为返回的最小边在edges[ ]数组中的下标
    EdgeType minEdge;
    int wMin = INF;  //保存最小权值
    int i;
    int M;  //控制循环次数
    if (G.gKind == UDG || G.gKind == UDN)
        M = G.ArcNum * 2;  //无向网（图），因为对称性，
        //邻接矩阵或邻接表中有效数据是边数的2倍
    else
        M = G.ArcNum;    //有向图，有向网中，M即为图的边数
    for (i = 1; i <= M; i++) {
        if (!edgeUsed[i] && edges[i].eWeight < wMin) {                //对未使用，且权值较小的边，
            //暂定为最小边，更新相关数组
            wMin = edges[i].eWeight;
            minEdge.eWeight = edges[i].eWeight;
            minEdge.vBegin = edges[i].vBegin;
            minEdge.vEnd = edges[i].vEnd;
            n = i;
        }
    }
    return minEdge;  //返回取得的最小边
}

void Kruskal(Graph &G) {                //顶点的连通分量编号数组
    int conVerID[100];
    //边数组--存放图的所有边信息
    EdgeType edges[100];
    //生成树数组--存放生成树的n-1条边
    EdgeType treeEdges[100];
    int edgeUsed[100];
    //辅助数组，标记edges[]中的边是否已经用过。
    //1--已用过，0--未用过
    //也可用排序算法对edges[]排序来完成这个工作

    EdgeType minEdge;  //保存最小边
    int i, j;
    int conID;  //边的终止顶点vEnd的连通分量的编号
    int n;      //返回的最小边在edges[]数组中的下标
    GetEdges(G, edges); //获取图所有边的信息，
    //存入数组edges[]
    //初始化可用边标记数组edgeUsed [ ]
    //--可用排序算法取代
    int M;      //循环次数
    if (G.gKind == UDG || G.gKind == UDN)
        M = G.ArcNum * 2;   //无向图（网）乘2
    else
        M = G.ArcNum;
    for (i = 1; i <= M; i++)
        edgeUsed[i] = FALSE;  //标记所有边都可用
    for (i = 1; i <= G.verNum; i++)  //初始化连通分量编号。
    {
        conVerID[i] = i;    //连通分量编号=顶点编号。
    }
    for (i = 1; i < G.verNum; i++)    //取出n-1条边，构成生成树
    {            //获取一条最小边
        minEdge = GetMinEdge(G, edges, edgeUsed, n);
        while (conVerID[minEdge.vBegin] == conVerID[minEdge.vEnd]) {                                    //如果minEdge会形成回路
            edgeUsed[n] = 1;    //标记此最小边不可用
            //继续取下一条最小边
            minEdge = GetMinEdge(G, edges, edgeUsed, n);
        }
        //到此取得了一条可用最小边，加入生成树中
        treeEdges[i] = minEdge;    //可用最小边加入生成树的边数组
        printf("%c -> %c\n", G.VerList[treeEdges[i].vBegin].data, G.VerList[treeEdges[i].vEnd].data);
        conID = conVerID[minEdge.vEnd];   //取得最小边的终点编号
        //conID=conVerID[minEdge.vBegin];
        //合并连通编号到最小编号
        for (j = 1; j <= G.verNum; j++) {     //所有连通分量编号为conID的顶点，
            //连通分量编号都改为最小边起始顶点的连通号
            if (conVerID[j] == conID) {
                conVerID[j] = conVerID[minEdge.vBegin];
            }
        }
        edgeUsed[n] = TRUE;   //标记当前选择的边已经用过。
    }
}

//DJ算法
void Dijkstra(Graph &G, int path[], int dist[], int vID) {
    int solved[MaxVerNum];
    int i, j, v;
    eInfoType minDist;  //保存最短距离值，
    //eInfoType为自定义的边的权值类型
    EdgeNode *p;      //指向边链表结点的指针，
    //EdgeNode为边链表结点结构类型
    //初始化数组solved[]、dist[]和path[]
    for (i = 1; i <= G.verNum; i++) {
        solved[i] = 0;  //所有顶点均未处理
        dist[i] = INF;   //所有顶点初始距离置为无穷大
        path[i] = -1;    //所有顶点的前驱置为-1，即无前驱
    }
    //处理起始顶点vID
    solved[vID] = 1;   //标记vID已经处理
    dist[vID] = 0;        //vID到自身距离为0
    path[vID] = -1;      //vID为起始顶点，没有前驱
    //从邻接表初始化dist[]和path[]
    //顶点vID的边链表头指针p
    p = G.VerList[vID].firstEdge;
    while (p) {
        v = p->adjVer;       //取得顶点vID的邻接顶点编号
        //取得vID与v之间边的权值，赋给dist[v]
        dist[v] = p->eInfo;
        path[v] = vID;       //顶点v的前驱为vID
        p = p->next;
    }
    //依次找出余下n-1个顶点加入已求解集合S中
    for (i = 1; i < G.verNum; i++) {
        minDist = INF;
        //在未解顶点中寻找距vID距离最近的顶点，
        //编号保存到v。
        for (j = 1; j <= G.verNum; j++) {
            if (solved[j] == 0 && dist[j] < minDist) {
                v = j;    //j为未解顶点集V-S中距离vID最近的顶点
                minDist = dist[j];
            }
        }                //已解顶点集S与未解顶点集V-S
        //没有相邻的顶点，算法退出
        if (minDist == INF)
            return;

        //输出本次选择的顶点距离
        cout << "选择顶点：" << G.VerList[v].data << "--距离：" << minDist << endl;
        solved[v] = 1;  //标记顶点v已解
        //对选中的已解顶点v，
        //更新v的邻接点（直接后继）的路径和距离
        p = G.VerList[v].firstEdge;  //取得v的边链表的头指针
        while (p) {
            j = p->adjVer;  //取得v的邻接顶点编号，保存到j
            if (solved[j] == 0 && minDist + p->eInfo < dist[j]) {              //更新顶点j到顶点vID的最小距离
                dist[j] = minDist + p->eInfo;
                path[j] = v;    //j的前驱改为顶点v
            }
            p = p->next;
        }
    }
}

void Floyd(Graph &G1, int dist[30][30], int path[30][30]) {
    //把G1 用邻接表表示的图转化成用矩阵表示的图
    int i = 1;
    int j = 1;
    EdgeNode *p;
    typedef struct g {
        int AdjMatrix[30][30];
        int VerNum;
    } Graph1;
    Graph1 G;
    G.VerNum = G1.verNum;
    for (; i <= G.VerNum; i++) {
        for (j = 1; j <= G.VerNum; j++) {
            if (i == j)
                G.AdjMatrix[i][i] = 0;
            else
                G.AdjMatrix[i][j] = INF;
        }
    }
    for (i = 1; i <= G.VerNum; i++) {
        p = G1.VerList[i].firstEdge;
        while (p) {
            j = p->adjVer;//eInfo
            G.AdjMatrix[i][j] = p->eInfo;
            p = p->next;
        }
    }
    int m;
    for (i = 1; i <= G.VerNum; i++)  //初始化距离矩阵和路径矩阵
    {
        for (j = 1; j <= G.VerNum; j++) {            //距离矩阵初始化为邻接矩阵
            dist[i][j] = G.AdjMatrix[i][j];
            //初始化路径矩阵，路径矩阵元素path[i][j]中
            //保存编号j顶点的前驱的顶点编号
            //如果i,j之间存在边(弧)，则j的前驱为i。否则前驱置为-1
            if (i != j && G.AdjMatrix[i][j] < INF)
                path[i][j] = i;
            else
                path[i][j] = -1;
        }
    }
    //下面是Floyd算法的核心 -- 三重for循环
    for (m = 1; m <= G.VerNum; m++)  //注意外层循环必须为选择跳点m的循环
    {
        for (i = 1; i <= G.VerNum; i++) {
            for (j = 1; j <= G.VerNum; j++) {        //m作为跳点，i、j之间距离变小，接收m作为中转点
                if (i != j && dist[i][m] + dist[m][j] < dist[i][j]) {                    //更新最短距离
                    dist[i][j] = dist[i][m] + dist[m][j];
                    //更新路径，以m->j路径j的前驱更新
                    //原来i->j路径j的前驱
                    path[i][j] = path[m][j];
                }
            }
        }
    }
    for (i = 1; i <= G.VerNum; i++) {
        for (j = 1; j <= G.VerNum; j++) {
            if (i != j && dist[i][j] < INF) {
                printf("%c -> %c 的最短距离为:%d \n路径为:%c ", G1.VerList[j].data, G1.VerList[i].data, dist[i][j],
                       G1.VerList[j].data);
                m = path[i][j];
                while (m != i) {
                    printf("%c ", G1.VerList[m].data);
                    m = path[i][m];
                }
                printf("%c\n", G1.VerList[i].data);
            }
        }
    }
}
//拓扑排序

//首先初始化一个栈
typedef struct s {
    int data[MAXLEN];
    int top;
} seqStack;

void initStack(seqStack &S) {
    S.top = 0;
}

void pushStack(seqStack &S, int i) {
    S.top++;
    S.data[S.top] = i;
}

void popStack(seqStack &S, int &v) {
    v = S.data[S.top];
    S.top--;
}

void GetInDegrees(Graph &G, int inds[])//保存各节点入度
{
    int i, w;
    for (i = 1; i <= G.verNum; i++) {
        inds[i] = 0;
    }
    for (i = 1; i <= G.verNum; i++) {
        w = firstAdj(&G, i);
        while (w) {
            inds[w]++;
            w = nextAdj(&G, i, w);
        }
    }
}

int stackEmpty(seqStack G) {
    return (G.top == 0);
}

int TopologicalSortS(Graph &G, int topoList[]) {
    int inds[G.verNum + 1];
    seqStack S;
    int i;
    int v;
    int w;
    int vCount = 0;
    initStack(S);
    GetInDegrees(G, inds);
    for (i = 1; i <= G.verNum; i++) {
        if (inds[i] == 0) {
            pushStack(S, i);
        }
    }
    printf("拓扑排序序列为:");
    while (!stackEmpty(S)) {
        popStack(S, v);
        printf("%c ", G.VerList[v].data);
        vCount++;
        topoList[vCount] = v;
        w = firstAdj(&G, v);
        while (w) {
            inds[w]--;
            if (inds[w] == 0) {
                pushStack(S, w);
            }
            w = nextAdj(&G, v, w);
        }
    }
    printf("\n");
    if (vCount == G.verNum)
        return 1;
    else
        return 0;
}
//求解关键路径

int ADJij(Graph G, int i, int j, int *einfo)//判断ij是否相邻
{//einfo 用来返回权值
    EdgeNode *p;
    p = G.VerList[i].firstEdge;
    while (p) {
        if (p->adjVer == j) {
            (*einfo) = p->eInfo;
            return 1;
        }
        p = p->next;
    }
    return 0;
}

void KeyPath(Graph &G) {
    int einfo;//用来记录权值
    int i, j, w, v;
    int vet[G.verNum + 1];//最早发生时间
    int vlt[G.verNum + 1];//最晚发生时间
    int vPre;//前驱顶点编号
    int vSuc;//后驱顶点编号
    int topoList[50];//存放拓扑序列编号

    //初始化AOE网中各个顶点最早发生时间为0
    TopologicalSortS(G, topoList);
    system("cls");
    for (i = 1; i <= G.verNum; i++) {
        vet[i] = 0;
    }
    //按拓扑次序求各个顶点(事件)的最早发生时间
    //存放在vet[]数组中
    for (i = 1; i <= G.verNum; i++) {
        //取出拓扑序列中的第一个顶点编号vPre
        //更新vPre所有邻接顶点的最早发生时间
        vPre = topoList[i];
        for (j = 1; j <= G.verNum; j++) {
            //vPre与j相邻
            if (ADJij(G, vPre, j, &einfo)) {
                if (vet[j] < vet[vPre] + einfo) {
                    vet[j] = vet[vPre] + einfo;
                    //更新顶点j的最早发生时间
                }
            }
        }
    }
    //初始化每个顶点的最迟发生时间
    for (i = 1; i <= G.verNum; i++) {
        vlt[i] = vet[G.verNum];
    }
    //按逆拓扑次序求每个顶点的最迟发生时间
    //顶点vSuc的最迟发生时间已经确定
    //更新vSuc每个前驱的最迟发生时间
    for (i = G.verNum; i >= 1; i--) {
        vSuc = topoList[i];
        for (j = G.verNum; j >= 1; j--) {//j与vSuc相邻
            if (ADJij(G, j, vSuc, &einfo)) {
                if (vlt[j] > vlt[vSuc] - einfo) {
                    vlt[j] = vlt[vSuc] - einfo;
                    //更新j的最迟发生时间
                }
            }
        }
    }
    //寻找关键路径 并只输出一条
    printf("关键路径为");
    v = topoList[1];
    printf("%c ", G.VerList[v].data);//输出源点
    while (v) {
        w = firstAdj(&G, v);
        while (w) {
            if (vet[w] == vlt[w]) {
                printf("%c ", G.VerList[w].data);
                break;
            } else
                w = nextAdj(&G, v, w);//搜索下一个邻接点

        }
        //继续寻找w的下一个关键节点
        v = w;
    }
}
