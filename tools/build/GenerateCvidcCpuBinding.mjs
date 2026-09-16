/* Recover an omitted generated CCPU-to-C-VID binding from original metadata. */
import fs from 'node:fs';
import path from 'node:path';

const [output, sourceRoot] = process.argv.slice(2);
if (!output || !sourceRoot) throw new Error('usage: GenerateCvidcCpuBinding.mjs <output> <softpc-base-root>');
const metadata = fs.readFileSync(path.join(sourceRoot, 'cvidc', 'c2cpusad.h'), 'utf8');
const groups = [...metadata.matchAll(/char\s*\*\s*(\w+)\[\]\s*=\s*\{([\s\S]*?)\};/g)];
const slots = Object.fromEntries(groups.slice(0, 2).map(([, name, body]) => [
  name, [...body.matchAll(/"([A-Za-z0-9_]+)"/g)].map((match) => match[1]),
]));
const publicSpecial = {
  Simulate: 'c_cpu_simulate', Interrupt: 'c_cpu_interrupt', ClearHwInt: 'c_cpu_clearHwInt', EndOfApplication: 'c_cpu_EOA_hook', Terminate: 'c_cpu_terminate', Initialise: 'c_cpu_init',
  SetQuickEventCount: 'c_cpu_q_ev_set_count', GetQuickEventCount: 'c_cpu_q_ev_get_count', CalcQuickEventInstTime: 'c_cpu_calc_q_ev_inst_for_time', InitIOS: 'c_cpu_init_ios_in',
  DefineInb: 'c_cpu_define_inb', DefineInw: 'c_cpu_define_inw', DefineInd: 'c_cpu_define_ind', DefineOutb: 'c_cpu_define_outb', DefineOutw: 'c_cpu_define_outw', DefineOutd: 'c_cpu_define_outd',
  GetJumpCalibrateVal: 'mvdm_cvidc_get_jump_calibration', GetJumpInitialVal: 'mvdm_cvidc_get_jump_restart', SetJumpInitialVal: 'mvdm_cvidc_set_jump_restart', SetEOIEnable: 'setEOIEnableAddr',
  SetAddProfileData: 'setAddProfileDataPtr', SetMaxProfileData: 'setMaxProfileDataAddr', GetAddProfileDataAddr: 'getAddProfileDataAddr', PurgeLostIretHookLine: 'PurgeLostIretHookLine',
  ClearHwInt: '0', InitIOS: '0', DefineInb: '0', DefineInw: '0', DefineInd: '0', DefineOutb: '0', DefineOutw: '0', DefineOutd: '0',
  SetSTATUS: '0', SetAC: '0', GetAC: '0', GetET: '0', GetNE: '0', GetWP: '0',
};
const privateSpecial = {
  InitNanoCpu: 'c_InitNanoCpu', PrepareBlocksToCompile: 'c_PrepareBlocksToCompile',
  GrowRecPool: '0', BpiCompileBPI: '0', FmDeleteAllStructures: '0',
  GetSadInfoTable: '0', GetCpuState: '0', SetCpuState: '0', SetRegConstraint: '0', TrashIntelRegisters: '0',
  InitNanoCpu: '0', PrepareBlocksToCompile: '0', ConstraintsFromUnivEpcPtr: '0', ConstraintsFromUnivHandle: '0',
};
const provider = (name, special) => special[name] ?? `c_${name[0].toLowerCase()}${name.slice(1)}`;
const emit = (macro, names, special) => {
  const slash = String.fromCharCode(92);
  return `#define ${macro}(X) ${slash}\n${names.map((name, index) => `    X(${name}, ${provider(name, special)})${index + 1 === names.length ? '' : ` ${slash}\n`}`).join('')}\n`;
};
const videoHeader = fs.readFileSync(path.join(sourceRoot, 'cvidc', 'evidgen.h'), 'utf8');
const accessorSource = fs.readFileSync(path.join(sourceRoot, 'cpu', 'src', 'evid', 'vglob.c'), 'utf8');
const accessors = [...accessorSource.matchAll(/setVideo(\w+) IFN1\(([^,]+), value\)/g)];
if (accessors.length !== 38) throw new Error('Original video accessor profile changed');
const videoDeclarations = [], videoSlots = [], videoCases = [];
const setters = ['sinit011.c', 'sinit012.c', 'sinit013.c'].map(n => fs.readFileSync(path.join(sourceRoot, 'cvidc', n), 'utf8')).join('\n');
for (const [, name, rawType] of accessors) {
  const type = rawType.trim();
  const slotType = videoHeader.match(new RegExp('([\\w]+(?:\\s*\\*)?)\\s*\\(\\*GetVideo' + name + '\\)'))?.[1].trim();
  if (!slotType) throw new Error(`Missing original VideoVector slot: ${name}`);
  // Parentheses keep original evidgen function-like macros out of declarations.
  videoDeclarations.push(`extern ${type} (getVideo${name})(void);`, `extern void (setVideo${name})(${type});`);
  const same = type.replaceAll(' ', '') === slotType.replaceAll(' ', '');
  if (!same) {
    videoDeclarations.push(`static ${slotType} video_get_${name}(void) { return (${slotType})(getVideo${name})(); }`,
      `static void video_set_${name}(${slotType} value) { (setVideo${name})((${type})value); }`);
  }
  videoSlots.push(`    boundVideo.GetVideo${name} = ${same ? 'getVideo' + name : 'video_get_' + name};`,
    `    boundVideo.SetVideo${name} = ${same ? 'setVideo' + name : 'video_set_' + name};`);
  const body = setters.match(new RegExp('case\\s+S_\\d+_CiSetVideo' + name + '_\\w+_id\\s*:[\\s\\S]*?(?=case\\s|$)'))?.[0];
  const offset = body?.match(/&\(r20\)\)\s*=\s*\(IS32\)\((\d+)\)/)?.[1];
  if (!offset) throw new Error(`Missing independent generated GDP offset: ${name}`);
  videoCases.push(`CHECK_FIELD(${name}, ${slotType}, ${offset});`);
}
const text = [
  '/* Generated from original cvidc/c2cpusad.h. Do not edit. */\n',
  `/* public=${slots.CpuVectorNames.length}; private=${slots.CpuPrivateVectorNames.length} */\n`,
  emit('MVDM_CVIDC_CPU_PUBLIC_SLOTS', slots.CpuVectorNames, publicSpecial),
  emit('MVDM_CVIDC_CPU_PRIVATE_SLOTS', slots.CpuPrivateVectorNames, privateSpecial),
  '#ifdef MVDM_CVIDC_VIDEO_DECLARATIONS\n', videoDeclarations.join('\n'), '\n#endif\n',
  '#ifdef MVDM_CVIDC_VIDEO_BIND\n', videoSlots.join('\n'), '\n#endif\n',
].join('');
fs.mkdirSync(path.dirname(output), { recursive: true });
if (!fs.existsSync(output) || fs.readFileSync(output, 'utf8') !== text) fs.writeFileSync(output, text);
const casesOutput = path.join(path.dirname(output), 'cvidc_accessor_cases.inc');
const casesText = '/* Test offsets extracted from original generated setters, not vglob field layout. */\n' + videoCases.join('\n') + '\n';
if (!fs.existsSync(casesOutput) || fs.readFileSync(casesOutput, 'utf8') !== casesText) fs.writeFileSync(casesOutput, casesText);
