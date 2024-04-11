#include "../headers/dijkstra.h"
#include "../headers/settings.h"

Node sourceNode, destinationNode;
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
	// change the color to black 
	glColor3f(0.0, 0.0, 0.0);
    glRasterPos2i(x,y);
  glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text);
}


//draw Circle at the given coordinates
void drawCircle(float cx, float cy, float r, float num_segments, enum Colors colorCharacter)
{
	// theta: angle in radians of the point 
	// x: x coordinate of point
	// y: y coordinate of point
	float theta, x, y;
  int triangleQty = 20;

  checkColor(colorCharacter);

	// line loop: connect all the points below 
	glBegin(GL_TRIANGLE_FAN);
 
    for(int i = 0; i < triangleQty; i++){
      // compute new angle and get the x, y coordinate
      theta = 2.0f * PI * i / triangleQty;
      x = r * cosf(theta);
      y = r * sinf(theta);
      
      // draw the point
      glVertex2f(x + cx, y + cy);
    }
	glEnd();
 
  // Only draws the label if is a new node
  if(colorCharacter == WHITE)
    writeLabel((char)65 + djk->nodeCount, cx - 5, cy - 14);
  else if(colorCharacter == BLUE) 
    writeLabel('A' + sourceNode.id, cx - 5, cy - 14);
  else
    writeLabel('A' + destinationNode.id, cx - 5, cy - 14);

	// flush to output screen
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
void writeText(char *text, int x, int y, enum Colors color)
{	
  checkColor(color);
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

void drawLine(enum Colors color) 
{
  checkColor(color);
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
	writeText(distanceString, midX, midY, YELLOW);

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
	writeText("Dijkstra Algorithm Simulation\n", 200, 900, WHITE);
	writeText("- Use Mouse Left Button to Create Nodes.", 250, 800, WHITE);
	writeText("- Select any two Nodes to form a Path.", 250, 770, WHITE);
	writeText("- Use Mouse Right Button to select Source and Destination.", 250, 740, WHITE);
	writeText("- Press ENTER key to begin Dijkstra's Algorithm.", 250, 710, WHITE);
	writeText("CLICK ANYWHERE TO CREATE A NODE!", 250, 600, WHITE);
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

void showInfo(){
	writeText("BLUE ", 100, 1000, BLUE);
	writeText("for the Source Node\n", 170, 1000, WHITE);
	writeText("RED ", 100, 950, RED);
	writeText("for the Destination Node\n", 150, 950, WHITE);
	writeText("ORANGE ", 100, 900, ORANGE);
	writeText("for the path taken by the algorithm\n", 180, 900, WHITE);
}

// mouse handler event 
void mouseHandler(int btn, int state, int x, int y) 
{

	if(instruction)
	{
		instruction = 0;
		glClear(GL_COLOR_BUFFER_BIT);
    showInfo();
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

			drawCircle(x, y, RADIUS, 200, WHITE);
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
				drawLine(GRAY);
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
		// if nodes are created
		if(djk->nodeCount)
		{	
			// if a node is clicked, lineNodes[0] will have the node clicked
			if(!noRepeat(djk, x, y))
			{
				// if source node is not assigned
				if(!source)
				{
					sourceNode = djk->selectedNodes[0];
					source = TRUE;
					// change color of the source node to BLUE 
          drawCircle(sourceNode.xCoord, sourceNode.yCoord, RADIUS, 200, BLUE);
				}
				else 
				{
					//first, check if source is not selected again
					if(((sourceNode.xCoord - djk->selectedNodes[0].xCoord) != 0) || ((sourceNode.yCoord - djk->selectedNodes[0].yCoord) != 0 ))
					{
						destinationNode = djk->selectedNodes[0];
						destination = TRUE;

						// change color of the destination node to RED
					drawCircle(destinationNode.xCoord, destinationNode.yCoord, RADIUS, 200, RED);
					} 
				}

			}
		}
	}

}

//return unselected node with minimum distance from the node
int minDistance(int dist[], int visited[])
{
	int minDistance = INT_MAX, minNode;

	for(int i = 0; i < djk->nodeCount; i++)
	{
		if(!visited[i] && dist[i] <= minDistance)
		{
			minNode = i;
			minDistance = dist[i];
		}
	}

	return minNode;
}

// route source to destination
void writeDistance(int distance)
{
  char buffer[10];
  sprintf(buffer, "%d", distance);

  writeText(buffer, 100, 850, YELLOW);
  writeText(" is the distance amount to go to the source to the dest node", 150, 850, WHITE);
}

void dijkstra()
{
	// distance of all nodes from the source node
	int dist[djk->nodeCount];

	// if node is visited
	int visited[djk->nodeCount];

  int prev[djk->nodeCount];
	//initilize nodes distance with 99999(ie not visited yet), selected to -1(not visited yet), prev to -1(no connection yet)
	for(int i = 0; i < djk->nodeCount; i++)
	{
		dist[i] = INT_MAX;
		visited[i] = FALSE;
    prev[i] = FALSE;
	}

	//distance of source node to 0 to select it first
	dist[sourceNode.id] = 0;

	for(int i = 0; i < djk->nodeCount - 1; i++)
	{
		int u = minDistance(dist, visited);

		visited[u] = TRUE;

		//loop through all the nodes finding its neighbours
		for(int v = 0; v < djk->nodeCount; v++)
		{
			if(!visited[v] && djk->adjList[u][v] && dist[u] != INT_MAX && 
        (dist[u] + djk->adjList[u][v]) < dist[v])
			{	
        dist[v] = dist[u] + djk->adjList[u][v];
        delay(700);
        djk->selectedNodes[1] = djk->nodes[v];
        djk->selectedNodes[0] = djk->nodes[u];
        drawLine(ORANGE);
        prev[v] = u;
			}
		}
	}

  writeDistance(dist[destinationNode.id]);
  delay(500);

  // DEBUG purpose 
  #ifdef DEBUG
    printf("node(i)\t\tdistance\tprevious\tselected\n");
    for(int i = 0; i < djk->nodeCount; i++)
    {
      printf("%c\t\t%d\t\t%c\t\t%d\n",65+i, dist[i], ((prev[i] != -1)? 65+prev[i] : ' ') , visited[i]);
    }
  #endif
}

void initDijkstra(unsigned char key, int x, int y)
{
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
