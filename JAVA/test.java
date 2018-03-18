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
    test newClass=new test("2.mds");
    System.out.println("maxLevel:"+newClass.maxLevel);
    System.out.println("ImageWidth:"+newClass.ImageWidth);
    System.out.println("ImageHeight:"+newClass.ImageHeight);
    System.out.println("TileWidth:"+newClass.TileWidth);
    System.out.println("TileHeight:"+newClass.TileHeight);

    //ªÒ»°Õﬂ∆¨
	for(int x=0;x<10;x++)
for(int y=0;y<10;y++)
{
byte[] data=newClass.GetTileData(12,x,y);
System.out.println("x:"+x+",y:"+y);
System.out.println("ptrMDS:"+newClass.ptrMDS);
for(int i=0;i<5;i++)
{
System.out.print((char)data[i]);
}
System.out.println("");
}
    
    newClass.ReleaseMDSFile();
  }
}
