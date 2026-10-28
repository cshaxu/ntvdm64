"""Run the exact host_read_resource source body with bounded fake file I/O."""
import hashlib
import json
import subprocess
import tempfile
from pathlib import Path

source = Path('src/mvdm/softpc.new/host/src/nt_rez.c').read_bytes().decode('latin1')
start = source.index('long host_read_resource(')
opening = source.index('{', start)
depth = 0
end = None
for index in range(opening, len(source)):
    if source[index] == '{':
        depth += 1
    elif source[index] == '}':
        depth -= 1
        if depth == 0:
            end = index + 1
            break
assert end
body = source[start:end]
assert 'if (size > maxsize)' in body and 'return(0);' in body and '_close(file_fd)' in body
prefix = r'''
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
typedef unsigned char byte;
#define MAXPATHLEN 260
#define ROMS_REZ_ID 42
#define O_RDONLY 0
#define O_BINARY 0
static int mode, opens, reads, closes, seeks, rom_calls;
char *host_find_file(char *name, char *path, int display_error) {
    (void)path; (void)display_error; return name;
}
int _open(const char *path, int flags) { (void)path; (void)flags; opens++; return mode == 0 ? -1 : 7; }
long _lseek(int fd, long offset, int origin) {
    (void)fd; (void)offset; seeks++; return origin == 2 ? (mode == 2 ? 65 : 3) : 0;
}
int _read(int fd, void *buffer, unsigned int count) {
    (void)fd; (void)buffer; reads++; return (int)count;
}
int _close(int fd) { (void)fd; closes++; return 0; }
long mvdm_softpc_firmware_read_embedded_rom(char *name, byte *addr, uint32_t size) {
    (void)name; (void)addr; (void)size; rom_calls++; return 11;
}
'''
suffix = r'''
static void check(int condition, const char *label) {
    if (!condition) { fprintf(stderr, "FAIL %s\n", label); exit(1); }
    printf("PASS %s\n", label);
}
static void reset(int m) { mode=m; opens=reads=closes=seeks=rom_calls=0; }
int main(void) {
    byte data[64]; long result;
    reset(0); result=host_read_resource(1,"cmos.ram",data,64,0);
    check(result==0 && opens==1 && !reads && !closes, "failed open has no close");
    reset(1); result=host_read_resource(1,"cmos.ram",data,64,0);
    check(result==3 && opens==1 && reads==1 && closes==1 && seeks==2, "valid resource closes once");
    reset(2); result=host_read_resource(1,"cmos.ram",data,64,0);
    check(result==0 && opens==1 && !reads && !closes && seeks==1, "oversize returns without close");
    reset(1); result=host_read_resource(ROMS_REZ_ID,"rom",data,64,0);
    check(result==11 && !opens && rom_calls==1 && !closes, "embedded ROM bypasses file path");
    return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='rom-read-audit-', dir='build/t473-s10') as temp:
    root = Path(temp)
    program = root / 'fixture.c'
    executable = root / 'fixture.exe'
    program.write_text(prefix + '\n' + body + '\n' + suffix, encoding='latin1')
    subprocess.run(['gcc', '-std=c11', '-O0', '-o', str(executable), str(program)], check=True)
    output = subprocess.check_output([str(executable)], text=True)
print(json.dumps(dict(result='pass', source_sha256=hashlib.sha256(source.encode('latin1')).hexdigest().upper(),
                      body_sha256=hashlib.sha256(body.encode('latin1')).hexdigest().upper(),
                      checks=output.splitlines(),
                      scope='Actual extracted source body with fake file boundaries; no actual session path, native handle leak or guest behavior proof'), indent=2))
