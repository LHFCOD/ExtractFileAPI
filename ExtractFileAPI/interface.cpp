
#include "interface.h"
#include <exception>
#include <QtXml\QtXml>
#include <QtXml\QDomDocument>

#include <cmath>
#include <stdio.h>

using namespace TileImage;

TileImage::MDSFile::MDSFile(QString RootDir)
{
	io = new ComDocIO(RootDir);
	GetImageInfo();
	GetLayerProperty();
	GetMinLevelImage();
}


void MDSFile::GetImageInfo()
{
	FileBlock *content = io->ReadFromPath(u8"\\DSI0\\MoticDigitalSlideImage");
	QByteArray xmlArray = QByteArray::fromRawData((char*)content->p, content->len);
	QDomDocument doc;
	if (!doc.setContent(xmlArray))
	{
		throw std::exception("read xml file failed!");
	}
	QDomElement root = doc.documentElement();//读取根节点 
	QDomElement ImageMatrix = root.elementsByTagName("ImageMatrix").at(0).toElement();
	QDomElement LayerCount = ImageMatrix.elementsByTagName("LayerCount").at(0).toElement();
	QDomElement ImageWidth = ImageMatrix.elementsByTagName("Width").at(0).toElement();
	QDomElement ImageHeight = ImageMatrix.elementsByTagName("Height").at(0).toElement();
	QDomElement TileWidth = ImageMatrix.elementsByTagName("CellWidth").at(0).toElement();
	QDomElement TileHeight = ImageMatrix.elementsByTagName("CellHeight").at(0).toElement();
	info.LayerCount = LayerCount.attribute("value").toInt() - 1;//获取层数，最低层排除
	info.ImageWidth = ImageWidth.attribute("value").toInt();//获取层数
	info.ImageHeight = ImageHeight.attribute("value").toInt();//获取层数
	info.TileWidth = TileWidth.attribute("value").toInt();//获取层数
	info.TileHeight = TileHeight.attribute("value").toInt();//获取层数
	//计算最大level
	int widthLevel = std::ceil(std::log(info.ImageWidth) / std::log(2));
	int heightLevel = std::ceil(std::log(info.ImageHeight) / std::log(2));
	if (widthLevel > heightLevel)
	{
		info.maxLevel = widthLevel;
	}
	else
	{
		info.maxLevel = heightLevel;
	}
	//计算最小level
	info.minLevel = info.maxLevel - info.LayerCount + 1;

	if (content != nullptr)
	{
		delete content;
		content = nullptr;
	}

}
void MDSFile::GetLayerProperty()
{
	FileBlock *content = io->ReadFromPath(u8"\\DSI0\\MoticDigitalSlideImage");
	QByteArray xmlArray = QByteArray::fromRawData((char*)content->p, content->len);
	QDomDocument doc;
	if (!doc.setContent(xmlArray))
	{
		throw std::exception("read xml file failed!");
	}
	QDomElement root = doc.documentElement();//读取根节点 
	QDomElement ImageMatrix = root.elementsByTagName("ImageMatrix").at(0).toElement();
	//文件夹和层数的对应关系
	for (int i = info.minLevel; i <= info.maxLevel; i++)
	{
		LayerProperty prop;
		int layer = info.maxLevel - i;
		QDomElement LayerElement = ImageMatrix.elementsByTagName(QString("Layer") + QString::number(layer)).at(0).toElement();
		QDomElement ScaleElement = LayerElement.elementsByTagName("Scale").at(0).toElement();
		QDomElement RowsElement = LayerElement.elementsByTagName("Rows").at(0).toElement();
		QDomElement ColseElement = LayerElement.elementsByTagName("Cols").at(0).toElement();

		prop.folderName = ScaleElement.attribute("value");
		prop.scaleVal = prop.folderName.toFloat();
		prop.rowCount = RowsElement.attribute("value").toInt();
		prop.colCount = ColseElement.attribute("value").toInt();
		//计算最后一行或者最后一列
		prop.curImageWidth = info.ImageWidth / std::pow(2, info.maxLevel - i);
		prop.curImageHeight = info.ImageHeight / std::pow(2, info.maxLevel - i);
		prop.lastImageWidth = prop.curImageWidth - info.TileWidth*(prop.colCount - 1);
		prop.lastImageHeight = prop.curImageHeight - info.TileHeight*(prop.rowCount - 1);
		LayerMap.insert(i, prop);
	}
	//虚拟层数填充
	for (int i = 0; i < info.minLevel; i++)
	{
		int curLevel = info.minLevel - i - 1;
		LayerProperty prop;
		prop.scaleVal = LayerMap[curLevel + 1].scaleVal / 2;
		prop.rowCount = std::ceil(LayerMap[curLevel + 1].rowCount / 2.0);
		prop.colCount = std::ceil(LayerMap[curLevel + 1].colCount / 2.0);
		//计算最后一行或者最后一列
		prop.curImageWidth = info.ImageWidth / std::pow(2, info.maxLevel - curLevel);
		prop.curImageHeight = info.ImageHeight / std::pow(2, info.maxLevel - curLevel);
		prop.lastImageWidth = prop.curImageWidth - info.TileWidth*(prop.colCount - 1);
		prop.lastImageHeight = prop.curImageHeight - info.TileHeight*(prop.rowCount - 1);

		LayerMap.insert(curLevel, prop);
	}
	if (content != nullptr)
	{
		delete content;
		content = nullptr;
	}
}
FileBlock* MDSFile::GetTileData(int Level, int x, int y)
{
	QString folderName = LayerMap[Level].folderName;
	char FileName[100];
	sprintf(FileName, "%04d_%04d", y, x);
	QString Path("\\DSI0\\");

	Path = Path + folderName + "\\" + QString(FileName);
	FileBlock *imge = io->ReadFromPath(Path);
	return imge;
}
FileBlock* MDSFile::GetTileVirtualData(int Level, int x, int y)
{
	if (Level > info.maxLevel || Level < 0)
	{
		throw std::exception("image not exist!");
	}
	if (Level >= info.minLevel)
	{
		int vWidth = info.TileWidth;
		int vHeight = info.TileHeight;
		if (x < LayerMap[Level].colCount - 1 && y < LayerMap[Level].rowCount - 1)
		{
			//正常返回
			return GetTileData(Level, x, y);
		}
		//最后一列
		else if (x == LayerMap[Level].colCount - 1 && y != LayerMap[Level].rowCount - 1)
		{
			vWidth = LayerMap[Level].lastImageWidth;
		}
		//最后一行
		else if (x != LayerMap[Level].colCount - 1 && y == LayerMap[Level].rowCount - 1)
		{
			vHeight = LayerMap[Level].lastImageHeight;
		}
		//最后一个
		else if (x == LayerMap[Level].colCount - 1 && y == LayerMap[Level].rowCount - 1)
		{
			vWidth = LayerMap[Level].lastImageWidth;
			vHeight = LayerMap[Level].lastImageHeight;
		}
		//剪裁处理
		FileBlock *temp_block = GetTileData(Level, x, y);
		QImage image;
		image.loadFromData(temp_block->p, temp_block->len);
		QImage lastImage = image.copy(0, 0,vWidth , vHeight);
		
		QByteArray ba;
		QBuffer buffer(&ba);
		buffer.open(QIODevice::WriteOnly);
		lastImage.save(&buffer, "JPEG"); // writes image into ba in PNG format
		byte_t *p = (byte_t*)malloc(ba.count());
		memcpy(p, ba.data(), ba.count());
		delete temp_block;
		return new FileBlock(p, ba.count());
	}
	else
	{
		int vWidth = LayerMap[Level].lastImageWidth;
		int vHeight = LayerMap[Level].lastImageHeight;
		if (x < LayerMap[Level].colCount - 1 && y < LayerMap[Level].rowCount - 1)
		{
			vWidth = info.TileWidth;
			vHeight = info.TileHeight;
			//不做处理
		}
		//最后一列
		else if (x == LayerMap[Level].colCount - 1 && y != LayerMap[Level].rowCount - 1)
		{
			vHeight = info.TileHeight;
		}
		//最后一行
		else if (x != LayerMap[Level].colCount - 1 && y == LayerMap[Level].rowCount - 1)
		{
			vWidth = info.TileWidth;
		}
		//最后一个
		else if (x == LayerMap[Level].colCount - 1 && y == LayerMap[Level].rowCount - 1)
		{
			vWidth = LayerMap[Level].lastImageWidth;
			vHeight = LayerMap[Level].lastImageHeight;
		}
		//剪裁处理
		int scaleWidth = LayerMap[info.minLevel].colCount*info.TileWidth / std::pow(2, info.minLevel - Level);
		int scaleHeight = LayerMap[info.minLevel].rowCount*info.TileHeight / std::pow(2, info.minLevel - Level);
		QImage scaleImage = minLevelImage->scaled(scaleWidth, scaleHeight);
		QImage lastImage = scaleImage.copy(x*info.TileWidth,y*info.TileHeight, vWidth, vHeight);
		QByteArray ba;
		QBuffer buffer(&ba);
		buffer.open(QIODevice::WriteOnly);
		lastImage.save(&buffer, "JPEG"); // writes image into ba in PNG format
		byte_t *p = (byte_t*)malloc(ba.count());
		memcpy(p, ba.data(), ba.count());
		return new FileBlock(p, ba.count());
	}
}
void MDSFile::GetMinLevelImage()
{
	minLevelImage = new QImage(LayerMap[info.minLevel].colCount*info.TileWidth, LayerMap[info.minLevel].rowCount*info.TileHeight, QImage::Format_RGB32);
	QPainter painter(minLevelImage);
	for(int i=0;i<LayerMap[info.minLevel].colCount;i++)
		for (int j = 0; j < LayerMap[info.minLevel].rowCount; j++)
		{
			FileBlock *temp_block = GetTileData(info.minLevel, i, j);
			QImage image;
			image.loadFromData(temp_block->p, temp_block->len);
			painter.drawImage(i*info.TileWidth, j*info.TileHeight, image);
		}
	
}
MDSFile::~MDSFile()
{
	if (io != nullptr)
	{
		delete io;
		io = nullptr;
	}
	
	if (minLevelImage != nullptr)
	{
		delete minLevelImage;
		minLevelImage = nullptr;
	}
}
