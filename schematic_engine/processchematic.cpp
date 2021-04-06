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
Image* resistorhz;
Image* dcmotorhz;
Image* capacitorhz;
Image* ledhz;
Image* switchhz;
Image* inductorhz;
Image* megahz;
Image* ultrasonichz;
Image* infraredhz;
Image* nrfhz;
Image* servo;
Image* micro; 
Image* buzzer;
Image* nano;
std::vector<struct instruct> line;
bool invokestate = false;

class component {
	public:
		int comptype;
		int compval;
		int orientation;
		wchar_t unit[16];
		Image* compimg;
		void rendercomponent(HDC hdc);
		void setcoords(int x, int y, int pole);
		void setcompvalue(int ori, int val);
		void setrenderingcoords(float x1, float y1, float x2, float y2);
		component(int ctype = EMPTYCOMP, int posxcoord = 0, int posycoord = 0, int negxcoord = 0, int negycoord = 0);
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

void configurepensettings(Graphics *graph, Pen *mainpen, Pen *negativepen, Pen *positivepen) {
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

void component::rendercomponent(HDC hdc) {
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
	}
	configurationstate += 1;
	//Calculate coordinates according to circuit pin grid
	int px, py, nx, ny;
	px = (positivexcoord * gridstep) + (gridstep/2);
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
				graph.DrawString(positivestr, -1, &font, PointF(px + (nx - px), ((ny - py) / 2)-gridstep), &solidBrush);
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
				compimg->RotateFlip(Rotate90FlipXY);
				graph.DrawImage(compimg, px - pydiff1, py - pxdiff1, gridstep * pydiff2, gridstep * pxdiff2);
				graph.DrawString(unit, -1, &font, PointF(px + gridstep * 1.2, py + gridstep * 1.7), &solidBrush);
			}
			break;
	}
	pushbuffer(hdc);
	return;
}

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

void component::setcompvalue(int ori, int val) {
	orientation = ori;
	compval = val;
}

void component::setrenderingcoords(float x1, float y1, float x2, float y2) {
	pxdiff1 = x1;
	pxdiff2 = x2;
	pydiff1 = y1;
	pydiff2 = y2;
}

component::component(int ctype, int posxcoord, int posycoord, int negxcoord, int negycoord) {
	comptype = ctype;
	positivexcoord = posxcoord;
	positiveycoord = posycoord;
	negativexcoord = negxcoord;
	negativeycoord = negycoord;
}

void rendergrid(HDC hdc) {
	int x, y;
	INT width, height;
	RECT dimension;
	Graphics maingraphics(memdc);
	Pen mainpen(Color(128, 128, 128), (REAL)0.2F);
	Pen negativepen(Color(255, 27, 155, 224), (REAL)(gridstep/3));
	Pen positivepen(Color(255, 224, 27, 27), (REAL)(gridstep/3));
	GetWindowRect(hWnd, &dimension);
	width = dimension.right - dimension.left;
	height = dimension.bottom - dimension.top - 62;
	
	for (x = 0; x < width; x+=gridstep) {
		maingraphics.DrawLine(&mainpen, x, 0, x, height);
	}
	for (y = 0; y < height; y+=gridstep) {
		maingraphics.DrawLine(&mainpen, 0, y, width, y);
	}
	maingraphics.DrawLine(&negativepen, 0, gridstep/2, width, gridstep/2);
	maingraphics.DrawLine(&positivepen, 0, (gridstep/2)+gridstep, width, (gridstep / 2) +gridstep);
	maingraphics.DrawLine(&positivepen, 0, height, width, height);
	maingraphics.DrawLine(&negativepen, 0, height - gridstep, width, height - gridstep);
	pushbuffer(hdc);
	return;
}

void processcommand(struct instruct cmd) {
	hdc = GetDC(hWnd);
	component comp;
	if (strcmp(cmd.command, "wire") == 0) {
		comp.setcoords(cmd.params[0], cmd.params[1], POSITIVE);
		comp.setcoords(cmd.params[2], cmd.params[3], NEGATIVE);
		comp.comptype = WIRECOMP;
		comp.rendercomponent(hdc);
	}
	else {
		comp.setcoords(cmd.params[0], cmd.params[1], POSITIVE);
		comp.setcoords(cmd.params[0] + gridstep * 3, cmd.params[1] + gridstep * 3, NEGATIVE);
		comp.setcompvalue(cmd.params[2], cmd.params[3]); 
		{
			if (strcmp(cmd.command, "dcmotor") == 0) {
				comp.comptype = DCMOTOR;
				comp.setrenderingcoords((gridstep / 2), (gridstep * 1.5), 5, 4);
				comp.compimg = dcmotorhz;
				wsprintf(comp.unit, L"%dτ", comp.compval);
			}
			if (strcmp(cmd.command, "capacitor") == 0) {
				comp.comptype = CAPACITOR;
				comp.setrenderingcoords((gridstep / 8), gridstep, 4, 2);
				comp.compimg = capacitorhz;
				wsprintf(comp.unit, L"%dμ", comp.compval);
			}
			if (strcmp(cmd.command, "resistor") == 0) {
				comp.comptype = RESISTOR;
				comp.setrenderingcoords(0, (gridstep / 2), 4, 1);
				comp.compimg = resistorhz;
				wsprintf(comp.unit, L"%dΩ", comp.compval);
			}
			if (strcmp(cmd.command, "switch") == 0) {
				comp.comptype = SWITCHCOMP;
				comp.setrenderingcoords((gridstep / 8), gridstep, 5.5, 1.7);
				comp.compimg = switchhz;
				wsprintf(comp.unit, L"0 | 1");
			}
			if (strcmp(cmd.command, "led") == 0) {
				comp.comptype = LEDLIGHT;
				comp.setrenderingcoords((gridstep / 8), (gridstep * 2.1), 4, 3);
				comp.compimg = ledhz;
				wsprintf(comp.unit, L"%dLum", comp.compval);
			}
			if (strcmp(cmd.command, "inductor") == 0) {
				comp.comptype = INDUCTOR;
				comp.setrenderingcoords((gridstep / 8), gridstep, 5.5, 2.1);
				comp.compimg = inductorhz;
				wsprintf(comp.unit, L"EMF");
			}
			if (strcmp(cmd.command, "mega") == 0) {
				comp.comptype = MEGA;
				comp.setrenderingcoords((gridstep / 8), 0, 40, 20);
				comp.compimg = megahz;
				wsprintf(comp.unit, L"ATmega2560");
			}
			if (strcmp(cmd.command, "ultrasonic") == 0) {
				comp.comptype = ULTRASONIC;
				comp.setrenderingcoords((gridstep / 2), (gridstep / 2), 16, 11);
				comp.compimg = ultrasonichz;
				wsprintf(comp.unit, L"%dkHz", comp.compval);
			}
			if (strcmp(cmd.command, "infrared") == 0) {
				comp.comptype = INFRARED;
				comp.setrenderingcoords((gridstep / 2), (gridstep / 2), 5, 8);
				comp.compimg = infraredhz;
				wsprintf(comp.unit, L"%dkHz", comp.compval);
			}
			if (strcmp(cmd.command, "nrf") == 0) {
				comp.comptype = NRF;
				comp.setrenderingcoords((gridstep / 2), (gridstep / 2), 6, 11);
				comp.compimg = nrfhz;
				wsprintf(comp.unit, L"%dGHz", comp.compval);
			}
			if (strcmp(cmd.command, "servo") == 0) {
				comp.comptype = SERVO;
				comp.setrenderingcoords((gridstep / 2), (gridstep / 2), 10.8, 9.6);
				comp.compimg = servo;
				wsprintf(comp.unit, L"%dkg/cm", comp.compval);
			}
			if (strcmp(cmd.command, "micro") == 0) {
				comp.comptype = MICRO;
				comp.setrenderingcoords((gridstep / 2), (gridstep / 2), 20, 7.2);
				comp.compimg = micro;
				wsprintf(comp.unit, L"32u4");
			}
			if (strcmp(cmd.command, "buzzer") == 0) {
				comp.comptype = BUZZER;
				comp.setrenderingcoords((gridstep / 2), (gridstep / 2), 7, 4.8);
				comp.compimg = buzzer;
				wsprintf(comp.unit, L"%ddB", comp.compval);
			}
			if (strcmp(cmd.command, "nano") == 0) {
				comp.comptype = NANO;
				comp.setrenderingcoords((gridstep / 2)+(gridstep/4), (gridstep / 2), 17.2, 7.2);
				comp.compimg = nano;
				wsprintf(comp.unit, L"Nano");
			}
		}
		comp.rendercomponent(hdc);
	}
}

void saveboard(const char* fn) {
	fp = fopen(fn, "w+");
	if (fp == NULL)
		return;
	for (int i = 0; i < line.size(); i++) {
		fprintf(fp, "%s %d %d %d %d\n", line[i].command, line[i].params[0], line[i].params[1], line[i].params[2], line[i].params[3]);
	}
	fprintf(fp, "END 0 0 0 0");
	fclose(fp);
	return;
}

std::vector<struct instruct> *emptystack(std::vector<struct instruct> *stack) {
	int sz = stack->size();
	int index = 0;
	stack->clear();
	stack->shrink_to_fit();
	return stack;
}

int renderboard(const char* boardfile, int mode) {
	struct instruct temp; 
	int index = 0, params[4];
	char* returnchar;
	char content[32];
	if (mode == 0) {
		if(strcmp(boardfile, "slotfile.txt") != 0)
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
			processcommand(line[i]);
		fclose(fp);
	}
	else
		for (int i = 0; i < line.size(); i++)
			processcommand(line[i]);
	return (0);
}

void pushbuffer(HDC hdc) {
	INT width, height;
	RECT dimension;
	GetWindowRect(hWnd, &dimension);
	width = dimension.right - dimension.left;
	height = dimension.bottom - dimension.top;
	BitBlt(hdc, 0, 0, width, height, memdc, 0, 0, SRCCOPY);
}

VOID MainRender(HDC hdc) {
	int x, y;
	INT width, height;
	RECT dimension;
	Graphics maingraphics(memdc);

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
	if (invokestate == false)
		renderboard(boardfn, 0);
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