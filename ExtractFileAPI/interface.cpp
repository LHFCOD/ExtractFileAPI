
#include "interface.h"
#include <exception>
#include <QtXml\QtXml>
#include <QtXml\QDomDocument>

#include <cmath>
#include <stdio.h>

FileBlock::FileBlock(byte_t *_p, int _len)
{
	p = _p;
	len = _len;
}
FileBlock::~FileBlock()
{
	if (p != nullptr)
	{
		free(p);
		p = nullptr;
	}
}
MDSFile::MDSFile(QString RootDir)
{
	//io = new ComDocIO(RootDir);
	::OleInitialize(NULL);
	wchar_t wpath[100] = { 0 };
	RootDir.toWCharArray(wpath);
	HRESULT hr = StgOpenStorage(wpath, NULL, STGM_READ |STGM_SHARE_DENY_WRITE, NULL, 0, &rootSt);
	if (FAILED(hr))
	{
		throw std::exception("Read MDS file error!");
	}

	GetImageInfo();
	GetLayerProperty();
	GetMinLevelImage();
	
}
//FileBlock* MDSFile::ReadFromPath(QString path)
//{
//	QStringList dirList = path.split("\\");
//	//��·���Ϸ��Խ��м��
//	for (int i = 0; i < dirList.count(); i++)
//	{
//		if (i == 0)
//		{
//			if (!dirList[i].isEmpty())
//				throw std::exception("path has error!");
//		}
//		else
//		{
//			if (dirList[i].isEmpty())
//				throw std::exception("path has error!");
//		}
//	}
//	i32_t currentDID = 0;
//	for (int i = 1; i < dirList.count(); i++)
//	{
//		bool isExist = false;
//		if (i < dirList.count() - 1)
//		{
//			QVector<Directory> *vec = io->FilePool[currentDID];
//			for each(Directory direct in *vec)
//			{
//				QString name = QString::fromUtf16((char16_t*)(direct.EntryName));
//				if (dirList[i] == name)
//				{
//
//					//�����Ӧ��ΪĿ¼
//					if (direct.EntryType == 1)
//					{
//						currentDID = direct.DID;
//						isExist = true;
//					}
//					else
//					{
//						throw std::exception("path has error!");
//
//					}
//
//				}
//			}
//			if (isExist == false)
//			{
//				throw std::exception("path has error!");
//			}
//		}
//		else
//		{
//			QVector<Directory> *vec = io->FilePool[currentDID];
//			for each(Directory direct in *vec)
//			{
//				QString name = QString::fromUtf16((char16_t*)(direct.EntryName));
//				if (dirList[i] == name)
//				{
//					isExist = true;
//					//
//					if (direct.EntryType == 1)
//					{
//						throw std::exception("path has error!");
//					}
//					else
//					{
//						isExist = true;
//						byte_t *p;
//						if (direct.StreamSize < io->m_Header.MiniSize)
//							p = io->ReadShortStreamFromSID(direct.SID, direct.StreamSize);
//						else
//							p = io->ReadStreamFromSID(direct.SID, direct.StreamSize);//һ���԰�����Ҫд���ַ�����ȡ��Ȼ��д���ļ�
//						FileBlock *content = new FileBlock(p, direct.StreamSize);
//						return content;
//					}
//				}
//			}
//			if (isExist == false)
//			{
//				throw std::exception("path has error!");
//			}
//		}
//	}
//}
FileBlock* MDSFile::ReadFromPath(QString path)
{
	QStringList dirList = path.split("\\");
	//��·���Ϸ��Խ��м��
	for (int i = 0; i < dirList.count(); i++)
	{
		if (i == 0)
		{
			if (!dirList[i].isEmpty())
				throw std::exception("path has error!");
		}
		else
		{
			if (dirList[i].isEmpty())
				throw std::exception("path has error!");
		}
	}
	IStorage *parentSt = rootSt;
	std::vector<IStorage*> StVec;
	for (int i = 1; i < dirList.count() - 1; i++)
	{
		IStorage *childSt;
		//��Ŀ¼����ת��Ϊ���ַ�
		wchar_t wpath[100] = { 0 };
		dirList[i].toWCharArray(wpath);
		HRESULT hr = parentSt->OpenStorage(wpath, NULL, STGM_READ |STGM_SHARE_EXCLUSIVE, NULL, 0, &childSt);
		if (FAILED(hr))
		{
			throw std::exception("Read MDS file error!");
		}
		parentSt = childSt;
		//���Storage����ĸ¼����
		StVec.insert(StVec.end(),parentSt);
	}
	wchar_t wpath[100] = { 0 };
	dirList.last().toWCharArray(wpath);
	IStream* pStream;
	HRESULT hr = parentSt->OpenStream(wpath, 0, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStream);
	if (FAILED(hr))
	{
		throw std::exception("Read MDS file error!");
	}
	//��ȡ����Ϣ
	STATSTG stg;
	pStream->Stat(&stg, NULL);
	char* content = (char*)malloc(stg.cbSize.QuadPart);
	unsigned long readEd = 0;
	pStream->Read(content, stg.cbSize.QuadPart, &readEd);
	//�����ͷŵ�
	pStream->Release();
	//�Ѹ�Ŀ¼�ͷŵ�
	for each(IStorage *st in StVec)
	{
		if(st!=nullptr)
		st->Release();
	}
	return new FileBlock((byte_t*)content, stg.cbSize.QuadPart);
}
void MDSFile::GetImageInfo()
{
	FileBlock *content = ReadFromPath(u8"\\DSI0\\MoticDigitalSlideImage");
	QByteArray xmlArray = QByteArray::fromRawData((char*)content->p, content->len);
	QDomDocument doc;
	if (!doc.setContent(xmlArray))
	{
		throw std::exception("read xml file failed!");
	}
	QDomElement root = doc.documentElement();//��ȡ���ڵ� 
	QDomElement ImageMatrix = root.elementsByTagName("ImageMatrix").at(0).toElement();
	QDomElement LayerCount = ImageMatrix.elementsByTagName("LayerCount").at(0).toElement();
	QDomElement ImageWidth = ImageMatrix.elementsByTagName("Width").at(0).toElement();
	QDomElement ImageHeight = ImageMatrix.elementsByTagName("Height").at(0).toElement();
	QDomElement TileWidth = ImageMatrix.elementsByTagName("CellWidth").at(0).toElement();
	QDomElement TileHeight = ImageMatrix.elementsByTagName("CellHeight").at(0).toElement();
	info.LayerCount = LayerCount.attribute("value").toInt() - 1;//��ȡ��������Ͳ��ų�
	info.ImageWidth = ImageWidth.attribute("value").toInt();//��ȡ����
	info.ImageHeight = ImageHeight.attribute("value").toInt();//��ȡ����
	info.TileWidth = TileWidth.attribute("value").toInt();//��ȡ����
	info.TileHeight = TileHeight.attribute("value").toInt();//��ȡ����
	//�������level
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
	//������Сlevel
	info.minLevel = info.maxLevel - info.LayerCount + 1;

	if (content != nullptr)
	{
		delete content;
		content = nullptr;
	}

}
void MDSFile::GetLayerProperty()
{
	FileBlock *content = ReadFromPath(u8"\\DSI0\\MoticDigitalSlideImage");
	QByteArray xmlArray = QByteArray::fromRawData((char*)content->p, content->len);
	QDomDocument doc;
	if (!doc.setContent(xmlArray))
	{
		throw std::exception("read xml file failed!");
	}
	QDomElement root = doc.documentElement();//��ȡ���ڵ� 
	QDomElement ImageMatrix = root.elementsByTagName("ImageMatrix").at(0).toElement();
	//�ļ��кͲ����Ķ�Ӧ��ϵ
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
		//�������һ�л������һ��
		prop.curImageWidth = info.ImageWidth / std::pow(2, info.maxLevel - i);
		prop.curImageHeight = info.ImageHeight / std::pow(2, info.maxLevel - i);
		prop.lastImageWidth = prop.curImageWidth - info.TileWidth*(prop.colCount - 1);
		prop.lastImageHeight = prop.curImageHeight - info.TileHeight*(prop.rowCount - 1);
		LayerMap.insert(i, prop);
	}
	//����������
	for (int i = 0; i < info.minLevel; i++)
	{
		int curLevel = info.minLevel - i - 1;
		LayerProperty prop;
		prop.scaleVal = LayerMap[curLevel + 1].scaleVal / 2;
		prop.rowCount = std::ceil(LayerMap[curLevel + 1].rowCount / 2.0);
		prop.colCount = std::ceil(LayerMap[curLevel + 1].colCount / 2.0);
		//�������һ�л������һ��
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
	FileBlock *imge = ReadFromPath(Path);
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
			//��������
			return GetTileData(Level, x, y);
		}
		//���һ��
		else if (x == LayerMap[Level].colCount - 1 && y != LayerMap[Level].rowCount - 1)
		{
			vWidth = LayerMap[Level].lastImageWidth;
		}
		//���һ��
		else if (x != LayerMap[Level].colCount - 1 && y == LayerMap[Level].rowCount - 1)
		{
			vHeight = LayerMap[Level].lastImageHeight;
		}
		//���һ��
		else if (x == LayerMap[Level].colCount - 1 && y == LayerMap[Level].rowCount - 1)
		{
			vWidth = LayerMap[Level].lastImageWidth;
			vHeight = LayerMap[Level].lastImageHeight;
		}
		//���ô���
		FileBlock *temp_block = GetTileData(Level, x, y);
		QImage image;
		image.loadFromData(temp_block->p, temp_block->len);
		QImage lastImage = image.copy(0, 0,vWidth , vHeight);
		delete temp_block;
		QByteArray ba;
		QBuffer buffer(&ba);
		buffer.open(QIODevice::WriteOnly);
		lastImage.save(&buffer, "JPEG"); // writes image into ba in PNG format
		byte_t *p = (byte_t*)malloc(ba.count());
		memcpy(p, ba.data(), ba.count());
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
			//��������
		}
		//���һ��
		else if (x == LayerMap[Level].colCount - 1 && y != LayerMap[Level].rowCount - 1)
		{
			vHeight = info.TileHeight;
		}
		//���һ��
		else if (x != LayerMap[Level].colCount - 1 && y == LayerMap[Level].rowCount - 1)
		{
			vWidth = info.TileWidth;
		}
		//���һ��
		else if (x == LayerMap[Level].colCount - 1 && y == LayerMap[Level].rowCount - 1)
		{
			vWidth = LayerMap[Level].lastImageWidth;
			vHeight = LayerMap[Level].lastImageHeight;
		}
		//���ô���
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
	/*if (io != nullptr)
	{
		delete io;
		io = nullptr;
	}*/
	if (rootSt != nullptr)
	{
		rootSt->Release();
		rootSt = nullptr;
	}
	::OleUninitialize();
	if (minLevelImage != nullptr)
	{
		delete minLevelImage;
		minLevelImage = nullptr;
	}
}
