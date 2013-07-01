CC = @cl.exe -nologo
CFLAGS = -c -EHsc -W3 -WX- -O2 -Oi -GL -Gm- -EHsc -GS -Gy -fp:precise -Zc:wchar_t -Zc:forScope -Gd -errorReport:queue
DEFINES = -DUNICODE -D_UNICODE -D WIN32 -D NDEBUG -D _WINDOWS

LD = @link.exe -nologo
LDFLAGS = -LTCG

RC = @rc.exe -nologo
RCFLAGS = -l 0x0409 -DUNICODE -D_UNICODE

STDLIBS = kernel32.lib user32.lib advapi32.lib gdi32.lib shell32.lib

PROJECT = PathEditor

all: $(PROJECT).exe

$(PROJECT).res:
	$(RC) $(RCFLAGS) -fo$(PROJECT).res ..\src\$(PROJECT).rc

clean: 
	@del PathEditor.exe PathEditor.obj PathEditorDlg.obj PathListCtrl.obj PathReader.obj Util.obj $(PROJECT).res
	@del PathEditor.exe.manifest PathEditor.ilk PathEditor.pdb vc100.pdb

$(PROJECT).exe: PathEditor.obj PathEditorDlg.obj PathListCtrl.obj PathReader.obj Util.obj $(PROJECT).res
	$(LD) $(LDFLAGS) $(STDLIBS) PathEditor.obj PathEditorDlg.obj PathListCtrl.obj PathReader.obj Util.obj $(PROJECT).res

PathEditor.obj: ..\src\resource.h
	$(CC) $(CFLAGS) $(DEFINES) ..\src\PathEditor.cpp

PathEditorDlg.obj: ..\src\PathEditorDlg.h ..\src\resource.h ..\src\Util.h
	$(CC) $(CFLAGS) $(DEFINES) ..\src\PathEditorDlg.cpp

PathListCtrl.obj: ..\src\PathListCtrl.h ..\src\resource.h
	$(CC) $(CFLAGS) $(DEFINES) ..\src\PathListCtrl.cpp

PathReader.obj: ..\src\PathReader.h
	$(CC) $(CFLAGS) $(DEFINES) ..\src\PathReader.cpp

Util.obj: ..\src\Util.h
	$(CC) $(CFLAGS) $(DEFINES) ..\src\Util.cpp
