#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "headers/dijkstra.h"

int main(int argc, char **argv) 
{
	glutInit(&argc, argv);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutCreateWindow("Dijkstra's Algorithm");
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
