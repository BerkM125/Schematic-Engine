#include <stdio.h>
#include <stdlib.h>
//WORK IN PROGRESS; Undo/Redo Functionality not yet integrated
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <iostream>
#include <vector>
#include <stack>
#include <thread>
#include "framework.h"
#include "schematic_engine.h"
#include "schem.h"

std::stack<std::vector<struct instruct>> undostk;
std::stack<std::vector<struct instruct>> redostk;

void pushdata(std::vector<struct instruct> data) {
	undostk.push(data);
}

void undoaction(void) {
	std::vector<struct instruct> undotop = undostk.top();
	undostk.pop();
	redostk.push(undotop);
	undotop = undostk.top();
}

void redoaction(void) {
	std::vector<struct instruct> redotop = redostk.top();
	redostk.pop();
	line = redostk.top();
	undostk.push(redotop);
}