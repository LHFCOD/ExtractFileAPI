#include "ComDocIO.h"
using namespace ComDoc;


ComDocIO::ComDocIO()
{
}

ComDocIO::ComDocIO(QString FilePath)
{
	ReadFile(FilePath);
	ReadHeader();
	ConfigureSAT();
	ConfigureShorStream();//配置短流信息

	//Directory direct;
	//for (int i = 0; i < 1000; i++)
	//{
	//	direct = ReadDirectory(i);
	//	if (direct.EntryType == 2 && direct.StreamSize < m_Header.MiniSize)
	//		break;
	//}
	//
	//byte_t *p;
	//if (direct.EntryType == 2)
	//{
	//	if (direct.StreamSize < m_Header.MiniSize)
	//		p = ReadShortStreamFromSID(direct.SID, direct.StreamSize);
	//	else
	//		p = ReadStreamFromSID(direct.SID, direct.StreamSize);//一次性把所需要写的字符流读取，然后写入文件
	//}
	//else
	//{
	//	//不为流
	//	return;
	//}

	//
	//QString str = QString::fromUtf16((ushort*)(direct.EntryName), direct.NameSize / 2);//文件名字
	//QByteArray arr = str.toLocal8Bit();//这个地方不太明白，为啥要分开去写比较合适
	//char *tmp = arr.data();
	//

	//QVector<Directory> *vec = new QVector<Directory>();
	//CreateFilePool(0, vec);
}
FileBlock * ComDocIO::ReadFromPath(QString path)
{
	QStringList dirList = path.split("\\");
	//对路径合法性进行检查
	for (int i = 0; i < dirList.count(); i++)
	{
		if (i == 0)
		{
			if (!dirList[i].isEmpty())
			{
				std::logic_error excep("path has error!");
				throw std::exception(excep);
			}
		}
		else
		{
			if (dirList[i].isEmpty())
			{
				std::logic_error excep("path has error!");
				throw std::exception(excep);
			}
		}
	}
	Directory lastRoot = ReadDirectory(0);

	for (int i = 1; i < dirList.count(); i++)
	{
#ifdef _WIN32
		lastRoot = FindDirectoryFromWName(lastRoot.RootDID, dirList[i].toStdWString());
#else
		lastRoot = FindDirectoryFromName(lastRoot.RootDID, dirList[i].toStdString());
#endif
		//如果找不到指定目录
		if (lastRoot.NameSize == 0)
		{
			std::logic_error excep("can't find the specified directory!");
			throw std::exception(excep);
		}
	}
	byte_t *p;
	if (lastRoot.StreamSize < m_Header.MiniSize)
		p = ReadShortStreamFromSID(lastRoot.SID, lastRoot.StreamSize);
	else
		p = ReadStreamFromSID(lastRoot.SID, lastRoot.StreamSize);//一次性把所需要写的字符流读取，然后写入文件
	FileBlock *block = new FileBlock(p, lastRoot.StreamSize);
	return block;
}
Directory ComDoc::ComDocIO::FindDirectoryFromWName(i32_t RootDID, std::wstring name)
{

	//入口名宽字符转单字符
	/*char sCharName[20] = { 0 };
	mbstate_t state = mbstate_t();
	wchar_t* mCharName = (wchar_t*)direct.EntryName;
	size_t countBytes = std::wcsrtombs(nullptr, (const wchar_t**)&mCharName, 0, &state);*/
	Directory direct = ReadDirectory(RootDID);

	std::wstring entry((wchar_t*)direct.EntryName);
	int comp = CompareWstring(name, entry);
	if (comp == 0)
		return direct;
	else if (comp == -1)
	{
		if (direct.LeftDID < 0)
			return Directory();
		else
		{
			Directory dir = FindDirectoryFromWName(direct.LeftDID, name);
			return dir;
		}
	}
	else
	{
		if (direct.RightDID < 0)
			return Directory();
		else
		{
			Directory dir = FindDirectoryFromWName(direct.RightDID, name);
			return dir;
		}
	}

}
Directory ComDoc::ComDocIO::FindDirectoryFromName(i32_t RootDID, std::string name)
{
	Directory direct = ReadDirectory(RootDID);
	
	std::string entry;
	for (int i = 0; i < direct.NameSize; i++)
	{
		if (i % 2 == 0 && direct.EntryName[i]!='\0')
		{
			entry.insert(entry.end(), direct.EntryName[i]);
		}
	}
	int comp = Comparestring(name, entry);
	if (comp == 0)
		return direct;
	else if (comp == -1)
	{
		if (direct.LeftDID < 0)
			return Directory();
		else
		{
			Directory dir = FindDirectoryFromName(direct.LeftDID, name);
			return dir;
		}
	}
	else
	{
		if (direct.RightDID < 0)
			return Directory();
		else
		{
			Directory dir = FindDirectoryFromName(direct.RightDID, name);
			return dir;
		}
	}
}
ComDocIO::~ComDocIO()
{
	if (dat)
	{
		//do something   to fpr
		file.unmap(dat);
	}
	file.close();
	for(auto *vec : FilePool)
	{
		if (vec)
			delete vec;
	}
}

void ComDocIO::ReadHeader()
{
	if (dat)
	{
		memcpy(m_Header.FileID, dat, sizeof(m_Header.FileID));
		ptr = dat + 8;//偏移8字节
		memcpy(m_Header.UID, dat, sizeof(m_Header.UID));
		m_Header.ReviNum = get_ui16(dat, 24);
		m_Header.VerNum = get_ui16(dat, 26);
		m_Header.ByteOrder = get_ui16(dat, 28);
		if (m_Header.ByteOrder != 0xFFFE)
		{
			//如果不是低端字节序
			return;
		}
		m_Header.SectorSize = MAKE_UI64(1) << get_ui16(dat, 30);
		if (m_Header.SectorSize != 512)
		{
			//如果扇区大小异常
		}
		m_Header.ShortSectSize = MAKE_UI64(1) << get_ui16(dat, 32);
		if (m_Header.ShortSectSize == 0)
		{
			//如果短扇区过大
		}
		m_Header.SectNum = get_ui32(dat, 44);
		m_Header.DirectSID = get_i32(dat, 48);
		m_Header.MiniSize = get_ui32(dat, 56);
		m_Header.ShortSectSID = get_i32(dat, 60);
		m_Header.ShortSectNum = get_ui32(dat, 64);
		m_Header.MSectSID = get_i32(dat, 68);
		if (m_Header.MSectSID != -2)
		{
			//如果MSAT有附加
		}
		m_Header.MSectNum = get_ui32(dat, 72);
		//偏移76字节
		ptr = dat + 76;
		memcpy(m_Header.MSectPart, ptr, sizeof(m_Header.MSectPart));
	}
}

void ComDocIO::ReadFile(QString FilePath)
{
	file.setFileName(FilePath);//文件对象不能摧毁，否则文件内存将会消失
	file.open(QIODevice::ReadWrite);
	dat = file.map(0, file.size());//映射文件，注意这里的file.size()如果大于系统的虚拟内存，就要对文件分块映射
										  //其实对于大文件一般都是进行分块映射的。
}

Directory ComDocIO::ReadDirectory(ui32_t DID)
{
	Directory direct;
	i32_t SeriNum = DID * 128 / m_Header.SectorSize;
	i32_t off = DID * 128 % m_Header.SectorSize;
	//i32_t NowSID = FindSID(m_Header.DirectSID, SeriNum);
	i32_t NowSID = 0;
	///////
	if (m_Header.DirectSID < 0)
	{
		//SID已结束
		//NowSID= m_Header.DirectSID;
		//获取的SID有问题
		return Directory();
	}
	else
	{
		NowSID = m_Header.DirectSID;
		for (ui32_t index = 0; index < SeriNum; index++)
			NowSID = FindNextSID(NowSID);
	}
	//if (NowSID < 0)
	//{
	//	//获取的SID有问题
	//	return Directory();
	//}

	ptr = dat + (NowSID + 1)*m_Header.SectorSize + off;//偏移到目录入口
	memcpy(direct.EntryName, ptr, sizeof(direct.EntryName));
	direct.NameSize = get_ui16(ptr, 64);
	direct.EntryType = get_byte(ptr, 66);
	direct.NodeColor = get_byte(ptr, 67);
	direct.LeftDID = get_i32(ptr, 68);
	direct.RightDID = get_i32(ptr, 72);
	direct.RootDID = get_i32(ptr, 76);
	direct.SID = get_i32(ptr, 116);
	direct.StreamSize = get_ui32(ptr, 120);
	direct.DID = DID;
	return direct;
}

i32_t ComDocIO::FindNextSID(i32_t SID)
{
	if (SID < 0)
	{
		//SID已结束
		return SID;
	}

	i32_t NextSID;
	i32_t seriNum = SID / (m_Header.SectorSize / 4);
	i32_t off = SID % (m_Header.SectorSize / 4);
	if (seriNum < 109)
	{
		ptr = dat + m_Header.MSectPart[seriNum] * m_Header.SectorSize + m_Header.SectorSize;
		NextSID = get_i32(ptr, off * 4);
	}
	else
	{
		int SIDCount = m_Header.SectorSize / 4 - 1;
		i32_t seri = (seriNum - 109) / SIDCount;
		i32_t offset = (seriNum - 109) % SIDCount;
		ptr = dat + SATAllo.at(seri) * m_Header.SectorSize + m_Header.SectorSize;
		i32_t EntrySID = get_i32(ptr, offset * 4);
		ptr = dat + EntrySID * m_Header.SectorSize + m_Header.SectorSize;
		NextSID = get_i32(ptr, off * 4);
	}
	return NextSID;
}


i32_t ComDocIO::FindSID(i32_t SID, ui32_t Offset = 1)//偏移量下的扇区SID查找
{
	if (SID < 0)
	{
		//SID已结束
		return SID;
	}
	i32_t NowSID = SID;
	for (ui32_t index = 0; index < Offset; index++)
		NowSID = FindNextSID(NowSID);
	return NowSID;
}

void ComDocIO::ConfigureShorStream()//配置短流存放流表和相应的存放扇区
{
	if (dat == 0)
		return;
	i32_t NowSID = m_Header.ShortSectSID;
	if (NowSID < 0)
		return;
	do
	{
		ShortSectAllo.append(NowSID);
		NowSID = FindNextSID(NowSID);
		if (ShortSectAllo.count() > 10000000)//避免出现扇区链不中断的情况
			return;
	} while (NowSID != -2);

	Directory RootDirect = ReadDirectory(0);
	if (RootDirect.EntryType != 5)
	{
		//DID为0的不为RootEntry，则退出
		return;
	}
	NowSID = RootDirect.SID;
	do
	{
		ShortSreamCon.append(NowSID);
		NowSID = FindNextSID(NowSID);
		if (ShortSectAllo.count() > 10000000)//避免出现扇区链不中断的情况
			return;
	} while (NowSID != -2);
}

i32_t ComDocIO::FindNextShortSID(i32_t SID)
{
	if (ShortSectAllo.count() == 0)
	{
		//如果配置表为空则返回
		return 0;
	}
	i32_t NextSID;
	i32_t SeriNum = SID / (m_Header.SectorSize / 4);
	i32_t off = SID % (m_Header.SectorSize / 4);
	ptr = dat + ShortSectAllo.at(SeriNum)*m_Header.SectorSize + m_Header.SectorSize;
	NextSID = get_i32(ptr, off * 4);
	return NextSID;
}

byte_t* ComDocIO::GetAddressFromShortSID(i32_t SID)
{
	if (ShortSreamCon.count() == 0)
	{
		//如果短流存放区为空
		return NULL;
	}
	i32_t SeriNum = SID*m_Header.ShortSectSize / m_Header.SectorSize;
	i32_t off = SID*m_Header.ShortSectSize % m_Header.SectorSize;
	ptr = dat + ShortSreamCon[SeriNum] * m_Header.SectorSize + m_Header.SectorSize + off;
	return ptr;
}

byte_t * ComDocIO::ReadStreamFromSID(i32_t SID, ui64_t len)
{
	byte_t *p = (byte_t *)malloc(len);
	byte_t *Nowp = p;
	i32_t SeriNum = len / m_Header.SectorSize;
	i32_t off = len%m_Header.SectorSize;
	i32_t NowSID = SID;
	do
	{
		ptr = dat + NowSID*m_Header.SectorSize + m_Header.SectorSize;
		if (SeriNum == 0)
		{
			memcpy(Nowp, ptr, off);
			return p;
		}
		else
		{
			memcpy(Nowp, ptr, m_Header.SectorSize);
			Nowp += m_Header.SectorSize;
			SeriNum--;
			NowSID = FindNextSID(NowSID);
		}
	} while (NowSID != -2);
	return p;
}
byte_t*  ComDocIO::ReadShortStreamFromSID(i32_t SID, ui64_t len)
{
	byte_t *p = (byte_t *)malloc(len);
	byte_t *Nowp = p;
	i32_t SeriNum = len / m_Header.ShortSectSize;
	i32_t off = len%m_Header.ShortSectSize;
	i32_t NowSID = SID;

	do
	{
		ptr = GetAddressFromShortSID(NowSID);
		if (SeriNum == 0)
		{
			memcpy(Nowp, ptr, off);
			return p;
		}
		else
		{
			memcpy(Nowp, ptr, m_Header.ShortSectSize);
			Nowp += m_Header.ShortSectSize;
			SeriNum--;
			NowSID = FindNextShortSID(NowSID);
		}
	} while (NowSID != -2);
	return p;

}

void ComDocIO::CreateFilePool(ui32_t RootDID, QVector<Directory> *vec)//创建文件池
{
	if (RootDID < 0)
		return;
	Directory root = ReadDirectory(RootDID);
	SerchTree(root.RootDID, vec);
	FilePool.insert(RootDID, vec);
	for(auto sub : *vec)
	{
		if (sub.EntryType == 1)
		{
			QVector<Directory> *subvec = new QVector<Directory>();
			CreateFilePool(sub.DID, subvec);
		}
	}


}
void  ComDocIO::SerchTree(ui32_t RootDID, QVector<Directory> *vec)
{
	Directory direct = ReadDirectory(RootDID);
	FileCount++;
	vec->append(direct);
	if (direct.LeftDID > 0)
	{
		SerchTree(direct.LeftDID, vec);
	}
	if (direct.RightDID > 0)
	{
		SerchTree(direct.RightDID, vec);
	}
	return;
}

void ComDocIO::ConfigureSAT()
{
	if (m_Header.MSectSID < 0)
		return;
	SATAllo.append(m_Header.MSectSID);
	SerchSAT(m_Header.MSectSID);

}

void ComDocIO::SerchSAT(i32_t SID)
{
	ptr = dat + SID*m_Header.SectorSize + m_Header.SectorSize;
	i32_t NowSID = get_i32(ptr, m_Header.SectorSize - 4);
	if (NowSID >= 0)
	{
		SATAllo.append(NowSID);
		SerchSAT(NowSID);
	}
}
