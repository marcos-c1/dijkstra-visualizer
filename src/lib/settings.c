#include "../headers/settings.h"

int running = FALSE;
int instruction = TRUE;
int source = FALSE;
int destination = FALSE;


void checkColor(enum Colors color){
  switch(color){
  case BLUE: 
    glColor3f(0.0, 0.0, 1.0);
    break;
  case RED:
    glColor3f(1.0, 0.0, 0.0);
    break;
  case WHITE: 
    glColor3f(1.0, 1.0, 1.0);
    break;
  case YELLOW:
    glColor3f(1.0, 1.0, 0.0);
    break;
  case ORANGE:
    glColor3f(1.0, 0.5, 0.0);
    break;
  case GRAY:
    glColor3f(0.3, 0.3, 0.3);
    break;
  default:
    fprintf(stderr, "Color doenst exist.\n"); 
  }
}

void delay(int number_of_seconds)
{
    int milli_seconds = 1000 * number_of_seconds;
    clock_t start_time = clock();

    // looping till required time is not achieved 
    while (clock() < start_time + milli_seconds);
}
