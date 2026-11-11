param(
    [Parameter(Mandatory)][int]$ObserverPid,
    [Parameter(Mandatory)][string]$OutputRoot,
    [Parameter(Mandatory)][int]$FrontendPid,
    [int]$WorkerPid=0,
    [switch]$NoToggle
)
$ErrorActionPreference='Stop'
$root=(Resolve-Path -LiteralPath $OutputRoot).Path
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)) { throw 'Build-owned output required' }
$savedTemp=$env:TEMP; $savedTmp=$env:TMP
try {
    $env:TEMP=$root; $env:TMP=$root
    if($PSVersionTable.PSEdition -eq 'Core') { Add-Type -AssemblyName System.Drawing.Common }
    else { Add-Type -AssemblyName System.Drawing }
    $drawing=@([System.Drawing.Bitmap].Assembly.Location,[System.Drawing.Point].Assembly.Location) | Select-Object -Unique
    Add-Type -ReferencedAssemblies $drawing -TypeDefinition @'
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Text;
public static class Win101SetupProbe {
    [StructLayout(LayoutKind.Explicit,Size=20)] public struct Record {
        [FieldOffset(0)] public short Type;
        [FieldOffset(4)] public int Down;
        [FieldOffset(8)] public short Repeat;
        [FieldOffset(10)] public short Key;
        [FieldOffset(12)] public short Scan;
        [FieldOffset(16)] public int State;
    }
    public delegate bool Visit(IntPtr hwnd,IntPtr param);
    [DllImport("kernel32.dll")] static extern bool FreeConsole();
    [DllImport("kernel32.dll",SetLastError=true)] static extern bool AttachConsole(uint pid);
    [DllImport("kernel32.dll")] static extern IntPtr GetStdHandle(int kind);
    [DllImport("kernel32.dll")] static extern bool SetStdHandle(int kind,IntPtr handle);
    [DllImport("kernel32.dll",CharSet=CharSet.Unicode,SetLastError=true)] static extern bool WriteConsoleInputW(IntPtr input,Record[] records,uint count,out uint written);
    [DllImport("user32.dll",CharSet=CharSet.Unicode)] static extern IntPtr OpenDesktopW(string name,uint flags,bool inherit,uint access);
    [DllImport("user32.dll")] static extern bool CloseDesktop(IntPtr desktop);
    [DllImport("user32.dll")] static extern bool EnumDesktopWindows(IntPtr desktop,Visit visit,IntPtr param);
    [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr window,out uint pid);
    [DllImport("user32.dll")] static extern bool GetWindowRect(IntPtr window,out Rect rect);
    [DllImport("user32.dll")] static extern bool PrintWindow(IntPtr window,IntPtr dc,uint flags);
    [DllImport("user32.dll",CharSet=CharSet.Unicode)] static extern int GetWindowTextW(IntPtr window,StringBuilder text,int size);
    [DllImport("user32.dll",CharSet=CharSet.Unicode)] static extern int GetClassNameW(IntPtr window,StringBuilder text,int size);
    [DllImport("user32.dll")] static extern bool IsWindowVisible(IntPtr window);
    [StructLayout(LayoutKind.Sequential)] public struct Rect { public int Left,Top,Right,Bottom; }
    public static void CAF(uint observer) {
        IntPtr savedIn=GetStdHandle(-10),savedOut=GetStdHandle(-11),savedError=GetStdHandle(-12);
        FreeConsole();
        try {
            if(!AttachConsole(observer)) throw new Exception("AttachConsole "+Marshal.GetLastWin32Error());
            Record[] keys=new Record[2];
            keys[0]=new Record{Type=1,Down=1,Repeat=1,Key=70,Scan=0x21,State=0x2a};
            keys[1]=keys[0]; keys[1].Down=0;
            uint written;
            if(!WriteConsoleInputW(GetStdHandle(-10),keys,2,out written) || written!=2) throw new Exception("CAF write failed");
        } finally {
            FreeConsole();
            SetStdHandle(-10,savedIn); SetStdHandle(-11,savedOut); SetStdHandle(-12,savedError);
        }
    }
    public static int Capture(string desktopName,uint wanted,string root) {
        IntPtr desktop=OpenDesktopW(desktopName,0,false,0x41);
        if(desktop==IntPtr.Zero) throw new Exception("Open private desktop failed");
        int count=0;
        try {
            Visit callback=delegate(IntPtr window,IntPtr unused) {
                uint pid; GetWindowThreadProcessId(window,out pid);
                if(pid!=wanted) return true;
                StringBuilder title=new StringBuilder(1024),kind=new StringBuilder(128);
                GetWindowTextW(window,title,title.Capacity);GetClassNameW(window,kind,kind.Capacity);
                System.IO.File.AppendAllText(System.IO.Path.Combine(root,"windows-"+wanted+".txt"),
                    "hwnd="+window+" visible="+IsWindowVisible(window)+" class="+kind+" title="+title+Environment.NewLine);
                Rect r; if(!GetWindowRect(window,out r)) return true;
                int w=r.Right-r.Left,h=r.Bottom-r.Top;
                if(w<=0 || h<=0 || w>4096 || h>4096) return true;
                using(Bitmap bitmap=new Bitmap(w,h)) using(Graphics g=Graphics.FromImage(bitmap)) {
                    IntPtr dc=g.GetHdc(); bool ok;
                    try { ok=PrintWindow(window,dc,2); } finally { g.ReleaseHdc(dc); }
                    if(ok) { bitmap.Save(System.IO.Path.Combine(root,"window-"+wanted+"-"+count+".bmp"),ImageFormat.Bmp); count++; }
                }
                return true;
            };
            if(!EnumDesktopWindows(desktop,callback,IntPtr.Zero)) throw new Exception("Enum private windows failed");
            GC.KeepAlive(callback);
        } finally { CloseDesktop(desktop); }
        return count;
    }
}
'@
    $observer=[Diagnostics.Process]::GetProcessById($ObserverPid)
    $context=Get-Content "$root/setup-context.json" -Raw | ConvertFrom-Json
    if($observer.HasExited -or $context.pid -ne $ObserverPid -or
       $observer.MainModule.FileName -ne $context.image -or
       $observer.StartTime.ToUniversalTime() -ne [DateTime]::Parse($context.startUtc).ToUniversalTime()) {
        throw 'Not the pinned owned observer'
    }
    foreach($pidValue in @($FrontendPid,$WorkerPid)) {
        if(!$pidValue) { continue }
        $peer=[Diagnostics.Process]::GetProcessById($pidValue)
        try {
            if($peer.HasExited -or !$peer.MainModule.FileName.StartsWith('Z:\system32\',[StringComparison]::OrdinalIgnoreCase)) {
                throw 'Not an owned test peer'
            }
        } finally { $peer.Dispose() }
    }
    if(!$NoToggle) { [Win101SetupProbe]::CAF($ObserverPid) }
    $desktop=$context.desktop
    if($desktop -ne "NTVDMConsoleTest-$($context.parentPid)") { throw 'Wrong private desktop ownership' }
    # Bounded test acquisition, not a product polling/refresh mechanism.
    $deadline=[DateTime]::UtcNow.AddSeconds(5)
    $count=0
    while(!$count -and [DateTime]::UtcNow -lt $deadline) {
        $count=[Win101SetupProbe]::Capture($desktop,$FrontendPid,$root)
        if(!$count) { Start-Sleep -Milliseconds 100 }
    }
    if($WorkerPid) { $null=[Win101SetupProbe]::Capture($desktop,$WorkerPid,$root) }
    "Captured frontend windows=$count; no guest input or mouse success claimed"
} finally {
    $env:TEMP=$savedTemp; $env:TMP=$savedTmp
}
