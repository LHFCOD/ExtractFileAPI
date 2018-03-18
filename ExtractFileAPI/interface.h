#pragma once
#include <objbase.h>
#include <QtCore/QtCore>
#include <QtGui\QImage>
#include <QtGui\QPainter>
//#include "ComDocIO.h"
#include <malloc.h>

#ifdef INTERFACE_EXPORTS
#define INTERFACE _declspec(dllexport)
#else
#define INTERFACE _declspec(dllimport)
#endif
typedef unsigned char byte_t;
typedef unsigned short ui16_t;
typedef unsigned int ui32_t;
typedef int i32_t;
typedef unsigned long long ui64_t;
class FileBlock
{
public:
	byte_t *p=nullptr;
	int len=0;
	FileBlock(byte_t *_p, int _len);
	FileBlock() {};
	~FileBlock();
};
class ImageInfo
{
public:
	int LayerCount = 0;
	int ImageWidth = 0;
	int ImageHeight = 0;
	int TileWidth = 0;
	int TileHeight = 0;
	int maxLevel = 0;
	int minLevel = 0;
};
class LayerProperty
{
public:
	QString folderName;
	float scaleVal;
	int rowCount;
	int colCount;
	int lastImageWidth;
	int lastImageHeight;
	int curImageWidth;
	int curImageHeight;
};
 class MDSFile
{
public:
	IStorage *rootSt = nullptr;
	// ComDocIO *io;
	 ImageInfo info;
	 QMap<int, LayerProperty> LayerMap;
	 QImage *minLevelImage;
public:
	MDSFile(QString _RootDir);
	FileBlock* ReadFromPath(QString path);
	void GetImageInfo();
	void GetLayerProperty();
	FileBlock* GetTileData(int Level, int x, int y);
	FileBlock* GetTileVirtualData(int Level, int x, int y);
	void GetMinLevelImage();
	~MDSFile();
};