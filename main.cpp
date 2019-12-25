#include <stdio.h>
#include <stdlib.h>
#include "Grahp.h"

//有关代码的问题可以QQ:396434855 进行探讨
int main() {
    int ichoice, i;
    char fileName[100];
    Graph graph;
    graph.verNum = 0;
    do {
        printf("***********************图实验***************************\n");
        printf("*1-文件创建邻接表表示的图                              *\n");
        printf("*2-Depth First Search                                  *\n");
        printf("*3-计算图的边数                                        *\n");
        printf("*4-深度优先搜索(DFS) + 构造生成树                      *\n");
        printf("*5-广度优先搜索(BFS) + 构造生成树                      *\n");
        printf("*6-Prim算法求最小生成树                                *\n");
        printf("*7-Kruskal算法求最小生成树                             *\n");
        printf("*8-Dijkstra算法求指定顶点到其余顶点之间的最短路径      *\n");
        printf("*9-Floyd算法求图G各顶点之间的最短路径                  *\n");
        printf("*10-拓扑排序                                           *\n");
        printf("*11-计算AOE网的关键路径                                *\n");
        printf("********************************************************\n");
        scanf("%d", &ichoice);
        getchar();
        system("cls");
        switch (ichoice) {
            case 1:
                printf("请输入文件名:");
                scanf("%s", fileName);
                if (CreateGraphFromFile(fileName, graph)) {
                    printf("文件创建完成!\n");
                }
                break;
            case 2: {
                int visited[MaxVerNum] = {0};//记录已访问过元素的数组 0单元不用
                printf("DFS(深度优先搜索)遍历为:");
                i = DFS(&graph, visited);
                printf("\n图的连通分量个数为:%d\n", i);
            }
                break;
            case 3: {
                int visited[MaxVerNum] = {0};
                int edge = 0;
                DfsCountEdge1(&graph, visited, &edge);
                printf("边数为%d\n", edge);
                break;
            }
                break;
            case 4: {
                printf("请输入起始遍历点:");
                scanf("%d", &i);
                getchar();
                int visited[MaxVerNum] = {0};//记录已访问过元素的数组 0单元不用
                printf("DFS(深度优先搜索)遍历及其生成树为:\n");
                DFS1(&graph, i, visited);
            }
                break;
            case 5: {
                printf("请输入起始遍历点:");
                scanf("%d", &i);
                getchar();
                int visited[MaxVerNum] = {0};//记录已访问过元素的数组 0单元不用
                printf("BFS(广度优先搜索)遍历及其生成树为:\n");
                BFS(&graph, i, visited);
            }
                break;
            case 6: {
                printf("请输入起始遍历点:");
                scanf("%d", &i);
                system("cls");
                getchar();
                printf("生成树为:\n");
                int visited[MaxVerNum] = {0};//记录已访问过元素的数组 0单元不用
                prim(&graph, i, visited);//O(n^2)
            }
                break;
            case 7:
                printf("Kruskal算法求最小生成树为:\n");
                Kruskal(graph);
                break;
            case 8: {
                int path[100], dist[100];
                printf("请输入起始点序号:");
                scanf("%d", &i);
                Dijkstra(graph, path, dist, i);
                printf("\n");
            }
                break;
            case 9: {
                int path[30][30];
                int dist[30][30];
                Floyd(graph, dist, path);
            }
                break;
            case 10: {
                int path[30];
                if (TopologicalSortS(graph, path))//path只是为了凑数
                {
                    printf("遍历完所有顶点是拓扑图!\n");
                } else {
                    printf("无法遍历所有结点,不是拓扑图!\n");
                }
            }
                break;
            case 11:
                KeyPath(graph);
                printf("\n");
                break;
        }
    } while (ichoice);
    return 0;
}
