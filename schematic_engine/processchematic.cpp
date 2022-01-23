#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <iostream>
#include <vector>
#include <thread>
#include "framework.h"
#include "schematic_engine.h"
#include "schem.h"
using namespace Gdiplus;
#pragma comment (lib,"gdiplus.lib")

FILE* fp;
int gridstep = 20;
int currentstate = 0;

std::vector<struct instruct> line;
bool invokestate = false;

//Configures all preferred pen drawing settings, change to your liking
void configurepensettings(Graphics* graph, Pen* mainpen, Pen* negativepen, Pen* positivepen) {
	graph->SetSmoothingMode(SmoothingModeAntiAlias);
	graph->SetTextRenderingHint(TextRenderingHintAntiAlias);
	mainpen->SetStartCap(LineCapRound);
	mainpen->SetEndCap(LineCapRound);
	negativepen->SetStartCap(LineCapRound);
	negativepen->SetEndCap(LineCapRound);
	positivepen->SetStartCap(LineCapRound);
	positivepen->SetEndCap(LineCapRound);
	return;
}

//Draw a single component onto memory buffer using virtual device context
void component::rendercomponent(void) {
	static int configurationstate = 0;
	Graphics graph(memdc);
	Pen mainpen(Color(200, 128, 128, 128), (REAL)(gridstep / 3));
	Pen negativepen(Color(200, 27, 155, 224), (REAL)(gridstep / 3));
	Pen positivepen(Color(200, 224, 27, 27), (REAL)(gridstep / 3));
	FontFamily  fontFamily(L"Calibri");
	Font        font(&fontFamily, gridstep, FontStyleRegular, UnitPixel);
	SolidBrush  solidBrush(Color(255, 0, 0, 255));
	WCHAR       positivestr[] = L"+ VCC";
	WCHAR       negativestr[] = L"⏚ GND";
	configurepensettings(&graph, &mainpen, &negativepen, &positivepen);
	if (configurationstate == 0) {
		resistorhz = Image::FromFile(L"horizontalres.png");
		dcmotorhz = Image::FromFile(L"dcmotorhz.png");
		capacitorhz = Image::FromFile(L"capacitorhz.png");
		ledhz = Image::FromFile(L"hzled.png");
		switchhz = Image::FromFile(L"switch.png");
		inductorhz = Image::FromFile(L"inductor.png");
		megahz = Image::FromFile(L"mega2560.png");
		ultrasonichz = Image::FromFile(L"ultrasonichz.png");
		infraredhz = Image::FromFile(L"infraredhz.png");
		nrfhz = Image::FromFile(L"nrfhz.PNG");
		servo = Image::FromFile(L"servo.PNG");
		micro = Image::FromFile(L"micro.PNG");
		buzzer = Image::FromFile(L"buzzer.PNG");
		nano = Image::FromFile(L"nano.PNG");
		dht11 = Image::FromFile(L"dht11_2.png");
		nodemcu = Image::FromFile(L"nodemcu.png");
		accelerometer = Image::FromFile(L"accelerometer.png");
	}
	configurationstate += 1;
	//Calculate coordinates according to circuit pin grid
	int px, py, nx, ny;
	px = (positivexcoord * gridstep) + (gridstep / 2);
	py = (positiveycoord * gridstep) + (gridstep / 2);
	nx = (negativexcoord * gridstep) + (gridstep / 2);
	ny = (negativeycoord * gridstep) + (gridstep / 2);
	switch (comptype) {
	case WIRECOMP:
		if ((positiveycoord * gridstep) == 0 || (negativeycoord * gridstep) == 0) {
			graph.DrawLine(&negativepen, px, py, nx, ny);
			graph.DrawString(negativestr, -1, &font, PointF(px + (nx - px), (ny - py) / 2), &solidBrush);
		}
		else if ((positiveycoord * gridstep) == gridstep || (negativeycoord * gridstep) == gridstep) {
			graph.DrawLine(&positivepen, px, py, nx, ny);
			graph.DrawString(positivestr, -1, &font, PointF(px + (nx - px), ((ny - py) / 2) - gridstep), &solidBrush);
		}
		else
			graph.DrawLine(&mainpen, px, py, nx, ny);
		break;
	default:
		if (orientation == 1) {
			graph.DrawImage(compimg, px - pxdiff1, py - pydiff1, gridstep * pxdiff2, gridstep * pydiff2);
			graph.DrawString(unit, -1, &font, PointF(px + gridstep * 1.2, py + gridstep * 1.7), &solidBrush);
		}
		else if (orientation == 0) {
			//Removed vertical alignment rotation for now due to a few issues in refreshing and updating
			/*compimg->RotateFlip(Rotate90FlipXY);
			graph.DrawImage(compimg, px - pydiff1, py - pxdiff1, gridstep * pydiff2, gridstep * pxdiff2);
			graph.DrawString(unit, -1, &font, PointF(px + gridstep * 1.2, py + gridstep * 1.7), &solidBrush);*/
			//Stick to horizontal alignment for now
			graph.DrawImage(compimg, px - pxdiff1, py - pydiff1, gridstep * pxdiff2, gridstep * pydiff2);
			graph.DrawString(unit, -1, &font, PointF(px + gridstep * 1.2, py + gridstep * 1.7), &solidBrush);
		}
		break;
	}
	
	return;
}

//Set component object's coordinates in grid space
void component::setcoords(int x, int y, int pole) {
	if (pole == POSITIVE) {
		positivexcoord = x;
		positiveycoord = y;
	}
	if (pole == NEGATIVE) {
		negativexcoord = x;
		negativeycoord = y;
	}
	return;
}

//Set orientation (currently deprecated), main component label value (numeric value)
void component::setcompvalue(int ori, int val) {
	orientation = ori;
	compval = val;
}

//Set component image rendering coordinates
void component::setrenderingcoords(float x1, float y1, float x2, float y2) {
	pxdiff1 = x1;
	pxdiff2 = x2;
	pydiff1 = y1;
	pydiff2 = y2;
}

/*component::component(int ctype, int posxcoord, int posycoord, int negxcoord, int negycoord) {
	comptype = ctype;
	positivexcoord = posxcoord;
	positiveycoord = posycoord;
	negativexcoord = negxcoord;
	negativeycoord = negycoord;
}*/

component::component(int ctype, float renderx1, float rendery1, float renderx2, float rendery2,
	Image* cimg, const wchar_t* unitval) {
	comptype = ctype;
	compimg = cimg;
	pxdiff1 = renderx1;
	pxdiff2 = renderx2;
	pydiff1 = rendery1;
	pydiff2 = rendery2;
	wcsncpy(unit, unitval, 16);
}

//Render grid lines in separate buffer, push to preferred buffer with the device context parameter 
void rendergrid(HDC hdc) {
	int x, y;
	INT width, height;
	RECT dimension;
	Graphics maingraphics(memdc);
	Pen mainpen(Color(128, 128, 128), (REAL)0.2F);
	Pen negativepen(Color(255, 27, 155, 224), (REAL)(gridstep / 3));
	Pen positivepen(Color(255, 224, 27, 27), (REAL)(gridstep / 3));
	GetWindowRect(hWnd, &dimension);
	width = dimension.right - dimension.left;
	height = dimension.bottom - dimension.top - 62;

	for (x = 0; x < width; x += gridstep) {
		maingraphics.DrawLine(&mainpen, x, 0, x, height);
	}
	for (y = 0; y < height; y += gridstep) {
		maingraphics.DrawLine(&mainpen, 0, y, width, y);
	}
	maingraphics.DrawLine(&negativepen, 0, gridstep / 2, width, gridstep / 2);
	maingraphics.DrawLine(&positivepen, 0, (gridstep / 2) + gridstep, width, (gridstep / 2) + gridstep);
	maingraphics.DrawLine(&positivepen, 0, height, width, height);
	maingraphics.DrawLine(&negativepen, 0, height - gridstep, width, height - gridstep);
	pushbuffer(hdc);
	return;
}

//Processes single command in the form of an instruct structure pointer
void processcommand(struct instruct *cmd, unsigned int ptype) {
	//Depending on ptype, function will check for either string field or integer field for the component
	wchar_t tempstr[16];
	hdc = GetDC(hWnd);
	component comp;
	if (strcmp(cmd->command, "wire") == 0 || cmd->compmacro == WIRECOMP) {
		comp.setcoords(cmd->params[0], cmd->params[1], POSITIVE);
		comp.setcoords(cmd->params[2], cmd->params[3], NEGATIVE);
		comp.comptype = WIRECOMP;
		comp.rendercomponent();
		cmd->compmacro = WIRECOMP;
	}
	else {
		switch (ptype) {
		//In this case, file loading won't set the integer component field by itself,
		//meaning we must fall back onto the string field
		case PROCESS_FILETYPE:
			{
				cmd->compmacro = commandmap[cmd->command];
				comp = componentmap[cmd->compmacro];
				break;
			}
		//Objects from a list/stack are guaranteed to have a valid integer component field
		case PROCESS_STACKTYPE: 
			{
				comp = componentmap[cmd->compmacro];
				break;
			}
		}
		//Might make label value a constructor field later
		comp.setcompvalue(cmd->params[2], cmd->params[3]);
		wsprintf(tempstr, comp.unit, comp.compval);
		wcsncpy(comp.unit, tempstr, 16);
		comp.setcoords(cmd->params[0], cmd->params[1], POSITIVE);
		comp.setcoords(cmd->params[0] + gridstep * 3, cmd->params[1] + gridstep * 3, NEGATIVE);
		comp.rendercomponent();
	}
}

//Saves current board state to a file determined by fn
void saveboard(const char* fn) {
	fp = fopen(fn, "w+");
	if (fp == NULL)
		return;
	for (int i = 0; i < line.size(); i++) {
		fprintf(fp, "%s %d %d %d %d\n", line[i].command, line[i].params[0], 
			line[i].params[1], line[i].params[2], line[i].params[3]);
	}
	//Indicate end of the list
	fprintf(fp, "END 0 0 0 0");
	fclose(fp);
	return;
}

//Empty component/instruct structure stack or list
std::vector<struct instruct>* emptystack(std::vector<struct instruct>* stack) {
	int sz = stack->size();
	int index = 0;
	stack->clear();
	stack->shrink_to_fit();
	return stack;
}

//Renders a full board from a file name or from loaded component list (line).
//File loading / list loading determined by mode
int renderboard(const char* boardfile, int mode) {
	struct instruct temp;
	int index = 0, params[4];
	char* returnchar;
	char content[32];
	
	//Mode 0 indicates a file load, meaning the list must be loaded
	//first and then processed via processcommand()
	if (mode == 0) {
		if (strcmp(boardfile, "slotfile.txt") != 0)
			emptystack(&line);
		fp = fopen(boardfile, "r+");
		if (fp == NULL)
			return (1);
		while (true) {
			fscanf(fp, "%s %d %d %d %d\n", content, &params[0], &params[1], &params[2], &params[3]);
			returnchar = strstr(content, "END");
			if (returnchar)
				break;
			for (int i = 0; i < 4; i++)
				temp.params[i] = params[i];
			strcpy(temp.command, content);
			line.push_back(temp);
			index++;
		}
		for (int i = 0; i < index; i++)
			processcommand(&line[i], PROCESS_FILETYPE);
		fclose(fp);
	}
	//Mode 1 indicates a loaded stack, in this mode the program 
	//simply iterates and renders based on stored data in the list
	else
		for (int i = 0; i < line.size(); i++)
			processcommand(&line[i], PROCESS_STACKTYPE);
	pushbuffer(hdc);
	return (0);
}

//Push all stored drawing on dual memory buffer onto display
void pushbuffer(HDC hdc) {
	INT width, height;
	RECT dimension;
	GetWindowRect(hWnd, &dimension);
	width = dimension.right - dimension.left;
	height = dimension.bottom - dimension.top;
	BitBlt(hdc, 0, 0, width, height, memdc, 0, 0, SRCCOPY);
}

//Main full rendering function
VOID MainRender(HDC hdc) {
	int x, y;
	INT width, height;
	RECT dimension;
	Graphics maingraphics(memdc);

	//Set general graphics styles, preferential
	maingraphics.SetCompositingMode(CompositingModeSourceCopy);
	maingraphics.SetCompositingQuality(CompositingQualityHighSpeed);
	maingraphics.SetPixelOffsetMode(PixelOffsetModeNone);
	maingraphics.SetInterpolationMode(InterpolationModeDefault);

	GetWindowRect(hWnd, &dimension);
	width = dimension.right - dimension.left;
	height = dimension.bottom - dimension.top;
	maingraphics.Clear(Color(255, 255, 255));
	maingraphics.SetSmoothingMode(SmoothingModeNone);

	rendergrid(memdc);
	if (invokestate == false) {
		renderboard(boardfn, 0);
		invokestate = true;
	}
	else
		renderboard(boardfn, 1);
	fp = fopen("slotfile.txt", "r+");
	fgets(currentcommand, 16, fp);
	fclose(fp);

	pushbuffer(hdc);
	invokestate = true;
	maingraphics.Flush();

	return;
}