using System;
using System.Text;
using System.Runtime.InteropServices;
public static class S38FatalDialog {
    public delegate bool Callback(IntPtr w, IntPtr p);
    [DllImport("user32.dll")] static extern bool EnumWindows(Callback cb, IntPtr p);
    [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr w, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] static extern int GetClassName(IntPtr w, StringBuilder b, int n);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] static extern uint GetDlgItemText(IntPtr w, int id, StringBuilder b, int n);
    [DllImport("user32.dll")] static extern IntPtr GetDlgItem(IntPtr w, int id);
    [DllImport("user32.dll")] static extern IntPtr SendMessageTimeout(IntPtr w, uint m, IntPtr a, IntPtr b, uint flags, uint ms, out IntPtr result);
    [DllImport("kernel32.dll")] static extern IntPtr OpenThread(uint access, bool inherit, uint tid);
    [DllImport("kernel32.dll")] static extern uint WaitForSingleObject(IntPtr h, uint ms);
    [DllImport("kernel32.dll")] static extern bool CloseHandle(IntPtr h);
    public static uint ThreadId(IntPtr w) { uint pid; return GetWindowThreadProcessId(w,out pid); }
    public static void IgnoreAndWait(IntPtr w) {
        IntPtr h=OpenThread(0x100000,false,ThreadId(w));
        if(h==IntPtr.Zero) throw new Exception("Cannot observe original dialog thread");
        try {
            if(!Click(w,102)) throw new Exception("Ignore action failed");
            if(WaitForSingleObject(h,5000)!=0) throw new Exception("Original dialog thread did not exit");
        } finally { CloseHandle(h); }
    }
    public static IntPtr Find(uint pid) {
        IntPtr found=IntPtr.Zero;
        EnumWindows(delegate(IntPtr w, IntPtr p) {
            uint owner; GetWindowThreadProcessId(w,out owner);
            if(owner==pid) { var b=new StringBuilder(128); GetClassName(w,b,128);
                if(b.ToString()=="#32770") { found=w; return false; } }
            return true;
        },IntPtr.Zero);
        return found;
    }
    public static string Text(IntPtr w) { var b=new StringBuilder(2048); GetDlgItemText(w,104,b,2048); return b.ToString(); }
    public static bool Click(IntPtr w, int id) {
        IntPtr button=GetDlgItem(w,id), result;
        return button!=IntPtr.Zero && SendMessageTimeout(w,0x111,new IntPtr(id),button,2,2000,out result)!=IntPtr.Zero;
    }
}
