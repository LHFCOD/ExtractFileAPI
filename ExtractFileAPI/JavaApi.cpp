#include "../JAVA/JavaApi.h"
#include "interface.h"
/*
* Class:     ExtractFile
* Method:    CreateMDSFile
* Signature: (Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL JavaFunc(CreateMDSFile)
(JNIEnv *env, jobject obj, jstring filePath)
{
	try
	{
		jclass javaApiClass = env->GetObjectClass(obj);
		jfieldID id_ptrMDS = env->GetFieldID(javaApiClass, "ptrMDS", "J");

		jboolean isCopy = 1;
		const char* path = env->GetStringUTFChars(filePath, &isCopy);
		QString strPath = QString::fromUtf8(path, env->GetStringUTFLength(filePath));
		MDSFile *ptrMDS = new MDSFile(strPath);

		env->SetLongField(obj, id_ptrMDS, (jlong)ptrMDS);
		//maxLevel
		jfieldID id_maxLevel = env->GetFieldID(javaApiClass, "maxLevel", "I");
		env->SetIntField(obj, id_maxLevel, ptrMDS->info.maxLevel);
		//ImageWidth
		jfieldID id_ImageWidth = env->GetFieldID(javaApiClass, "ImageWidth", "I");
		env->SetIntField(obj, id_ImageWidth, ptrMDS->info.ImageWidth);
		//ImageHeight
		jfieldID id_ImageHeight = env->GetFieldID(javaApiClass, "ImageHeight", "I");
		env->SetIntField(obj, id_ImageHeight, ptrMDS->info.ImageHeight);
		//TileWidth
		jfieldID id_TileWidth = env->GetFieldID(javaApiClass, "TileWidth", "I");
		env->SetIntField(obj, id_TileWidth, ptrMDS->info.TileWidth);
		//TileHeight
		jfieldID id_TileHeight = env->GetFieldID(javaApiClass, "TileHeight", "I");
		env->SetIntField(obj, id_TileHeight, ptrMDS->info.TileHeight);
	}
	catch (std::exception ex)
	{

	}
	catch(...)
	{

	}
}

/*
* Class:     ExtractFile
* Method:    GetTileData
* Signature: (III)[B
*/
JNIEXPORT jbyteArray JNICALL JavaFunc(GetTileData)
(JNIEnv *env, jobject obj, jint Level, jint x, jint y)
{
	try
	{
		jclass javaApiClass = env->GetObjectClass(obj);
		jfieldID id_ptrMDS = env->GetFieldID(javaApiClass, "ptrMDS", "J");
		MDSFile  *ptrMDS = (MDSFile*)env->GetLongField(obj, id_ptrMDS);

		if (ptrMDS != nullptr)
		{
			FileBlock *block = ptrMDS->GetTileVirtualData(Level, x, y);
			jbyteArray byteArray = env->NewByteArray(block->len);
			env->SetByteArrayRegion(byteArray, 0, block->len, (jbyte*)block->p);
			delete block;
			return byteArray;
		}
		else
		{
			jbyteArray byteArray = env->NewByteArray(0);
			return byteArray;
		}
	}
	catch (std::exception ex)
	{
		jbyteArray byteArray = env->NewByteArray(0);
		return byteArray;
	}
	catch (...)
	{
		jbyteArray byteArray = env->NewByteArray(0);
		return byteArray;
	}
}

/*
* Class:     ExtractFile
* Method:    ReleaseMDSFile
* Signature: ()V
*/
JNIEXPORT void JNICALL JavaFunc(ReleaseMDSFile)
(JNIEnv *env, jobject obj)
{
	try
	{
		jclass javaApiClass = env->GetObjectClass(obj);
		jfieldID id_ptrMDS = env->GetFieldID(javaApiClass, "ptrMDS", "J");
		MDSFile  *ptrMDS = (MDSFile*)env->GetLongField(obj, id_ptrMDS);
		if (ptrMDS != nullptr)
		{
			delete ptrMDS;
			env->SetLongField(obj, id_ptrMDS, 0);
		}
	}
	catch (std::exception ex)
	{
		
	}
	catch (...)
	{
	
	}
}