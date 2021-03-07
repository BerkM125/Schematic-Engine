#include <Windows.h>
#include <stdio.h>
#include "schem.h"

char currentcommand[32];
int getlen(const char* str) {
	int i = 0;
	while (str[i] != '\0') {
		i++;
	}
	return i;
}

void checkfile(void) {
	FILE* fp;
	char cmd[32];
	static int state = 0;
	RECT* windowrect{};
	GetWindowRect(hWnd, windowrect);
	fp = fopen("slotfile.txt", "r+");
	fgets(cmd, 16, fp);
	if (strcmp(currentcommand, cmd) != 0 && getlen(cmd) >= 12) {
		strcpy(currentcommand, cmd);
		renderboard("slotfile.txt", 0);
		RedrawWindow(hWnd, windowrect, NULL, RDW_ERASE | RDW_INVALIDATE);
		state++;
	}
	fclose(fp);
}