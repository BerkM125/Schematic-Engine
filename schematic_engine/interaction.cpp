#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "schematic_engine.h"
#include "schem.h"
using namespace Gdiplus;
#pragma comment (lib,"gdiplus.lib")

int dragx1 = UNDEFCOORD, dragy1 = UNDEFCOORD, dragx2 = UNDEFCOORD, dragy2 = UNDEFCOORD;
int oldx1 = UNDEFCOORD, oldy1 = UNDEFCOORD;

void mouseup(int x, int y) {
	hdc = GetDC(hWnd);
	Graphics maingraph(memdc);
	int i;
	char string[32];
	bool matchingcomp = false;
	dragx2 = x / gridstep;
	dragy2 = y / gridstep;
	sprintf(string, "\0");
	sprintf(string, "%d %d %d %d", oldx1, oldy1, dragx2, dragy2);
	if (dragx1 != UNDEFCOORD && dragy1 != UNDEFCOORD) {
		for (i = 0; i < line.size(); i++) {
			if (oldx1 >= line[i].params[0] && oldx1 <= line[i].params[0]+XTOLERANCE && oldy1 >= line[i].params[1] && oldy1 <= line[i].params[1]+YTOLERANCE && strcmp(line[i].command, "wire") != 0) {
				matchingcomp = true;
				break;
			}
		}
	}
	else return;
	if (matchingcomp == true && dragx2 != UNDEFCOORD && dragy2 != UNDEFCOORD) {
		int dx, dy;
		dx = line[i].params[2] - line[i].params[0];
		dy = line[i].params[3] - line[i].params[1];
		line[i].params[0] = dragx2;
		line[i].params[1] = dragy2;
		maingraph.Clear(Color(255, 255, 255));
		renderboard(boardfn, 1);
		rendergrid(memdc);
	}
	pushbuffer(hdc);
	ReleaseDC(hWnd, hdc);
	maingraph.Flush();
}

void mouselmove (int x, int y) {
	hdc = GetDC(hWnd);
	Graphics maingraph(memdc);
	Pen whitepen(Color(255, 255, 255), 2.0F);
	Pen mainpen(Color(255, 0, 72), 2.0F);
	int amnt = gridstep;
	static int gx = UNDEFCOORD, gy = UNDEFCOORD;
	if (gx != UNDEFCOORD && gy != UNDEFCOORD) {
		Rect oldrect((gx * gridstep) + amnt, (gy * gridstep) + amnt, amnt, amnt);
		maingraph.DrawRectangle(&whitepen, oldrect);
	}
	dragx1 = x / gridstep;
	dragy1 = y / gridstep;
	gx = x / gridstep;
	gy = y / gridstep;
	gx -= 1;
	gy -= 1;
	Rect normrect((gx * gridstep) + amnt, (gy * gridstep) + amnt, amnt, amnt);
	maingraph.DrawRectangle(&mainpen, normrect);
	rendergrid(memdc);
	pushbuffer(hdc);
	ReleaseDC(hWnd, hdc);
	maingraph.Flush();
	return;
}

void mousermove (int x, int y) {

}

void mousemove (int x, int y) {
	hdc = GetDC(hWnd);
	Graphics maingraph(hdc);
	Pen whitepen(Color(255,255,255), 2.0F);
	Pen mainpen(Color(0, 0, 255), 2.0F);
	int amnt = gridstep;
	int index;
	static int gx = UNDEFCOORD, gy = UNDEFCOORD, indexstate = 0;
	rendergrid(hdc);
	if (gx != UNDEFCOORD && gy != UNDEFCOORD) {
		Rect oldrect((gx * gridstep) + amnt, (gy * gridstep) + amnt, amnt, amnt);
		maingraph.DrawRectangle(&whitepen, oldrect);
	}
	gx = x / gridstep;
	gy = y / gridstep;
	gx -= 1;
	gy -= 1;
	Rect normrect((gx * gridstep) + amnt, (gy * gridstep) + amnt, amnt, amnt);

	maingraph.DrawRectangle(&mainpen, normrect);
	ReleaseDC(hWnd, hdc);
	maingraph.Flush();
}

void mouselbutton(int x, int y) {
	struct instruct temp{};
	static int buttondownstate = 0, fx = UNDEFCOORD, fy = UNDEFCOORD, gx = UNDEFCOORD, gy = UNDEFCOORD;
	oldx1 = x / gridstep;
	oldy1 = y / gridstep;
	if (selectedcomponent != UNDEFCOMP) {
		if (buttondownstate == 0) {
			fx = x / gridstep;
			fy = y / gridstep;
			buttondownstate = 1;
		}
		else if (buttondownstate == 1) {
			gx = x / gridstep;
			gy = y / gridstep;
			if (selectedcomponent != WIRECOMP) {
				if (gy < fy) {
					int tmp = gy;
					gy = fy;
					fy = tmp;
				}
				if (gx < fx) {
					int tmp = gx;
					gx = fx;
					fx = tmp;
				}
				temp.params[0] = fx;
				temp.params[1] = fy;
				if (fx == gx)
					temp.params[2] = 0;
				else
					temp.params[2] = 1;
			}
			pushcomponent(temp, selectedcomponent, fx, fy, gx, gy);
			buttondownstate = 0;
			selectedcomponent = UNDEFCOMP;
		}
	}
}

void pushcomponent(struct instruct temp, int selectedcomponent, int fx, int fy, int gx, int gy) {
	switch (selectedcomponent) {
		case WIRECOMP:
			temp.params[0] = fx;
			temp.params[1] = fy;
			temp.params[2] = gx;
			temp.params[3] = gy;
			strcpy(temp.command, "wire");
			break;
		case LEDLIGHT:
			temp.params[3] = globalluminosity;
			strcpy(temp.command, "led");
			break;
		case INDUCTOR:
			temp.params[3] = 0;
			strcpy(temp.command, "inductor");
			break;
		case SWITCHCOMP:
			temp.params[3] = 0;
			strcpy(temp.command, "switch");
			break;
		case MEGA:
			temp.params[3] = 0;
			strcpy(temp.command, "mega");
			break;
		case DCMOTOR:
			temp.params[3] = globaltorque;
			strcpy(temp.command, "dcmotor");
			break;
		case RESISTOR:
			temp.params[3] = globalres;
			strcpy(temp.command, "resistor");
			break;
		case CAPACITOR:
			temp.params[3] = globalcapacity;
			strcpy(temp.command, "capacitor");
			break;
		case ULTRASONIC:
			temp.params[3] = 40;
			strcpy(temp.command, "ultrasonic");
			break;
		case INFRARED:
			temp.params[3] = 35;
			strcpy(temp.command, "infrared");
			break;
		case NRF:
			temp.params[3] = 2.4;
			strcpy(temp.command, "nrf");
			break;
		case SERVO:
			temp.params[3] = 9.4;
			strcpy(temp.command, "servo");
			break;
		case MICRO:
			temp.params[3] = 32;
			strcpy(temp.command, "micro");
			break;
		case BUZZER:
			temp.params[3] = 125;
			strcpy(temp.command, "buzzer");
			break;
		case NANO:
			temp.params[3] = 125;
			strcpy(temp.command, "nano");
			break;
		default:
			break;
	}
	line.push_back(temp);
	renderboard(boardfn, 1);
}