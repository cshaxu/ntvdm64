"""Bounded source-body comparison; mocked handles, no real Console access."""
import hashlib
import re
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
FILES = {
    "original": (Path("O:/repos.external/OpenNT/base/mvdm/softpc.new/host/src/nt_hosts.c"),
                 "9840CB53A0298FDF7F9D27ADE387C57659E3C57AFBE587CFDF63FBDBAFE8C41D"),
    "current": (ROOT / "src/mvdm/softpc.new/host/src/nt_hosts.c",
                "86D35DEA880856074CE1C205B9EB1F0AB1DA0664CEC3A5422221A99CC3DF5034"),
}
PREFIX = r'''
#include <stdio.h>
#include <stdint.h>
#include <string.h>
typedef intptr_t HANDLE;
typedef unsigned long DWORD;
typedef struct { DWORD nLength; void *lpSecurityDescriptor; int bInheritHandle; } SECURITY_ATTRIBUTES;
struct { HANDLE OutputHandle, ActiveOutputBufferHandle, ScreenBufHandle, InputHandle; int StaticPalette, FontsAreOpen; } sc;
HANDLE SCS_hStdIn, SCS_hStdOut, SCS_hStdErr;
int stdoutRedirected, VDMForWOW=1, DosSessionId=1, closed[100];
#define INVALID_HANDLE_VALUE ((HANDLE)-1)
#define STD_OUTPUT_HANDLE 1
#define STD_INPUT_HANDLE 2
#define STD_ERROR_HANDLE 3
#define FILE_TYPE_CHAR 2
#define GENERIC_READ 1
#define GENERIC_WRITE 2
#define FILE_SHARE_WRITE 4
#define FILE_SHARE_READ 8
#define OPEN_EXISTING 3
#define TRUE 1
#define FALSE 0
#define EHS_FUNC_FAILED 0
static HANDLE GetStdHandle(int n) { return n==STD_INPUT_HANDLE ? 11 : 22; }
static DWORD GetFileType(HANDLE h) { return h==11 ? FILE_TYPE_CHAR : 3; }
static int GetConsoleMode(HANDLE h,DWORD *m) { *m=0;return h==11; }
static HANDLE CreateFile(const char *n,DWORD a,DWORD b,SECURITY_ATTRIBUTES *s,DWORD d,DWORD f,void *t) { (void)a;(void)b;(void)s;(void)d;(void)f;(void)t;return !strcmp(n,"CONOUT$") ? 33 : 44; }
static void SetStdHandle(int n,HANDLE h) { (void)n;(void)h; }
static void CloseHandle(HANDLE h) { if(h>=0 && h<100)closed[h]=1; }
static DWORD GetLastError(void) { return 1; }
static void DisplayErrorTerm(int a,DWORD b,const char *c,int d) { (void)a;(void)b;(void)c;(void)d; }
static void SetupConsoleMode(void) {}
static void SetLastConsoleEventActive(void) {}
'''
SUFFIX = r'''
int main(void) {
    InitScreenDesc();
    printf("output=%ld active=%ld active_closed=%d redirected=%d\n",
        (long)sc.OutputHandle,(long)sc.ActiveOutputBufferHandle,
        closed[sc.ActiveOutputBufferHandle],stdoutRedirected);
    return sc.OutputHandle!=33 || sc.ActiveOutputBufferHandle!=22 || !stdoutRedirected;
}
'''

def body(text):
    match = re.search(r"void\s+InitScreenDesc\s*\(void\)\s*\{", text)
    if not match:
        raise ValueError("InitScreenDesc not found")
    depth, i = 1, match.end()
    while depth and i < len(text):
        depth += (text[i] == "{") - (text[i] == "}")
        i += 1
    if depth:
        raise ValueError("unbalanced body")
    return text[match.start():i]

with tempfile.TemporaryDirectory(prefix="screen-", dir=ROOT / "build/t473-s10") as tmp:
    for name, (path, expected) in FILES.items():
        raw = path.read_bytes()
        if hashlib.sha256(raw).hexdigest().upper() != expected:
            raise ValueError(name + " source identity changed")
        source, executable = Path(tmp) / (name + ".c"), Path(tmp) / (name + ".exe")
        source.write_text(PREFIX + body(raw.decode("latin1")) + SUFFIX, encoding="utf-8")
        subprocess.run(["gcc", "-std=c11", "-O0", str(source), "-o", str(executable)], check=True)
        result = subprocess.run([str(executable)], check=True, text=True, capture_output=True).stdout.strip()
        print(name, result)
        if ("active_closed=1" if name == "current" else "active_closed=0") not in result:
            raise ValueError(name + " unexpected handle state")
