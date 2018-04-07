package com.platform.util;
public class ExtractFile {
  static{
    System.loadLibrary("JavaDll");
  }
  ExtractFile(String filePath)
  {
    CreateMDSFile(filePath);
  }
  public native void CreateMDSFile(String filePath);
  public native byte[] GetTileData(int Level,int x,int y);
  public native void ReleaseMDSFile();
  private long ptrMDS;
  public int maxLevel;
  public int ImageWidth;
  public int ImageHeight;
  public int TileWidth;
  public int TileHeight;
  public static void main(String[] args) {
    ExtractFile newClass=new ExtractFile("C:\\Users\\NCEPU\\Documents\\Visual Studio 2015\\Projects\\ExtractFileAPI\\mds\\气管.mds");
    System.out.println("maxLevel:"+newClass.maxLevel);
    System.out.println("ImageWidth:"+newClass.ImageWidth);
    System.out.println("ImageHeight:"+newClass.ImageHeight);
    System.out.println("TileWidth:"+newClass.TileWidth);
    System.out.println("TileHeight:"+newClass.TileHeight);
    //获取瓦片
    byte[] data=newClass.GetTileData(11,1,7);
for (int i=0;i<20;i++)
{
System.out.print((char)data[i]);
}
    newClass.ReleaseMDSFile();
  }
}
