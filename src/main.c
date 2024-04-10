#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define MAX_NODES 26
#define HEIGHT 1020
#define WIDTH 1080
#define PI 3.14285714286
#define TRUE 1
#define FALSE 0
#define RADIUS 30

int running = FALSE;
int instruction = TRUE;
int source = FALSE;
int destination = FALSE;

typedef struct Node {
  int id;
  int xCoord;
  int yCoord;
} Node;

Node sourceNode, destinationNode;

typedef struct Dijkstra {
  int adjList[MAX_NODES][MAX_NODES];
  int nodeCount;
  int lineNodesCount;
  Node *selectedNodes;
  Node *nodes;
} Dijkstra;

Dijkstra *djk; 

void garbageCollector(Dijkstra *djk){
  free(djk->nodes);
  free(djk->selectedNodes);
}

Dijkstra* createDijkstra(){
  Dijkstra *djk = malloc(sizeof(Dijkstra));
  djk->nodeCount = FALSE;
  djk->lineNodesCount = FALSE;
  djk->selectedNodes = malloc(sizeof(Node) * 2);
  djk->nodes = malloc(sizeof(Node) * MAX_NODES);
  return djk;
}

Node* createNode(int id, int xCoord, int yCoord){
  Node *n = malloc(sizeof(Node));
  n->id = id; 
  n->xCoord = xCoord;
  n->yCoord = yCoord;
  return n; 
}

void delay(int number_of_seconds)
{
    int milli_seconds = 1000 * number_of_seconds;
    clock_t start_time = clock();

    // looping till required time is not achieved 
    while (clock() < start_time + milli_seconds);
}

int computeDistance(Node a, Node b){
  return (int) sqrt(pow(abs(a.xCoord - b.xCoord), 2) + pow(abs(a.yCoord - b.yCoord), 2));
}

int noRepeat(Dijkstra *djk, int x, int y) 
{	
	// if no nodes on screen, dont bother
	if(!djk->nodeCount)
		return 1;

	// create a tmp node for comparison on the clicked coordinate
	Node tmp = {
    djk->nodeCount, 
    x, 
    y
  };

	for(int i = 0; i < djk->nodeCount; i++)
	{	
		// if the nodes intersect, assume a node is clicked
		if(computeDistance(tmp, djk->nodes[i]) < 2 * RADIUS)
		{	 
			// push the clicked node to the stack
			djk->selectedNodes[djk->lineNodesCount] = djk->nodes[i];
			return 0;
		}
	}
	// no node was pressed, create a new node at x,y
	return 1;
}

void initAdjList(Dijkstra *djk){
  for(int i = 0; i < MAX_NODES; i++){
    for(int j = 0; j < MAX_NODES; j++){
      djk->adjList[i][j] = INT_MAX; 
    }
  }
}

// opengl: labeling nodes
void writeLabel(char text, int x, int y)
{	
	// change the color to yellow
	glColor3f(1.0, 1.0, 0.0);
    glRasterPos2i(x,y);
  glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24 , text);
}


//draw Circle at the given coordinates
void drawCircle(float cx, float cy, float r, float num_segments, char colorCharacter)
{
	// theta: angle in radians of the point 
	// x: x coordinate of point
	// y: y coordinate of point
	float theta,x,y;

	// Label the Node to [A-Z]
	// Change the color to green if colorCharacter = 'N'
	if(colorCharacter == 'N'){
		writeLabel((char)65 + djk->nodeCount, cx - 5, cy - 14);
		glColor3f(0.0, 1.0, 0.0);
  }

	if(colorCharacter == 'B')
		glColor3f(0.0, 0.0, 1.0);

	if(colorCharacter == 'R')
		glColor3f(1.0, 0.0, 0.0);

	// line loop: connect all the points below 
	glBegin(GL_LINE_LOOP);
 
	for(int i = 0; i < num_segments; i++){
		// compute new angle and get the x, y coordinate
		theta = 2.0f * PI * i / num_segments;
 		x = r * cosf(theta);
		y = r * sinf(theta);
 		
 		// draw the point
		glVertex2f(x + cx, y + cy);
	}
	glEnd();

	//flush to output screen
	glFlush();

}

// get perpendicular coordinates to display the text
void getPerpendicularCoordinates(int *x, int *y)
{
	int x1, x2, y1, y2;

	x1 = djk->selectedNodes[0].xCoord;
	x2 = djk->selectedNodes[1].xCoord;
	y1 = djk->selectedNodes[0].yCoord;
	y2 = djk->selectedNodes[1].yCoord;

	float diff = (x2 - x1)? 0.0001 : x2 - x1;

	float slope = ((float)(y2 - y1)) / (diff);

	float perpendicularSlope = -1*(1 / slope);


	*x = *x + 25; 	
	*y = *y + perpendicularSlope * (*x) + 7;
}

// opengl: writing the distances between two nodes
void writeDistance(char *text, int x, int y)
{	
	if(instruction)
		glColor3f(1.0, 1.0, 1.0);
	else
		glColor3f(1.0, 1.0, 0.0);

  glRasterPos2i(x,y);
  size_t len = strlen(text);
  for(int i = 0; i < len; i++)
  { 
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
  }
  glFlush();
}

//sets the mid point coordinates to the arguments
void getMidPoint(int *midX, int *midY)
{
	*midX = (djk->selectedNodes[0].xCoord + djk->selectedNodes[1].xCoord)/2;
	*midY = (djk->selectedNodes[0].yCoord + djk->selectedNodes[1].yCoord)/2;
}

void drawLine(char color) 
{
	//set color to white
	if(color == 'N' || color == 'R')
		glColor3f(0.3, 0.3, 0.3);

	if(color == 'D')
		glColor3f(0.8, 0.8, 0.8);

	if(color == 'M')
		glColor3f(0.5, 0.0, 0.0);

	// draw lines between selected nodes (stored in lineNodes)
	glBegin(GL_LINES);
		glVertex2f(djk->selectedNodes[0].xCoord, djk->selectedNodes[0].yCoord);
		glVertex2f(djk->selectedNodes[1].xCoord, djk->selectedNodes[1].yCoord);
	glEnd();
	glFlush();

	// mid Point of the line segment to display distance
	int midX, midY;
	char distanceString[10]; 
	// get the mid coordinates 
	getMidPoint(&midX, &midY);  

	int id1 = djk->selectedNodes[0].id;
	int id2 = djk->selectedNodes[1].id;
	//compute distance between the nodes.
	int distance = computeDistance(djk->selectedNodes[0], djk->selectedNodes[1]);
	//store it in adjMatrix
	djk->adjList[id1][id2] = distance;
	djk->adjList[id2][id1] = distance;

	// convert distance to string
	sprintf(distanceString, "%d",distance);

	//Display to the window
	getPerpendicularCoordinates(&midX, &midY);
	writeDistance(distanceString, midX, midY);

	glFlush();

	//write both circles' labels again
	writeLabel('A' + djk->selectedNodes[0].id, djk->selectedNodes[0].xCoord - 5, djk->selectedNodes[0].yCoord - 14);
	writeLabel('A' + djk->selectedNodes[1].id, djk->selectedNodes[1].xCoord - 5, djk->selectedNodes[1].yCoord - 14);

	glFlush();

	//Display the updated matrix to the console
  /*
	if(color == 'N')
	{
		printf("\n");
		for(int i = 0; i < djk->nodeCount; i++)
		{
			for(int j = 0; j < djk->nodeCount; j++)
			{
				printf("%d \t", djk->adjList[i][j]);
			}
			printf("\n");
		}
	}
  */
}

// display instructions
void showInstruction()
{
	writeDistance("Welcome! To The Simulation of Dijkstra's Algorithn.", 200, 900);
	writeDistance("- Use Mouse Left Button to Create Nodes.", 250, 800);
	writeDistance("- Select any two Nodes to form a Path.", 250, 770);
	writeDistance("- Use Mouse Right Button to select Source and Destrination.", 250, 740);
	writeDistance("- Press ENTER key to begin Dijkstra's Algorithm.", 250, 710);
}

// display handler event to show the instructions
void displayHandler(){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  showInstruction();
  glFlush();
}

// handler event in window reshape size 
void reshapeHandler(GLsizei w, GLsizei h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

// init settings for OPENGL window
void init()
{
  glViewport(0, 0, WIDTH, HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, WIDTH, 0, HEIGHT);
  glMatrixMode(GL_MODELVIEW);
  glClearColor(0.0, 0.0, 0.0, 1);
  glColor3f(1.0, 1.0, 1.0);
  glLineWidth(5);
}

// mouse handler event 
void mouseHandler(int btn, int state, int x, int y) 
{

	if(instruction)
	{
		instruction = 0;
		glClear(GL_COLOR_BUFFER_BIT);
	}

	if(running) 
	{
		return;
	}

	//fix coordinate system
	y = HEIGHT - y;

	//left click: create nodes, join nodes
	if(btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{	
		// if not clicked on the circle 
		if (noRepeat(djk, x, y))
		{
			// dont draw if nodeCount > 26;
			if(djk->nodeCount >= 26)
			{
				return;
			}

			drawCircle(x, y, RADIUS, 200, 'N');
			djk->lineNodesCount = 0;
			djk->nodes[djk->nodeCount].xCoord = x;
			djk->nodes[djk->nodeCount].yCoord = y;
			djk->nodes[djk->nodeCount].id = djk->nodeCount;

			djk->nodeCount++;
		} 
		//draw line
		else
		{
			if(djk->lineNodesCount == 1)
			{
				drawLine('N');
				djk->lineNodesCount = 0;
			}
			else
			{
				djk->lineNodesCount += 1;
			}
		}
	}

	// right click: select source and destination nodes
	if(btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && (!source || !destination))
	{
		//if nodes are created
		if(djk->nodeCount)
		{	
			//if a node is clicked, lineNodes[0] will have the node clicked
			if(!noRepeat(djk, x, y))
			{
				// if source node is not assigned
				if(!source)
				{
					sourceNode = djk->selectedNodes[0];
					source = TRUE;
					// change color of the source node to (R)ed
					drawCircle(sourceNode.xCoord, sourceNode.yCoord, RADIUS, 200, 'R');
				}
				else 
				{
					//first, check if source is not selected again
					if(((sourceNode.xCoord - djk->selectedNodes[0].xCoord) != 0) || ((sourceNode.yCoord - djk->selectedNodes[0].yCoord) != 0 ))
					{
						destinationNode = djk->selectedNodes[0];
						//destination node created
						destination = TRUE;
						//change color of the destination node to (B)lue
						drawCircle(destinationNode.xCoord, destinationNode.yCoord, RADIUS, 200, 'B');
					} 
				}

			}
		}
	}

}

//return unselected node with minimum distance from the node
int minDistance(int *distanceFromSource, int *visited)
{
	int minDistance = INT_MAX, minNode;

	for(int i = 0; i < djk->nodeCount; i++)
	{
		if(!visited[i] && distanceFromSource[i] <= minDistance)
		{
			minNode = i;
			minDistance = distanceFromSource[i];
		}
	}

	return minNode;
}

//route source to destination
void routeNodes(int n1, int n2)
{
	djk->selectedNodes[0] = djk->nodes[n1];
	djk->selectedNodes[1] = djk->nodes[n2];

	drawLine('M');
}

void dijkstra()
{

	// distance of all nodes from the source node
	int distanceFromSource[MAX_NODES];

	// node connecting to the current node
	int prev[MAX_NODES];

	// if node is visited
	int visited[MAX_NODES];

	// the current node
	int cur;

	//initilize nodes distance with 99999(ie not visited yet), selected to -1(not visited yet), prev to -1(no connection yet)
	for(int i = 0; i < djk->nodeCount; i++)
	{
		distanceFromSource[i] = INT_MAX;
		prev[i] = -1;
		visited[i] = -1;
	}

	//distance of source node to 0 to select it first
	distanceFromSource[sourceNode.id] = 0;

	for(int i = 0; i < djk->nodeCount; i++)
	{
		//get node with minimum distance not selected
		cur = minDistance(distanceFromSource, visited);
		//mark it selected

    printf("\n%d\n", cur);
		if(cur == -1)
			break;

		visited[cur] = TRUE;

		//loop through all the nodes finding its neighbours
		for(int j = 0; j < djk->nodeCount; j++)
		{
			if(!visited[j])
			{	
				//if the distance from the current node is less, select it as it's previous node
				if((distanceFromSource[cur] + djk->adjList[cur][j]) < distanceFromSource[j])
				{	
					distanceFromSource[j] = distanceFromSource[cur] + djk->adjList[cur][j];

					if(prev[j] != -1)
					{	
						djk->selectedNodes[0] = djk->nodes[prev[j]];
						djk->selectedNodes[1] = djk->nodes[j];
						delay(700);
						drawLine('R');

						delay(700);
						djk->selectedNodes[0] = djk->nodes[cur];
						drawLine('D');
					}
					else
					{	
						delay(700);
						djk->selectedNodes[1] = djk->nodes[j];
						djk->selectedNodes[0] = djk->nodes[cur];
						drawLine('D');
					}

					prev[j] = cur;
				}
			}
		}

	}

	while(1)
	{
		if(prev[destinationNode.id] == -1)
			break;
		delay(500);
		routeNodes(destinationNode.id, prev[destinationNode.id]);
		destinationNode = djk->nodes[prev[destinationNode.id]];
	}

//display distance from array and previous array to console
	printf("node(i)\t\tdistance\tprevious\tselected\n");
	for(int i = 0; i < djk->nodeCount; i++){
		printf("%c\t\t%d\t\t%c\t\t%d\n",65+i, distanceFromSource[i], ((prev[i] != -1)? 65+prev[i] : ' ') , visited[i]);
	}
}

void initDijkstra(unsigned char key, int x, int y)
{
	printf("%d\n",key);

	//if source or destination nodes are not selected dont start
	if(!source|| !destination)
	{	
		printf("Source node / destination node not selected\n");
		return;
	}
	//begin dijkstra if enter key is pressed
	if((int)key == 13)
	{	
	  running = TRUE;	
		dijkstra();
	}
}

int main(int argc, char **argv) 
{
	glutInit(&argc, argv);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutCreateWindow("Dijksta's Algorithm");
	init();
  djk = createDijkstra();
	glutReshapeFunc(reshapeHandler);
	glutMouseFunc(mouseHandler);
	glutKeyboardFunc(initDijkstra);
	glutDisplayFunc(displayHandler);
	glutMainLoop();
  garbageCollector(djk);

	return 0;
}

