#pragma once

#include "interface.h"

extern "C" INTERFACE void CreateMDSFile(char *path);
extern "C" INTERFACE void ReleaseMDSFile();
extern "C" INTERFACE char* GetTileData(int Level, int x, int y);
extern "C" INTERFACE int GetTileSize(int Level, int x, int y);
extern "C" INTERFACE int TestDll(char* path);