#ifndef SETTINGS_H
#define SETTINGS_H
#define HEIGHT 1020
#define WIDTH 1080
#define PI 3.14285714286
#define TRUE 1
#define FALSE 0
#define RADIUS 30
#include <time.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>

extern int running; 
extern int instruction;
extern int source;
extern int destination;

enum Colors {
  BLUE,
  RED,
  WHITE,
  YELLOW,
  ORANGE,
  GRAY
};

void checkColor(enum Colors color);
void delay(int number_of_seconds);

#endif 
