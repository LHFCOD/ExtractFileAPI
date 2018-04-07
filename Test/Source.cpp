
#include "../ExtractFileAPI/CSharp.h"
//#include <QtCore/QtCore>
#ifdef _DEBUG
#pragma comment(lib,"../x64/Debug/Debug_x64_ExtractFileAPI")
#else 
#pragma comment(lib,"../x64/Release/Release_x64_ExtractFileAPI")
#endif

#include <stdio.h>
//#include "..\ExtractFileAPI\ComDocIO.h"
//using namespace ComDoc;

int main()
{
	printf("Hello World!");
	try
	{
		TileImage::MDSFile *mdsFile = new TileImage::MDSFile(u8"F:\\document\\haoge\\bigfile\\5.mds");
		for (int count=0;count<0;count++)
		for (int level = 18; level <= 18; level++)
		{
			int X = mdsFile->LayerMap[level].colCount;
			int Y = mdsFile->LayerMap[level].rowCount;
			for (int x = 0; x<X; x++)
				for (int y = 0; y < Y; y++)
				{
					FileBlock *block=mdsFile->GetTileData(level, x, y);
					if (block->len == 0 || block->p==nullptr)
					{
						printf("%d,%d,%d,error\n", level, x, y);
					}
					else
					printf("%d,%d,%d\n", level, x, y);
					delete block;
				}
		}
		delete mdsFile;
		//CreateMDSFile(u8"F:\\document\\haoge\\bigfile\\5.mds");
		//for (int i = 0; i < 1000; i++)
		//{
		//	for (int level = 10; level <= 18; level++)
		//	for (int x = 0; x<2; x++)
		//		for (int y = 0; y < 2; y++)
		//		{
		//			char* image = GetTileData(level, x, y);
		//			int len = GetTileSize(level, x, y);
		//			if (image != nullptr)
		//			{
		//				/*for (int zz = 0; zz < 10; zz++)
		//				{
		//				printf("%c", image + 2 * zz + 1);
		//				}
		//				printf("\n");*/
		//				free(image);
		//				image = nullptr;
		//			}
		//			else
		//			{
		//				printf("error\n");
		//			}
		//			if (len == 0)
		//			{
		//				printf("error\n");
		//			}
		//		}
		//	
		//}
		//

	}
	catch (std::exception exp)
	{
		printf("error\n");
	}
	catch (...)
	{
		printf("error\n");
	}
	/*for(int x=0;x<40;x++)
		for (int y = 0; y < 40; y++)
		{
			char* image = GetTileData(18, x, y);
			if (image != nullptr)
			{
				free(image);
				image = nullptr;
			}
			else
			{
				printf("error\n");
			}
		}*/
	ReleaseMDSFile();
	//ComDoc::ComDocIO io(u8"F:\\document\\皓哥\\大文件\\5.mds");

	/*ComDocIO *io = new ComDocIO(u8"C:\\Users\\NCEPU\\Documents\\Visual Studio 2015\\Projects\\ExtractFileAPI\\mds\\气管.mds");
	Directory direct=io->FindDirectoryFromWName(10039, L"0000_0001");
	FileBlock *block = io->ReadFromPath("\\DSI0\\0.031250\\0001_0001");
	delete io;*/
}