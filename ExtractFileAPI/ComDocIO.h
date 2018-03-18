#pragma once
#include <QtCore/QtCore>
typedef unsigned char byte_t;
typedef unsigned short ui16_t;
typedef unsigned int ui32_t;
typedef int i32_t;
typedef unsigned long long ui64_t;

/*ȡ���� */
#define get_data(ptr, off)  (((const byte_t *)ptr) + off)
#define get_byte(ptr, off)  (*(const byte_t *)get_data(ptr, off))
#define get_ui16(ptr, off)  (*(const ui16_t *)get_data(ptr, off))
#define get_ui32(ptr, off)  (*(const ui32_t *)get_data(ptr, off))
#define get_ui64(ptr, off)  (*(const ui64_t *)get_data(ptr, off))

#define get_i32(ptr, off)  (*(const i32_t *)get_data(ptr, off))

#define MAKE_UI32(n) n##u
#define MAKE_UI64(n) n##ull

struct Header
{
	byte_t FileID[8];
	byte_t UID[16];
	ui16_t ReviNum;
	ui16_t VerNum;
	ui16_t ByteOrder;
	ui64_t SectorSize;
	ui16_t ShortSectSize;
	ui32_t SectNum;
	i32_t DirectSID;
	ui64_t MiniSize;
	i32_t ShortSectSID;
	ui32_t ShortSectNum;
	i32_t MSectSID;
	ui32_t MSectNum;
	i32_t MSectPart[109];
};
struct Directory
{
	byte_t EntryName[64];
	ui16_t NameSize;
	byte_t EntryType;
	byte_t NodeColor;
	i32_t LeftDID;
	i32_t RightDID;
	i32_t RootDID;
	i32_t SID;
	ui32_t StreamSize;
	i32_t DID;
};
class ComDocIO
{
public:
	ComDocIO();
	ComDocIO(QString FilePath);
	~ComDocIO();
public:
	Header m_Header;
	byte_t* dat;//�ļ��ڴ�����ָ��
	QFile file;//�ļ�����
	byte_t* ptr;//��ǰ�ļ�ָ��
    
	QVector<i32_t> ShortSreamCon;
	QVector<i32_t> ShortSectAllo;
	QVector<i32_t> SATAllo;
	void ReadHeader();
	void ReadFile(QString FilePath);
	Directory ReadDirectory(ui32_t DID);
	i32_t FindNextSID(i32_t SID);
	i32_t FindSID(i32_t SID,ui32_t Offset);
	void ConfigureShorStream();
	i32_t FindNextShortSID(i32_t SID);
	byte_t* GetAddressFromShortSID(i32_t SID);
	byte_t* ReadStreamFromSID(i32_t SID,ui64_t len);
	byte_t*  ReadShortStreamFromSID(i32_t SID, ui64_t len);

	QMap<ui32_t, QVector<Directory>*> FilePool;//����ļ���֯�ṹ
	i32_t FileCount = 0;//����ļ�����
	void CreateFilePool(ui32_t RootDID, QVector<Directory> *vec);
	void SerchTree(ui32_t RootDID, QVector<Directory> *vec);//���Һ����
	void ConfigureSAT();
	void SerchSAT(i32_t SID);
};

