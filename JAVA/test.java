package com.platform.util;
public class test {
  static{
    System.loadLibrary("JavaDll");
  }
  test(String filePath)
  {
    CreateMDSFile(filePath);
  }
  public native void CreateMDSFile(String filePath);
  public native byte[] GetTileData(int Level,int x,int y);
  public native void ReleaseMDSFile();
  public long ptrMDS;
  public int maxLevel;
  public int ImageWidth;
  public int ImageHeight;
  public int TileWidth;
  public int TileHeight;

  public static void main(String[] args) {
    test newClass=new test("E:\\web\\NEW\\1.mds");
    System.out.println("maxLevel:"+newClass.maxLevel);
    System.out.println("ImageWidth:"+newClass.ImageWidth);
    System.out.println("ImageHeight:"+newClass.ImageHeight);
    System.out.println("TileWidth:"+newClass.TileWidth);
    System.out.println("TileHeight:"+newClass.TileHeight);
byte[] data=newClass.GetTileData(12,1,5);
    //ªÒ»°Õﬂ∆¨
	for(int i=0;i<20;i++)
{
System.out.println(data[i]);
}
    
    newClass.ReleaseMDSFile();
  }
}
