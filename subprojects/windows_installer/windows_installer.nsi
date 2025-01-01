!include "MUI2.nsh"

;--------------------------------
;General

Name "ikulab-motion-viewer"
OutFile "..\..\ikulab-motion-viewer_Installer.exe"
Unicode True

InstallDir "$PROGRAMFILES\ikulab-motion-viewer"

;--------------------------------
;Interface Settings

!define MUI_ABORTWARNING

;Show all languages, despite user's codepage
!define MUI_LANGDLL_ALLLANGUAGES

;--------------------------------
;Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;Support Languages
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Japanese"
!insertmacro MUI_LANGUAGE "SimpChinese"
!insertmacro MUI_LANGUAGE "TradChinese"
!insertmacro MUI_LANGUAGE "Korean"

;--------------------------------
;Variables

Var ALLUSERSPROFILE

;--------------------------------
;Installer Sections

Section
  ; Grant fullAccess to ProgrameData directory
  System::Call 'kernel32::GetEnvironmentVariable(t "ALLUSERSPROFILE", t .r0, i ${NSIS_MAX_STRLEN}) i .r1'
  StrCpy $ALLUSERSPROFILE $0

  CreateDirectory "$ALLUSERSPROFILE\ikulab-motion-viewer"
  AccessControl::GrantOnFile "$ALLUSERSPROFILE\ikulab-motion-viewer" "(BU)" "FullAccess"
  Pop $0
  ${If} $0 != "ok"
    MessageBox MB_OK "Failed to grant full access to $ALLUSERSPROFILE\ikulab-motion-viewer"
  ${EndIf}

  ; Install files
  SetOutPath "$INSTDIR"
  File "..\..\build_release_windows\src\app\ikulab-motion-viewer.exe"
  File "..\..\build_release_windows\src\app\vulkan-1.dll"
  File "..\..\build_release_windows\src\app\glfw3.dll"

  SetOutPath "$INSTDIR\fonts"
  File "..\..\assets\fonts\NotoSansJP-Medium.otf"

  ; Version checker
  SetOutPath "$INSTDIR"
  File "..\..\assets\imv_version_checker.exe"

  ; Version info dir
  SetOutPath "$ALLUSERSPROFILE\ikulab-motion-viewer\version_info"
  File "..\..\build_release_windows\version_info\current_version.txt"
  File "..\..\build_release_windows\version_info\skip_version.txt"

  ; VC++ Runtime Installer
  SetOutPath "$INSTDIR"
  File "..\..\assets\check_vcpp_version_and_install.exe"
  File "..\..\msvc_version.txt"
  File "..\..\vc_redist.x64.exe"

  ;Start Menu Shortcut
  CreateShortcut "$SMPROGRAMS\ikulab-motion-viewer.lnk" "$INSTDIR\ikulab-motion-viewer.exe" ""
  ;Desktop Shortcut
  CreateShortcut "$DESKTOP\ikulab-motion-viewer.lnk" "$INSTDIR\ikulab-motion-viewer.exe" ""

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  ;Registry for Uninstalling from Windows Settings
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ikulab-motion-viewer" "DisplayName" "ikulab-motion-viewer"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ikulab-motion-viewer" "UninstallString" '"$INSTDIR\Uninstall.exe"'

  ExecWait "$INSTDIR\check_vcpp_version_and_install.exe"

SectionEnd

;--------------------------------
;Installer Functions

Function .onInit

  ;Show Language Selector
  !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"
  System::Call 'kernel32::GetEnvironmentVariable(t "ALLUSERSPROFILE", t .r0, i ${NSIS_MAX_STRLEN}) i .r1'
  StrCpy $ALLUSERSPROFILE $0

  RMDir /r "$INSTDIR"
  RMDir /r "$ALLUSERSPROFILE\ikulab-motion-viewer"

  Delete "$SMPROGRAMS\ikulab-motion-viewer.lnk"
  Delete "$DESKTOP\ikulab-motion-viewer.lnk"

  ;Registry for Uninstalling from Windows Settings
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ikulab-motion-viewer"

SectionEnd