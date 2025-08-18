#include "Editor.h"

#ifdef _MSC_VER
#ifndef _DEBUG  
#pragma comment(linker, "/SUBSYSTEM:windows")
#pragma comment(linker, "/ENTRY:mainCRTStartup")
#endif
#endif

int main() {
	Editor app;
	app.run();
	return 0;
}