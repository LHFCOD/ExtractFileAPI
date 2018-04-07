CC=cl
RM=del
INCLUDE=/I "D:\Qt\5.8\msvc2015_64\include" /I "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\include" /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.14393.0\ucrt"
LIBRARY= /LIBPATH:"D:\Qt\5.8\msvc2015_64\lib" /LIBPATH:"..\x64\Release"
LibFile=/DEFAULTLIB:Qt5Core.lib /DEFAULTLIB:Release_x64_ExtractFileAPI.lib

Source.exe : Source.obj
	link $(LIBRARY) $(LibFile) Source.obj
Source.obj : Source.cpp
	$(CC) $(INCLUDE) /c  Source.cpp 
clean:
	$(RM) *.obj *.exe
  