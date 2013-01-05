!system 'get_version.bat'
!include version.nsh
; The name of the installer
Name "Steel and concrete ${VERSION}"

; The file to write
OutFile "setup_steelandconcrete_${VERSION}.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\steelandconcrete"

; Registry key to check for directory (so if you install again, it will overwrite the old one automatically)
InstallDirRegKey HKLM "Software\steelandconcrete" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel user

;--------------------------------

; Pages
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;-------------------------------- 
 Function .onInit
  IfFileExists $SYSDIR\OpenAL32.dll allOk 0 
  MessageBox MB_OK "Can not find OpenAL32.dll in system directory. Please download and install OpenAL: http://connect.creativelabs.com/openal/Downloads/oalinst.zip"
  Abort
  allOk:
 FunctionEnd
 
; The stuff to install
Section "Files (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "bin\steelandconcrete.exe"
  File "bin\steelandconcrete_server.exe"      
  
  File /r "bin\*.mesh"
  File /r "bin\*.material"
  File /r "bin\*.skeleton"
  File /r "bin\*.program"
  File /r "bin\*.cg"
  File /r "bin\*.glsl"
  File /r "bin\*.scheme"
  File /r "bin\*.font"
  File /r "bin\*.imageset"
  File /r "bin\*.layout"
  File /r "bin\*.looknfeel"
  
  File /r "bin\*.wav"
  File /r "bin\*.ogg"
  File /r "bin\*.png"
  File /r "bin\*.tga"
  File /r "bin\*.jpg"  
  File /r "bin\*.ttf"

  File /r "bin\*.mo"
  
  
  ; Write the installation path into the registry
  WriteRegStr HKLM "SOFTWARE\steelandconcrete" "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\steelandconcrete" "DisplayName" "steelandconcrete ${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\steelandconcrete" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\steelandconcrete" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\steelandconcrete" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\steelandconcrete"
  CreateShortCut "$SMPROGRAMS\steelandconcrete\steelandconcrete.lnk" "$INSTDIR\steelandconcrete.exe" "" "$INSTDIR\steelandconcrete.exe" 0  
  CreateShortCut "$SMPROGRAMS\steelandconcrete\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\steelandconcrete"
  DeleteRegKey HKLM "SOFTWARE\NSIS_Example2"

  ; Remove files and uninstaller
  Delete "$INSTDIR\steelandconcrete.exe"
  Delete "$INSTDIR\steelandconcrete_server.exe"
  Delete "$INSTDIR\uninstall.exe"

  Delete "$SMPROGRAMS\steelandconcrete\*.*"
  Delete "$INSTDIR\*.log"

  RMDir /r "$INSTDIR\res"
  RMDir /r "$INSTDIR\lang"
  RMDir "$SMPROGRAMS\steelandconcrete"
  RMDir "$INSTDIR"

SectionEnd
