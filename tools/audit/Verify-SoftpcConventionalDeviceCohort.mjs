import fs from 'node:fs';
import path from 'node:path';

const root = path.resolve(process.argv[2] ?? '.');
const read = (relativePath) => fs.readFileSync(path.join(root, relativePath), 'utf8');
const requireText = (text, fragment, context) => {
  if (!text.includes(fragment)) throw new Error(`${context}: missing ${fragment}`);
};
const requireAbsent = (text, fragment, context) => {
  if (text.includes(fragment)) throw new Error(`${context}: unexpectedly contains ${fragment}`);
};

const graph = read('build/M0-T362/S2/x86/build.ninja');
const reset = read('src/mvdm-host/softpc.new/base/bios/reset.c');
const ios = read('src/mvdm-host/softpc.new/base/support/ios.c');
const printer = read('src/mvdm-host/softpc.new/base/comms/printer.c');
const printerBios = read('src/mvdm-host/softpc.new/base/comms/printer_.c');
const lpt = read('src/mvdm-host/softpc.new/host/src/nt_lpt.c');

for (const source of [
  'ccpusas4.c', 'reset.c', 'ios.c', 'printer.c', 'printer_.c', 'nt_lpt.c'
]) requireText(graph, source, 'selected CPU40 graph');
requireText(graph, '/DNTVDM', 'selected CPU40 graph');
requireAbsent(graph, '/DMONITOR', 'selected CPU40 graph');

requireText(reset, 'printer_init(adapter);', 'reset initialization');
requireText(reset, 'printer_post(adapter);', 'reset initialization');
requireText(printer, 'sas_storew(port_address[adapter],(word)port_start[adapter]);', 'BDA port initialization');
requireText(printer, 'sas_store(timeout_address[adapter], (half_word)0x14 );', 'BDA timeout initialization');
requireText(printer, 'io_define_inb( lpt_adapter[adapter], printer_inb );', 'IOS input registration');
requireText(printer, 'io_define_outb( lpt_adapter[adapter], printer_outb );', 'IOS output registration');
requireText(printer, 'io_connect_port(i,lpt_adapter[adapter],IO_READ_WRITE);', 'IOS port registration');
requireText(ios, 'Ios_inb_function[Ios_in_adapter_table[io_address & (PC_IO_MEM_SIZE-1)]]', 'IOS input dispatch');
requireText(printerBios, 'case 0:', 'INT 17 print status direction');
requireText(printerBios, 'case 1:', 'INT 17 initialize status direction');
requireText(printerBios, 'case 2:', 'INT 17 status status direction');
requireText(printerBios, 'time_count = time_out * 0xFFFF;', 'INT 17 timeout direction');
requireText(printer, '!(host_lpt_status(adapter) & HOST_LPT_BUSY)', 'no-device/busy status direction');
requireText(lpt, '#define get_lpt_status(adap)\t\t(host_lpt[(adap)].port_status)', 'non-monitor host LPT state');

console.log('SoftPC conventional-device cohort source/graph verification passed.');
