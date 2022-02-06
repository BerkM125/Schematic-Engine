#pragma once
#include <windows.h>
#include <windowsx.h>
#include <objidl.h>
#include <gdiplus.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include <thread>
#include <unordered_set>
using namespace Gdiplus;
#pragma comment (lib,"gdiplus.lib")
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
#define MICRO 16
#define BUZZER 17
#define NANO 18
#define DHT11 19
#define NODEMCU 20
#define ACCELEROMETER 21
#define XTOLERANCE 5
#define YTOLERANCE 5
#define PROCESS_FILETYPE 1
#define PROCESS_STACKTYPE 2

struct instruct {
	int params[4];
	unsigned int compmacro;
	char command[32];
};

class parameterstruct {
public:
	int unitparam;
	char label[32];
	parameterstruct(int up = 0, const char* lb = "") {
		unitparam = up;
		strcpy(label, lb);
	}
	parameterstruct(const parameterstruct& nv) {
		unitparam = nv.unitparam;
		strcpy(label, nv.label);
	}
};

class component {
public:
	int comptype;
	int compval;
	int orientation;
	wchar_t unit[16];
	Image* compimg;
	void rendercomponent(void);
	void setcoords(int x, int y, int pole);
	void setcompvalue(int ori, int val);
	void setrenderingcoords(float x1, float y1, float x2, float y2);
	//component(int ctype = EMPTYCOMP, int posxcoord = 0, int posycoord = 0, int negxcoord = 0, int negycoord = 0);
	component(int ctype = EMPTYCOMP, float renderx1 = 0, float rendery1 = 0, float renderx2 = 0, float rendery2 = 0, 
		Image* cimg = NULL, const wchar_t* unitval = L"");
	component(const component& nv) {
		comptype = nv.comptype;
		compval = nv.compval;
		orientation = nv.orientation;
		wcsncpy(unit, nv.unit, 16);
		compimg = nv.compimg;
		positivexcoord = nv.positivexcoord;
		positiveycoord = nv.positiveycoord;
		negativexcoord = nv.negativexcoord;
		negativeycoord = nv.negativeycoord;
		pxdiff1 = nv.pxdiff1;
		pxdiff2 = nv.pxdiff2;
		pydiff1 = nv.pydiff1;
		pydiff2 = nv.pydiff2;
	}
private:
	int positivexcoord;
	int positiveycoord;
	int negativexcoord;
	int negativeycoord;
	float pxdiff1;
	float pxdiff2;
	float pydiff1;
	float pydiff2;
}; 

//BST tree hierarchy structure based on component x-coordinate values 
class componentTreeHierarchy {
public:
	componentTreeHierarchy() {
		val = 0;
		left = NULL;
		right = NULL;
	}

	std::vector<int> searchHierarchy(int target);
	void traverse(void);
	void insertComponent(int target, int data);
	//int searchHierarchy(struct instruct);
	//void insertComponent(struct instruct);
private:
	int val;
	std::vector<int> positionval;
	std::unordered_set<int> positionset;
	componentTreeHierarchy* left;
	componentTreeHierarchy* right;

	void searchHierarchyUtil(componentTreeHierarchy* node, int target, std::vector<int>& holder);
};

extern componentTreeHierarchy mainhierarchy;
extern Image* resistorhz;
extern Image* dcmotorhz;
extern Image* capacitorhz;
extern Image* ledhz;
extern Image* switchhz;
extern Image* inductorhz;
extern Image* megahz;
extern Image* ultrasonichz;
extern Image* infraredhz;
extern Image* nrfhz;
extern Image* servo;
extern Image* micro;
extern Image* buzzer;
extern Image* nano;
extern Image* dht11;
extern Image* nodemcu;
extern Image* accelerometer;

extern char currentcommand[32];
extern LPWSTR* cmdtext;
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
extern HINSTANCE hInst;
extern HCURSOR mcursor;
extern char boardfn[64];
extern std::vector<struct instruct> line;
extern std::map<int, parameterstruct> parametermap;
extern std::map<int, component> componentmap;
extern std::map<std::string, int> commandmap;
extern std::stack<std::vector<struct instruct>> undostk;
extern std::stack<std::vector<struct instruct>> redostk;
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
extern void mouserbutton(int x, int y);
extern void saveboard(const char* fn);
extern void pushbuffer(HDC hdc);
extern void initparametermapping(void);
extern void initcomponentmapping(void);
extern void pushdata(std::vector<struct instruct> data);
extern void undoaction(void);
extern void redoaction(void);