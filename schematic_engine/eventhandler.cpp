#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <vector>
#include "schematic_engine.h"
#include "schem.h"
using namespace Gdiplus;
using namespace std;
#pragma comment (lib,"gdiplus.lib")

int dragx1 = UNDEFCOORD, dragy1 = UNDEFCOORD, dragx2 = UNDEFCOORD, dragy2 = UNDEFCOORD;
int oldx1 = UNDEFCOORD, oldy1 = UNDEFCOORD;
HCURSOR mcursor;
bool mousedragging = false;

//Checks if a coordinate pair (in grid block units) is within any existing component bounds
int withincompbounds(int xval, int yval, bool wirecheck) {
	/*for (int i = 0; i < line.size(); i++) {
		if (line[i].compmacro == WIRECOMP && !wirecheck) continue;
		if (xval + 1 >= line[i].params[0] && xval + 1 <= line[i].params[0] + XTOLERANCE &&
			yval + 1 >= line[i].params[1] && yval + 1 <= line[i].params[1] + YTOLERANCE) {
			return i;
		}
	}*/
	vector<int> list = mainhierarchy.searchHierarchy(xval);
	if (list.size() == 0) return -1;
	std::cout << list.size() << std::endl;
	//mainhierarchy.traverse();
	for (int i = 0; i < list.size(); i++) {
		//cout << "XCOORD: " << xval << " LIST[I]: " << list[i] << " LINE[LIST[I]]: " << line[list[i]].params[0] << endl;
		if (line[list[i]].compmacro == WIRECOMP && !wirecheck) continue;
		if (xval + 1 >= line[list[i]].params[0] && xval + 1 <= line[list[i]].params[0] + XTOLERANCE &&
			yval + 1 >= line[list[i]].params[1] && yval + 1 <= line[list[i]].params[1] + YTOLERANCE) {
			return list[i];
		}
	}
	return -1;
}

//Mouse up event callback
void mouseup(int x, int y) {
	if (!mousedragging)
		return;
	hdc = GetDC(hWnd);
	Graphics maingraph(memdc);
	int i;
	char string[32];
	bool matchingcomp = false;
	dragx2 = x / gridstep;
	dragy2 = y / gridstep;
	//Using coordinates, check to see if there is a component that exists when finishing dragging maneuver
	if (dragx1 != UNDEFCOORD && dragy1 != UNDEFCOORD) {
		i = withincompbounds(oldx1, oldy1, false);
		//cout << i << endl;
		if (i != -1) {
			matchingcomp = true;
		}
	}
	else return;
	//Now if there was a matching component, the index, i, is the index of the component in the vector of electronic components, 
	//and thus we can use that
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
	mousedragging = false;
}

//Mouse move while left button is clicked, indicating a dragging action
void mouselmove(int x, int y) {
	hdc = GetDC(hWnd);
	Graphics maingraph(memdc);
	Pen whitepen(Color(255, 255, 255), 4.0F);
	Pen mainpen(Color(255, 0, 72), 4.0F);
	int amnt = gridstep;
	static int gx = UNDEFCOORD, gy = UNDEFCOORD;
	mousedragging = true;
	dragx1 = x / gridstep;
	dragy1 = y / gridstep;
	gx = x / gridstep;
	gy = y / gridstep;
	gx -= 1;
	gy -= 1;
	SetCursor(LoadCursor(NULL, IDC_SIZEALL));
	ReleaseDC(hWnd, hdc);
	maingraph.Flush();
	return;
}

void mousermove(int x, int y) {

}

//Idle mouse movement
void mousemove(int x, int y) {
	hdc = GetDC(hWnd);
	Graphics maingraph(hdc);
	Pen whitepen(Color(255, 255, 255), 2.5F);
	Pen mainpen(Color(0, 0, 255), 2.5F);
	int amnt = gridstep;
	int index;
	static int gx = UNDEFCOORD, gy = UNDEFCOORD, indexstate = 0;
	rendergrid(hdc);
	gx = x / gridstep;
	gy = y / gridstep;
	gx -= 1;
	gy -= 1;
	Rect normrect((gx * gridstep) + amnt, (gy * gridstep) + amnt, amnt, amnt);
	if (withincompbounds(gx+1, gy+1, false)!=-1) {
		normrect.Width += gridstep * 3;
		normrect.Height += gridstep * 3;
		mainpen.SetColor(Color(47, 219, 222));
	}
	
	maingraph.DrawRectangle(&mainpen, normrect);
	SetCursor(LoadCursor(NULL, IDC_CROSS));
	ReleaseDC(hWnd, hdc);
	maingraph.Flush();
}
 
//Right mouse click
void mouserbutton(int x, int y) {
	hdc = GetDC(hWnd);
	Graphics maingraph(memdc);
	int gx, gy, indx;
	gx = x / gridstep;
	gy = y / gridstep;
	gx -= 1;
	gy -= 1;
	indx = withincompbounds(gx, gy, false);
	if (indx!=-1) {
		line.erase(line.begin() + indx);
		maingraph.Clear(Color(255, 255, 255));
		renderboard(boardfn, 1);
		rendergrid(memdc);
		pushbuffer(hdc);
	}
	ReleaseDC(hWnd, hdc);
}

//Left mouse click
void mouselbutton(int x, int y) {
	struct instruct temp {};
	static int buttondownstate = 0, fx = UNDEFCOORD, fy = UNDEFCOORD, gx = UNDEFCOORD, gy = UNDEFCOORD;
	oldx1 = x / gridstep;
	oldy1 = y / gridstep;
	mainhierarchy.searchHierarchy(oldx1);
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
			OutputDebugStringW(L"PUSHED");
			buttondownstate = 0;
			selectedcomponent = UNDEFCOMP;
		}
	}
}

//Use a parameterstruct object to extract data from parametermap and copy into temp,
//push back temp into the component rendering list.
void pushcomponent(struct instruct temp, int selectedcomponent, int fx, int fy, int gx, int gy) {
	switch (selectedcomponent) {
	case WIRECOMP:
		temp.params[0] = fx;
		temp.params[1] = fy;
		temp.params[2] = gx;
		temp.params[3] = gy;
		strcpy(temp.command, "wire");
		break;
	default: 
	{
		parameterstruct ps = parametermap[selectedcomponent];
		temp.params[3] = ps.unitparam;
		temp.compmacro = selectedcomponent;
		strcpy(temp.command, ps.label);
		break;
	}
	}
	line.push_back(temp);
	renderboard(boardfn, 1);
}