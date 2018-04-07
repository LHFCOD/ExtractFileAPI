#define INTERFACE_EXPORTS
#include "CSharp.h"
using namespace TileImage;

MDSFile *mds;
FileBlock *block;
int curLevel = -1;
int curx = -1;
int cury = -1;
void CreateMDSFile(char *path)
{
	mds = new MDSFile(path);
}
void ReleaseMDSFile()
{
	if (mds != nullptr)
	{
		delete mds;
		mds = nullptr;
	}
}
char* GetTileData(int Level, int x, int y)
{
	//try
	//{
	//	if (mds)
	//	{
	//		return (char*)mds->GetTileVirtualData(Level, x, y)->p;//�п������ڴ�й©
	//	}
	//	else
	//	{
	//		return nullptr;
	//	}
	//}
	//catch (std::exception ex)
	//{
	//	return nullptr;
	//}
	return (char*)mds->GetTileVirtualData(Level, x, y)->p;//�п������ڴ�й©
}
int GetTileSize(int Level, int x, int y)
{
	//try
	//{
	//	if (mds)
	//	{
	//		return mds->GetTileVirtualData(Level, x, y)->len;//�п������ڴ�й©
	//	}
	//	else
	//	{
	//		return 0;
	//	}
	//}
	//catch (std::exception ex)
	//{
	//	return 0;
	//}
	return mds->GetTileVirtualData(Level, x, y)->len;//�п������ڴ�й©

}
int TestDll(char* path)
{
	return 1;
}