[CmdletBinding()]
param(
    [Parameter(Mandatory)][string[]]$Image,
    [Parameter(Mandatory)][guid]$PackageId
)
$ErrorActionPreference='Stop'
if($PackageId -eq [guid]::Empty){throw 'Package identity must not be empty'}
if(-not ('NtvdmPackageResource' -as [type])) {
    Add-Type -TypeDefinition @'
using System;
using System.ComponentModel;
using System.Runtime.InteropServices;
public static class NtvdmPackageResource {
    [DllImport("kernel32.dll", CharSet=CharSet.Unicode, SetLastError=true)]
    static extern IntPtr BeginUpdateResourceW(string file, bool deleteExisting);
    [DllImport("kernel32.dll", CharSet=CharSet.Unicode, SetLastError=true)]
    static extern bool UpdateResourceW(IntPtr update, IntPtr type, string name,
        ushort language, byte[] data, uint size);
    [DllImport("kernel32.dll", SetLastError=true)]
    static extern bool EndUpdateResourceW(IntPtr update, bool discard);
    public static void Set(string file, byte[] value) {
        IntPtr update=BeginUpdateResourceW(file,false);
        if(update==IntPtr.Zero) throw new Win32Exception(Marshal.GetLastWin32Error());
        bool committed=false;
        try {
            if(!UpdateResourceW(update,(IntPtr)10,"NTVDM_PACKAGE_ID",0,value,(uint)value.Length))
                throw new Win32Exception(Marshal.GetLastWin32Error());
            if(!EndUpdateResourceW(update,false)) {
                update=IntPtr.Zero;
                throw new Win32Exception(Marshal.GetLastWin32Error());
            }
            update=IntPtr.Zero; committed=true;
        } finally {
            if(!committed && update!=IntPtr.Zero) EndUpdateResourceW(update,true);
        }
    }
}
'@
}
$bytes=$PackageId.ToByteArray()
foreach($item in $Image) {
    $path=(Resolve-Path -LiteralPath $item -ErrorAction Stop).Path
    [NtvdmPackageResource]::Set($path,$bytes)
}
