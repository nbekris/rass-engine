[Setup]

#define ApplicationName 'Rass'
#define ApplicationVersion '1.0.0'
#define CPlusPlusYearVersion '2014'

DisableWelcomePage=no
WizardStyle=modern dynamic
WizardImageFile=INSTALLERFILES\panel_image_100.bmp
WizardSmallImageFile=INSTALLERFILES\banner.bmp

AppId={{8AD81A61-E1BA-45F7-9D19-39C251973319}

; Standard app data stuff
AppName={#ApplicationName}
AppVerName={#ApplicationName}
VersionInfoVersion = {#ApplicationVersion}
AppVersion = {#ApplicationVersion}
AppPublisher=DigiPen Institute of Technology
AppPublisherURL=http://www.digipen.edu/
AppSupportURL=http://www.digipen.edu/

; Default path to the file storage directory.
; {pf} is the default program files directory set by Windows
DefaultDirName={autopf}\DigiPen\{#ApplicationName}

; Start menu directory
DefaultGroupName=DigiPen\{#ApplicationName}

; Output directory for the installer.
OutputDir=.\INSTALLER

; Setup executable installer
OutputBaseFilename={#ApplicationName}_Setup

; Path to the DigiPen EULA (Needed to pass TCRs)
LicenseFile=INSTALLERFILES\DigiPen_EULA.txt

; Compression scheme for the installer. Check Inno Setup help files for more options.
Compression=lzma
SolidCompression=yes

; Path to the icon for the installer (TCR check requires custom icon)
SetupIconFile=.\INSTALLERFILES\RassIcon.ico

; This allows the installer to run without admin privileges, which means you can't install
; in the Program Files, or change the registry. This is done to allow installation on Sandbox
; or other intermediate directory
PrivilegesRequired=none

; These should prevent the installer from ever asking for a reboot, notably with the VC runtime
; redist installs
AlwaysRestart=no
RestartIfNeededByRun=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
; Creates an installer option to allow/disallow desktop shortcut
; Checked by default
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"


; This is the list of files that the installer should grab and install.
;
; Destination Directory Notes
;   {app} is the root directory that you will be installing to.
;   {temp} is a temporary directory that Windows sets that gets deleted after the
;      installer quits.
;   {userdocs} is the directory for My Documents/Documents on Windows XP, Vista, and 7.
;
; For more information on default paths to install to, check the "Constants" article
;   in the Inno Setup 5 documentation.
;
; I recommend placing any installers for required stuff (DirectX, PhysX, etc)
;   in the general structure below to keep things neat for you.
[Files]
; Main executable
Source: ".\INSTALLERFILES\{#ApplicationName}.exe"; DestDir: "{app}"; Flags: ignoreversion

;Chirs Onorati: Add every possible image.
Source: ".\INSTALLERFILES\panel_image_*.bmp"; Flags: dontcopy

; The game directoy is exactly what you want your install directory in program files to look like
Source: .\GAMEDIRECTORY\*; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs

;Chris Onorati: You need to place any redists you want to install here under files, and then install them under the RUN section
Source: ".\REDIST\VC_redist.x64.exe"; DestDir: {tmp}; Flags: deleteafterinstall

; Required DLLs
Source: ".\INSTALLERFILES\DLLs\fmod.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\INSTALLERFILES\DLLs\glbinding.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\INSTALLERFILES\DLLs\glbinding-aux.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\INSTALLERFILES\DLLs\glfw3.dll"; DestDir: "{app}"; Flags: ignoreversion

; Assets
Source: ".\INSTALLERFILES\Assets\*"; DestDir: "{app}\Assets"; Flags: ignoreversion recursesubdirs createallsubdirs

; ImGui config
Source: ".\INSTALLERFILES\imgui.ini"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist

; Licenses
Source: ".\INSTALLERFILES\Licenses\*"; DestDir: "{app}\Licenses"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist

; This is the list of shortcuts that the installer will setup for you.
; Of note, this will create the uninstaller automatically.
;
; Directory Notes
;   {group} is the start menu location that the game will install shortcuts to.
;   {commondesktop} is your Windows desktop directory.
[Icons]
Name: {group}\{#ApplicationName}; Filename: {app}\{#ApplicationName}.exe; WorkingDir: {app}; IconFilename: "{app}\RassIcon.ico"
Name: {group}\{cm:UninstallProgram,{#ApplicationName}}; Filename: {uninstallexe}
Name: {commondesktop}\{#ApplicationName}; Filename: {app}\{#ApplicationName}.exe; Tasks: desktopicon; WorkingDir: {app}; IconFilename: "{app}\RassIcon.ico"

; List of items to execute in the installer.
; Note that this needs to run all executables in their silent versions as required by the TCRs.
; Note the parameters are simply paramters for each program - not for inno - you will need to look at each program/redist you install to see how to do it silently.
;
; The last item being run is the installer option to automatically launch the game after
;   the installer exits as required by the TCRs.

; TODO: Update this list with the correct redistributables for your game.
; NOTE: For C++ runtimes you may need to change the command line arguments for silent install.  
;Reference this site to see how to do so: https://silentinstallhq.com/visual-c-redistributable-silent-install-master-list/

[Run]
Filename: {tmp}\VC_redist.x64.exe; Parameters: /q /Q /quiet /passive /silent /norestart /NORESTART /noreboot /NOREBOOT; StatusMsg: Installing Visual C++ {#CPlusPlusYearVersion} Redistributable...
;Filename: {tmp}\dxsetup.exe; Parameters: /Q; StatusMsg: Installing DirectX...
Filename: {app}\{#ApplicationName}.exe; Description: {cm:LaunchProgram,{#ApplicationName}}; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: filesandordirs; Name: "{app}"
Type: filesandordirs; Name: "{localappdata}\{#ApplicationName}"

[Code]

//Scale the image based on DPI of monitor screen.
function GetScalingFactor: Integer;
begin
  if WizardForm.Font.PixelsPerInch >= 192 then Result := 200
    else
  if WizardForm.Font.PixelsPerInch >= 144 then Result := 150
    else
  if WizardForm.Font.PixelsPerInch >= 120 then Result := 125
    else Result := 100;
end;

//Set image of the far left side panel.
procedure LoadEmbededScaledBitmap(Image: TBitmapImage; NameBase: string);

var Name: String;
var FileName: String;

begin
  Name := Format('%s_%d.bmp', [NameBase, GetScalingFactor]);
  ExtractTemporaryFile(Name);
  FileName := ExpandConstant('{tmp}\' + Name);
  Image.Bitmap.LoadFromFile(FileName);
  DeleteFile(FileName);
end;

//Set images based on resolution.
procedure InitializeWizard;

begin
  { If using larger scaling, load the correct size of images }
  if GetScalingFactor > 100 then 
  begin
    LoadEmbededScaledBitmap(WizardForm.WizardBitmapImage, 'panel_image');
  end;

end;