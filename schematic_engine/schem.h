#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <thread>
#define UNDEFCOORD 99999
#define UNDEFCOMP 99998
#define POSITIVE 999
#define NEGATIVE 998
#define VERTICAL 0
#define HORIZONTAL 1
#define LEDLIGHT 1
#define DCMOTOR 2
#define RESISTOR 3
#define CAPACITOR 4
#define PWRSOURCE 5
#define EMPTYCOMP 6
#define WIRECOMP 7
#define SWITCHCOMP 8
#define INDUCTOR 9
#define MEGA 13
#define MICRO 10
#define ULTRASONIC 11
#define INFRARED 12
#define NRF 14
#define SERVO 15

struct instruct {
	char command[32];
	int params[4];
};
extern char currentcommand[32];
extern LPWSTR *cmdtext;
extern int globalluminosity;
extern int globalres;
extern int globalcapacity;
extern int globaltorque;
extern bool invokestate;
extern FILE* fp;
extern HWND hWnd;
extern int gridstep;
extern int selectedcomponent;
extern HDC hdc;
extern HDC memdc;
extern char boardfn[64];
extern std::vector<struct instruct> line;
extern void checkfile(void);
extern void pushcomponent(struct instruct temp, int selectedcomponent, int fx, int fy, int gx, int gy);
extern VOID MainRender(HDC hdc);
extern void rendergrid(HDC hdc);
extern int renderboard(const char* boardfile, int mode);
extern void mouseup(int x, int y);
extern void mousemove(int x, int y);
extern void mouselmove(int x, int y);
extern void mousermove(int x, int y);
extern void mouselbutton(int x, int y);
extern void saveboard(const char* fn);
extern void pushbuffer(HDC hdc);