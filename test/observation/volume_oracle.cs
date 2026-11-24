using System;
using System.Text;
using System.Runtime.InteropServices;
public static class S37VolumeOracle {
    [DllImport("kernel32.dll", CharSet=CharSet.Unicode, SetLastError=true)]
    public static extern bool GetVolumeInformationW(string root,
        StringBuilder label, uint labelSize, out uint serial,
        out uint maxComponent, out uint flags, StringBuilder filesystem,
        uint filesystemSize);
}
