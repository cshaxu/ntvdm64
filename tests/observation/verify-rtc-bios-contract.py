"""Controlled selected RTC body/actual layout checks; not guest IRQ evidence."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess

p=argparse.ArgumentParser()
p.add_argument('--build-root',required=True)
a=p.parse_args()
repo=Path(__file__).resolve().parents[2]
out=(repo/a.build_root).resolve()
if not out.is_relative_to(repo/'build') or out.exists():
    raise SystemExit('Fresh repository build root required')
out.mkdir(parents=True)
source=repo/'src/mvdm-host/softpc.new/base/bios/rtc_bios.c'
header=repo/'src/mvdm-host/softpc.new/base/inc/rtc_bios.h'
text=source.read_text(encoding='utf-8')
body=text[text.index('void rtc_int(void)'):text.index('#endif   /* NTVDM */')]
prefix=r'''
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
typedef uint8_t half_word;
typedef uint16_t word;
typedef uint32_t double_word;
#define BIOS_VAR_START 0x400
#define CMOS_PORT 0x70
#define CMOS_DATA 0x71
#define CMOS_REG_B 11
#define CMOS_REG_C 12
#define CMOS_SHUT_DOWN 15
#define NMI_DISABLE 0x80
#define PIE 0x40
#define AIE 0x20
#define ICA1_PORT_0 0xa0
#define ICA0_PORT_0 0x20
#define RCPU_INT4A_SEGMENT 0xf000
#define RCPU_INT4A_OFFSET 0x100
static uint8_t memory[0x110000], regb, regc, index_reg;
static uint16_t cs, ip;
static unsigned callbacks, eois, ports[2], checks, failures;
#define CHECK(x) do { ++checks; if(!(x)){++failures;printf("FAIL line=%u: %s\n",(unsigned)__LINE__,#x);} } while(0)
static void outb(unsigned port,unsigned value) {
 if(port==CMOS_PORT) index_reg=(uint8_t)(value&0x7f);
 else if(port==CMOS_DATA){assert(index_reg==CMOS_REG_B);regb=(uint8_t)value;}
 else {assert(value==0x20 && eois<2);ports[eois++]=port;}
}
static void inb(unsigned port,half_word *value) {
 assert(port==CMOS_DATA);
 if(index_reg==CMOS_REG_B)*value=regb;
 else {assert(index_reg==CMOS_REG_C);*value=regc;regc=0;}
}
static void sas_loadw(unsigned addr,word *v) {*v=(word)(memory[addr]|memory[addr+1]<<8);}
static void sas_storew(unsigned addr,word v) {memory[addr]=(uint8_t)v;memory[addr+1]=(uint8_t)(v>>8);}
static void sas_store(unsigned addr,half_word v) {assert(addr<sizeof(memory));memory[addr]=v;}
static half_word sas_hw_at(unsigned addr) {return memory[addr];}
static unsigned effective_addr(word seg,word off) {return (unsigned)seg*16+off;}
static word getCS(void){return cs;}
static word getIP(void){return ip;}
static void setCS(word v){cs=v;}
static void setIP(word v){ip=v;}
static void host_simulate(void) {
 CHECK(cs==RCPU_INT4A_SEGMENT && ip==RCPU_INT4A_OFFSET);
 ++callbacks;cs=0x9999;ip=0x8888;
}
'''
main=r'''
static uint32_t count(void){word lo,hi;sas_loadw(RTC_LOW,&lo);sas_loadw(RTC_HIGH,&hi);return (uint32_t)lo|((uint32_t)hi<<16);}
static void setup(uint32_t value,uint8_t b,uint8_t c,uint8_t waiting) {
 memset(memory,0,sizeof memory);regb=b;regc=c;index_reg=0;
 callbacks=eois=0;cs=0x1234;ip=0x5678;
 sas_storew(RTC_LOW,(word)value);sas_storew(RTC_HIGH,(word)(value>>16));
 sas_storew(USER_FLAG_SEG,0x100);sas_storew(USER_FLAG,0x20);
 memory[0x1020]=0x35;memory[rtc_wait_flag]=waiting;
}
static void common(void){CHECK(regc==0);CHECK(eois==2 && ports[0]==0xa0 && ports[1]==0x20);CHECK(cs==0x1234 && ip==0x5678);}
int main(void){
 unsigned i;
 uint32_t values[]={0,1,49999,50000,50001,65535,65536,100000,0x12345678};
 CHECK(sizeof(word)==2 && sizeof(double_word)==4);
 setup(100000,0,PIE,1);rtc_int();common();CHECK(count()==100000 && memory[0x1020]==0x35 && callbacks==0);
 setup(100000,PIE,0,1);rtc_int();common();CHECK(count()==100000 && memory[0x1020]==0x35);
 for(i=0;i<sizeof(values)/sizeof(values[0]);i++) {
  uint32_t expected=values[i]-(uint32_t)TIME_DEC;
  setup(values[i],PIE,PIE,1);rtc_int();common();
  printf("count before=%08x observed=%08x expected=%08x\n",values[i],count(),expected);
  CHECK(count()==expected);
  if(values[i]<(uint32_t)TIME_DEC){CHECK(!(regb&PIE) && memory[0x1020]==0x80 && memory[rtc_wait_flag]==0);}
  else {CHECK((regb&PIE) && memory[0x1020]==0x35 && memory[rtc_wait_flag]==1);}
 }
 setup(1,PIE,PIE,2);rtc_int();common();CHECK(memory[rtc_wait_flag]==0x83);
 setup(123456,AIE,AIE,0);rtc_int();common();CHECK(callbacks==1 && count()==123456);
 setup(123456,PIE,AIE,0);rtc_int();common();CHECK(callbacks==0 && count()==123456);
 printf("RTC_BIOS checks=%u failures=%u\n",checks,failures);
 return failures?1:0;
}
'''
(out/'fixture.c').write_text(prefix+'\n#include "'+header.as_posix()+'"\n'+body+main,encoding='utf-8')
driver=repo/'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
with (out/'build.txt').open('w',encoding='utf-8') as log:
    subprocess.run([str(driver),'cl.exe','/nologo','/MT','/W4','/TC',str(out/'fixture.c'),
        '/Fo'+str(out/'fixture.obj'),'/Fe'+str(out/'fixture.exe')],cwd=repo,
        stdout=log,stderr=subprocess.STDOUT,timeout=90,check=True)
result=subprocess.run([str(out/'fixture.exe')],capture_output=True,text=True,timeout=15)
(out/'result.txt').write_text(result.stdout+result.stderr,encoding='utf-8')
sha=lambda path:hashlib.sha256(path.read_bytes()).hexdigest().upper()
(out/'result.json').write_text(json.dumps(dict(exit=result.returncode,
 identities={str(path):sha(path) for path in (source,header,out/'fixture.c',out/'fixture.exe')}),indent=2),encoding='utf-8')
print(result.stdout+result.stderr,end='')
raise SystemExit(result.returncode)
