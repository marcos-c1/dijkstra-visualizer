#ifndef DJIKSTRA_H
#define DJIKSTRA_H
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "settings.h"
#include <limits.h>
#include <math.h>
#include <string.h>

#define MAX_NODES 26

typedef struct Node {
  int id;
  int xCoord;
  int yCoord;
} Node;

typedef struct Dijkstra {
  int adjList[MAX_NODES][MAX_NODES];
  int nodeCount;
  int lineNodesCount;
  Node *selectedNodes;
  Node *nodes;
} Dijkstra;
#endif

extern Node sourceNode, destinationNode;
extern Dijkstra *djk;

void garbageCollector(Dijkstra *djk);
Dijkstra* createDijkstra();
Node* createNode(int id, int xCoord, int yCoord);
int computeDistance(Node a, Node b);
int noRepeat(Dijkstra *djk, int x, int y); 
void initAdjList(Dijkstra *djk);
void writeLabel(char text, int x, int y);
void drawCircle(float cx, float cy, float r, float num_segments, enum Colors colorCharacter);
void getPerpendicularCoordinates(int *x, int *y);
void writeText(char *text, int x, int y, enum Colors color);
void getMidPoint(int *midX, int *midY);
void drawLine(enum Colors color); 
void showInstruction();
void displayHandler();
void reshapeHandler(GLsizei w, GLsizei h);
void init();
void showInfo();
void mouseHandler(int btn, int state, int x, int y); 
int minDistance(int dist[], int visited[]);
void writeDistance(int distance);
void dijkstra();
void initDijkstra(unsigned char key, int x, int y);
