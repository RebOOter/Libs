/* 
 * File:   main.cpp
 * Author: RebOOter
 *
 * Created on 15 Июнь 2013 г., 01:54
 */

#include <stdio.h>
#include "./libs/SDL/SDL.h"
#include "./libs/SDL/SDL_opengl.h"
#include <math.h>
#include <string.h>
#include <fstream>
#include <time.h>

#define WIDTH 512
#define HEIGHT 512
#define N 10



using namespace std;


typedef struct SPoint
{
    float x, y;
}SPoint;

typedef struct SLine{
    float k, b;
    float A, B, C;
} SLine;

SLine init_line(SPoint p1, SPoint p2){
    SLine l;
    l.A = p1.y - p2.y;
    l.B = p2.x - p1.x;
    l.C = p1.x*p2.y - p2.x*p1.y;
    l.k = - (l.A / l.B);
    l.b = - (l.C / l.B);
    return l;
}

SLine normal(SLine in, SPoint p){
    SLine n;
    n.A = in.B;
    n.B = -in.A;
    n.C = - (n.A*p.x + n.B*p.y);
    n.k = - (n.A / n.B);
    n.b = - (n.C / n.B);
    return n;
}

void draw_line(SLine line, float r, float g, float b){
    glColor3f(r, g, b);
    glLineWidth(1);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < WIDTH; i++){
        glVertex2f(-(line.B/line.A)*i - (line.C/line.A), i);
    }
    glEnd();
}

float distance(SLine l, SPoint p){
    float numer = abs(l.A*p.x + l.B*p.y + l.C);
    float denom = sqrt(l.A*l.A + l.B*l.B);
    return numer/denom;
}

SPoint init_point(float x, float y)
{
    SPoint p;
    p.x = x; p.y = y;
    return p;
}

SPoint center(SPoint p1, SPoint p2){
    SPoint n;
    n.x = (p1.x + p2.x)/2;
    n.y = (p1.y + p2.y)/2;
    return n;
}

void draw_point(SPoint p, float r, float g, float b){
    glPointSize(4);
    glColor4f(r, g, b, 0);
    glBegin(GL_POINTS);
        glVertex2f(p.x, p.y);
    glEnd();
}

void draw_straight(SPoint p1, SPoint p2, float r, float g, float b){
    glColor3f(r, g, b);
    glBegin(GL_LINE_LOOP);
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glEnd();
}

void print_point(SPoint p){
    printf("x = %f; y = %f\n", p.x, p.y);
}

/*  Работа с SDL  */

void sdldie(const char *msg)
{
	printf("%s: %s\n", msg, SDL_GetError());
	SDL_Quit();
	exit(1);
    
}

void checkSDLError(int line = -11){
#ifndef NDEBUG
	const char *error = SDL_GetError();
	if (*error != '\0'){
		printf("SDL Error: $s\n", error);
		if (line != -1)
			printf(" + line: %i\n", line);
		SDL_ClearError();
    }
#endif
}

int initSDL_GL(){
	if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
		sdldie("Unable to initialize SDL"); //or die in error
		SDL_Delay(3000);
		exit(1);
    }
    
	SDL_SetVideoMode(WIDTH,HEIGHT,24,SDL_SWSURFACE | SDL_OPENGL | SDL_GL_DOUBLEBUFFER);
	
	glMatrixMode(GL_PROJECTION);
	
	glLoadIdentity();
	glOrtho(0, WIDTH, HEIGHT, 0, -1, 1 );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glClearColor(0.0, 0.0, 0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	//SDL_GL_SwapBuffers();
        return 0;
}

/*  draw_array забивает в буфер OpenGL графики  */
int draw_array(int length,double *array,int pos,int scale){
      //  glPointSize(16);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
			
	glPointSize(1);
	glBegin(GL_LINE_STRIP);
        	for (int i=0;i<length;i++)
			glVertex2f((float)i*2,-array[i]*scale+pos);
	glEnd();
	
        return 0;
}
/*
 * 
 */
int main(int argc, char** argv){
    
    srand(time(NULL));
    int done = 0;
    bool flag = false;
    int n_min = 0;
    int min = 0;
    int now = 0;
    SPoint temp;
    SLine line;
    
    SPoint p[N];
    
    for (int i = 0; i < N; i++)
    {
        p[i] = init_point(rand()%500+1, rand()%500+1);
        print_point(p[i]);
    }
    
    now = 0;
    while (!flag)
    {
        min = p[n_min].x;
        for (int i = now+1; i < N; i++)
        {
            if (p[i].x < min)
            {
                n_min = i; min = p[i].x;
            }
        }
        temp = p[now];
        p[now] = p[n_min];
        p[n_min] = temp;
        now++; n_min = now;
        if (now == N) flag = true;
    }
    
    flag = false;
    
    printf("Afret sort:\n");
    for (int i = 0; i < N; i++){
        print_point(p[i]);
    }
    
    initSDL_GL();
    
    SDL_Event event;
    
    while(done == 0)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT){ done = 1; }
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE) { done = 1; }
            }
        }
        for (int i = 0; i < N; i++){
            draw_point(p[i], 1, 0, 0);
        }
        
        now = 0;
        while (!flag){
            int n_d = now+1;
            float d = distance(line, p[n_d]);
            for (int i = now; i < N; i++){
                if (i != now)
                if (d > distance(line, p[i])){
                    d = distance(line, p[i]);
                    n_d = i;
                }
            }
            temp = center(p[now], p[n_d]);
            draw_point(temp, 0, 1, 1);
            //находим прямую по двум точкам
            line = init_line(temp, p[now]);
            draw_straight(p[now], p[now+1], 1, 0, 0);
            //находим перпендикуляр к прямой, проходящий ч/з точку
            line = normal(line, p[now]);
            draw_line(line, 0, 1, 0);
            //находим ближаюшую точку к перпендикуляру
            //draw_straight(p[now], p[n_d], 1, 0, 0);
            //draw_straight(p[n_d], p[now+1], 1, 0, 0);
            
            flag = true;
        }
        SDL_GL_SwapBuffers();
    }
    
    return 0;

}