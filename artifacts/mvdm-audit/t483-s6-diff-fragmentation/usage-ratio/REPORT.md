# 镜像有差异文件：原版接入占比扫描

基线 `956d1ce30e18b83367c17a9fffca2afdcfb7bd97`。254个文件；806个当前产品编译上下文全部预处理成功。无产品源码、guest或发布包修改。

## 口径

分母是固定原版文件的物理LF总行数。分子是可证实保留的原版函数范围行（含函数内注释、空行），加函数外已输出的原版声明／数据行；不计本地新增适配行、条件骨架、#error/#line。宏定义另列，不把未归因宏误报为无用。因此这是可证实接入规模，下限性质；不是精确运行覆盖率，也不是自动删除依据。

C实现和头文件分组排列；头文件的声明行是下限，不能与完整函数行数直接比较。另有3个构建／导出描述文件不适用C预处理占比。所有逐行及函数范围见 evidence.zip 内的 line-evidence.json，所有上下文见 contexts.jsonl。CSV提供逐项数据；源码与对象哈希均已核验。

## C implementation

| 文件 | 可证实接入行（下限） | 原版总行 | 下限占比 | 函数数 | 非-error点 | +行 | −行 | diff行 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| `src/non-mvdm/windows/core/ntuser/client/wow.c` | 15 | 1076 | 1.394% | 1 | 21 | 0 | 832 | 832 |
| `src/non-mvdm/windows/core/ntuser/kernel/input.c` | 102 | 5971 | 1.708% | 2 | 25 | 5 | 5498 | 5503 |
| `src/non-mvdm/windows/core/ntuser/kernel/handtabl.c` | 55 | 2157 | 2.550% | 1 | 24 | 54 | 1327 | 1381 |
| `src/non-mvdm/windows/core/ntuser/client/client.c` | 124 | 3750 | 3.307% | 8 | 8 | 25 | 3539 | 3564 |
| `src/non-mvdm/base/ntos/rtl/time.c` | 55 | 1332 | 4.129% | 1 | 6 | 0 | 1216 | 1216 |
| `src/mvdm-host/softpc.new/base/support/xt.c` | 28 | 652 | 4.294% | 1 | 4 | 8 | 331 | 339 |
| `src/non-mvdm/windows/core/ntuser/rtl/rect.c` | 22 | 421 | 5.226% | 2 | 3 | 1 | 367 | 368 |
| `src/non-mvdm/windows/core/ntuser/kernel/miscutil.c` | 14 | 229 | 6.114% | 1 | 3 | 0 | 198 | 198 |
| `src/non-mvdm/windows/core/ntuser/kernel/inctlpan.c` | 71 | 1155 | 6.147% | 0 | 4 | 1 | 880 | 881 |
| `src/non-mvdm/windows/core/ntuser/kernel/queue.c` | 355 | 4796 | 7.402% | 4 | 35 | 5 | 4168 | 4173 |
| `src/non-mvdm/windows/core/ntuser/client/handles.c` | 27 | 353 | 7.649% | 1 | 5 | 1 | 278 | 279 |
| `src/non-mvdm/windows/core/ntuser/kernel/random.c` | 36 | 440 | 8.182% | 2 | 6 | 2 | 359 | 361 |
| `src/non-mvdm/windows/core/ntuser/kernel/exitwin.c` | 48 | 557 | 8.618% | 1 | 5 | 14 | 480 | 494 |
| `src/non-mvdm/windows/core/ntuser/client/ntstubs.c` | 236 | 2261 | 10.438% | 5 | 35 | 25 | 1778 | 1803 |
| `src/non-mvdm/windows/core/ntuser/kernel/class.c` | 173 | 1627 | 10.633% | 4 | 12 | 16 | 1374 | 1390 |
| `src/non-mvdm/ds/netapi/api/apibuff.c` | 28 | 258 | 10.853% | 1 | 4 | 0 | 174 | 174 |
| `src/mvdm-host/softpc.new/host/src/nt_cga.c` | 131 | 1193 | 10.981% | 4 | 7 | 13 | 36 | 49 |
| `src/non-mvdm/windows/core/ntuser/client/winmgrc.c` | 70 | 601 | 11.647% | 3 | 6 | 4 | 496 | 500 |
| `src/non-mvdm/windows/core/ntuser/kernel/getset.c` | 116 | 860 | 13.488% | 1 | 10 | 5 | 673 | 678 |
| `src/mvdm-host/softpc.new/base/system/illegalp.c` | 55 | 395 | 13.924% | 4 | 1 | 1 | 0 | 1 |
| `src/non-mvdm/windows/core/ntuser/kernel/profile.c` | 168 | 1095 | 15.342% | 3 | 7 | 2 | 864 | 866 |
| `src/non-mvdm/ds/netapi/netlib/packstr.c` | 126 | 668 | 18.862% | 1 | 5 | 0 | 478 | 478 |
| `src/non-mvdm/windows/core/ntuser/client/clmsg.c` | 364 | 1922 | 18.939% | 3 | 27 | 6 | 973 | 979 |
| `src/non-mvdm/windows/core/ntuser/kernel/server.c` | 534 | 2351 | 22.714% | 3 | 20 | 2 | 1533 | 1535 |
| `src/mvdm-host/dpmi32/data.c` | 32 | 140 | 22.857% | 0 | 1 | 47 | 0 | 47 |
| `src/mvdm-host/softpc.new/base/system/idetect.c` | 117 | 505 | 23.168% | 2 | 1 | 0 | 1 | 1 |
| `src/mvdm-host/softpc.new/host/src/nt_fulsc.c` | 585 | 2415 | 24.224% | 17 | 14 | 47 | 21 | 68 |
| `src/non-mvdm/base/ntos/ex/probe.c` | 48 | 194 | 24.742% | 1 | 4 | 2 | 97 | 99 |
| `src/non-mvdm/base/ntdll/csrutil.c` | 226 | 867 | 26.067% | 3 | 7 | 7 | 615 | 622 |
| `src/mvdm-host/dpmi32/i386/dpmi386.c` | 116 | 386 | 30.052% | 3 | 15 | 33 | 232 | 265 |
| `src/non-mvdm/windows/core/ntuser/client/hdata.c` | 215 | 712 | 30.197% | 4 | 2 | 1 | 441 | 442 |
| `src/non-mvdm/windows/core/ntuser/client/clres.c` | 1401 | 4597 | 30.476% | 28 | 30 | 27 | 2706 | 2733 |
| `src/mvdm-host/softpc.new/base/ccpu386/localfm.c` | 3 | 9 | 33.333% | 0 | 2 | 2 | 1 | 3 |
| `src/non-mvdm/windows/core/ntuser/rtl/getset.c` | 134 | 386 | 34.715% | 1 | 8 | 14 | 223 | 237 |
| `src/mvdm-host/softpc.new/base/system/ica.c` | 798 | 2244 | 35.561% | 15 | 3 | 13 | 0 | 13 |
| `src/non-mvdm/windows/core/ntuser/server/exports.c` | 66 | 182 | 36.264% | 2 | 2 | 1 | 85 | 86 |
| `src/mvdm-host/softpc.new/host/src/nt_cpu.c` | 126 | 344 | 36.628% | 10 | 7 | 13 | 3 | 16 |
| `src/mvdm-host/softpc.new/base/video/ega_read.c` | 369 | 1002 | 36.826% | 11 | 2 | 4 | 3 | 7 |
| `src/mvdm-host/softpc.new/base/support/main.c` | 125 | 339 | 36.873% | 2 | 2 | 3 | 0 | 3 |
| `src/mvdm-host/softpc.new/base/support/terminat.c` | 53 | 143 | 37.063% | 1 | 2 | 2 | 0 | 2 |
| `src/non-mvdm/windows/core/ntuser/rtl/winmgr.c` | 273 | 712 | 38.343% | 11 | 6 | 11 | 301 | 312 |
| `src/mvdm-host/softpc.new/base/ccpu386/lmsw.c` | 22 | 56 | 39.286% | 1 | 2 | 2 | 0 | 2 |
| `src/mvdm-host/wow32/wudlg.c` | 586 | 1489 | 39.355% | 22 | 8 | 17 | 2 | 19 |
| `src/mvdm-host/softpc.new/base/video/egwrtm12.c` | 483 | 1218 | 39.655% | 9 | 14 | 21 | 438 | 459 |
| `src/mvdm-host/wow32/wuser.c` | 1434 | 3555 | 40.338% | 63 | 3 | 11 | 2 | 13 |
| `src/non-mvdm/windows/core/ntuser/rtl/drawtext.c` | 805 | 1974 | 40.780% | 15 | 8 | 2 | 245 | 247 |
| `src/non-mvdm/windows/core/ntuser/rtl/text.c` | 20 | 49 | 40.816% | 1 | 4 | 11 | 3 | 14 |
| `src/non-mvdm/windows/core/ntuser/rtl/wow.c` | 348 | 844 | 41.232% | 12 | 14 | 2 | 183 | 185 |
| `src/mvdm-host/softpc.new/base/bios/reset.c` | 464 | 1118 | 41.503% | 4 | 2 | 6 | 2 | 8 |
| `src/mvdm-host/softpc.new/base/support/ios.c` | 430 | 1021 | 42.116% | 35 | 1 | 32 | 0 | 32 |
| `src/non-mvdm/windows/core/ntuser/client/clmenu.c` | 321 | 754 | 42.573% | 6 | 2 | 2 | 7 | 9 |
| `src/mvdm-host/softpc.new/host/src/nt_reset.c` | 181 | 422 | 42.891% | 5 | 5 | 10 | 0 | 10 |
| `src/mvdm-host/softpc.new/base/video/ega_writ.c` | 372 | 861 | 43.206% | 5 | 2 | 2 | 1 | 3 |
| `src/mvdm-host/softpc.new/base/video/vga_mode.c` | 253 | 575 | 44.000% | 2 | 3 | 3 | 3 | 6 |
| `src/mvdm-host/softpc.new/base/bios/slave_bs.c` | 75 | 170 | 44.118% | 3 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/softpc.new/base/video/egawrtm0.c` | 619 | 1366 | 45.315% | 9 | 8 | 15 | 411 | 426 |
| `src/mvdm-host/softpc.new/host/src/nt_eoi.c` | 247 | 545 | 45.321% | 17 | 2 | 5 | 5 | 10 |
| `src/mvdm-host/wow32/wuwind.c` | 1406 | 3092 | 45.472% | 40 | 12 | 49 | 11 | 60 |
| `src/non-mvdm/windows/core/ntuser/rtl/random.c` | 92 | 197 | 46.701% | 5 | 1 | 4 | 0 | 4 |
| `src/mvdm-host/wow32/wumsg.c` | 1040 | 2225 | 46.742% | 25 | 32 | 95 | 14 | 109 |
| `src/mvdm-host/softpc.new/host/src/sim32.c` | 216 | 462 | 46.753% | 14 | 10 | 15 | 6 | 21 |
| `src/mvdm-host/softpc.new/base/cvidc/ev_glue.c` | 379 | 803 | 47.198% | 21 | 4 | 25 | 0 | 25 |
| `src/mvdm-host/softpc.new/host/src/nt_timer.c` | 477 | 982 | 48.574% | 17 | 1 | 1 | 0 | 1 |
| `src/mvdm-host/softpc.new/host/src/nt_rez.c` | 91 | 187 | 48.663% | 1 | 1 | 3 | 0 | 3 |
| `src/mvdm-host/softpc.new/host/src/nt_emm.c` | 410 | 817 | 50.184% | 20 | 7 | 61 | 8 | 69 |
| `src/mvdm-host/xms.486/xmsblock.c` | 125 | 247 | 50.607% | 5 | 5 | 19 | 13 | 32 |
| `src/mvdm-host/softpc.new/base/dos/emm_mngr.c` | 1111 | 2129 | 52.184% | 46 | 1 | 190 | 0 | 190 |
| `src/mvdm-host/dos/dem/dem.c` | 40 | 76 | 52.632% | 1 | 2 | 2 | 1 | 3 |
| `src/mvdm-host/softpc.new/host/src/config.c` | 681 | 1293 | 52.668% | 18 | 1 | 1 | 0 | 1 |
| `src/mvdm-host/wow32/wow32.c` | 1253 | 2376 | 52.736% | 23 | 15 | 31 | 13 | 44 |
| `src/non-mvdm/windows/core/ntuser/rtl/help.c` | 37 | 70 | 52.857% | 3 | 2 | 4 | 2 | 6 |
| `src/non-mvdm/windows/core/ntuser/client/drawtext.c` | 242 | 450 | 53.778% | 5 | 5 | 0 | 137 | 137 |
| `src/mvdm-host/softpc.new/base/bios/virtual.c` | 460 | 844 | 54.502% | 15 | 5 | 22 | 0 | 22 |
| `src/mvdm-host/softpc.new/base/video/v7_video.c` | 289 | 524 | 55.153% | 8 | 2 | 4 | 1 | 5 |
| `src/mvdm-host/softpc.new/host/src/nt_hosts.c` | 190 | 343 | 55.394% | 4 | 1 | 1 | 0 | 1 |
| `src/mvdm-host/softpc.new/host/src/nt_bop.c` | 533 | 960 | 55.521% | 23 | 11 | 19 | 6 | 25 |
| `src/non-mvdm/windows/core/ntuser/rtl/chartran.c` | 337 | 605 | 55.702% | 6 | 2 | 2 | 1 | 3 |
| `src/mvdm-host/softpc.new/base/keymouse/keyba.c` | 1814 | 3241 | 55.970% | 32 | 24 | 49 | 7 | 56 |
| `src/non-mvdm/windows/core/ntuser/rtl/draw.c` | 697 | 1231 | 56.621% | 13 | 5 | 1 | 388 | 389 |
| `src/mvdm-host/softpc.new/base/keymouse/keybd_io.c` | 1265 | 2162 | 58.511% | 22 | 3 | 5 | 4 | 9 |
| `src/mvdm-host/softpc.new/base/bios/tape_io.c` | 191 | 325 | 58.769% | 2 | 1 | 1 | 0 | 1 |
| `src/mvdm-host/softpc.new/host/src/nt_mem.c` | 858 | 1456 | 58.929% | 16 | 3 | 8 | 0 | 8 |
| `src/mvdm-host/wow32/wuclip.c` | 739 | 1245 | 59.357% | 21 | 16 | 63 | 27 | 90 |
| `src/mvdm-host/softpc.new/host/src/nt_unix.c` | 273 | 459 | 59.477% | 12 | 1 | 0 | 1 | 1 |
| `src/non-mvdm/windows/core/ntuser/client/rtlinit.c` | 220 | 368 | 59.783% | 5 | 2 | 2 | 2 | 4 |
| `src/mvdm-host/wow32/wparam.c` | 399 | 666 | 59.910% | 12 | 3 | 14 | 1 | 15 |
| `src/mvdm-host/softpc.new/base/video/ega_prts.c` | 2093 | 3459 | 60.509% | 37 | 2 | 2 | 0 | 2 |
| `src/mvdm-host/dos/dem/demhndl.c` | 367 | 605 | 60.661% | 8 | 12 | 32 | 14 | 46 |
| `src/mvdm-host/softpc.new/host/src/nt_mouse.c` | 1602 | 2630 | 60.913% | 45 | 6 | 24 | 1 | 25 |
| `src/mvdm-host/wow32/wmsgcb.c` | 122 | 198 | 61.616% | 2 | 5 | 11 | 9 | 20 |
| `src/mvdm-host/dpmi32/modesw.c` | 74 | 120 | 61.667% | 2 | 6 | 28 | 6 | 34 |
| `src/mvdm-host/softpc.new/base/ccpu386/ccpusas4.c` | 1217 | 1960 | 62.092% | 65 | 13 | 17 | 10 | 27 |
| `src/non-mvdm/windows/core/ntuser/client/dlgmgrc.c` | 285 | 450 | 63.333% | 8 | 2 | 0 | 75 | 75 |
| `src/mvdm-host/softpc.new/base/video/vga_prts.c` | 2107 | 3315 | 63.560% | 41 | 2 | 4 | 2 | 6 |
| `src/mvdm-host/dbg/dbg.c` | 1085 | 1701 | 63.786% | 19 | 1 | 1 | 1 | 2 |
| `src/non-mvdm/windows/core/ntuser/client/dlgend.c` | 111 | 174 | 63.793% | 1 | 1 | 2 | 0 | 2 |
| `src/mvdm-host/softpc.new/base/ccpu386/c_xcptn.c` | 412 | 641 | 64.275% | 21 | 13 | 16 | 6 | 22 |
| `src/mvdm-host/softpc.new/host/src/nt_msscs.c` | 918 | 1416 | 64.831% | 23 | 6 | 16 | 58 | 74 |
| `src/mvdm-host/softpc.new/host/src/nt_rflop.c` | 1323 | 2036 | 64.980% | 32 | 4 | 4 | 3 | 7 |
| `src/mvdm-host/softpc.new/host/src/nt_vga.c` | 1430 | 2188 | 65.356% | 25 | 2 | 8 | 1 | 9 |
| `src/mvdm-host/wow32/wuclass.c` | 692 | 1052 | 65.779% | 7 | 7 | 41 | 7 | 48 |
| `src/mvdm-host/softpc.new/base/video/gfx_updt.c` | 3209 | 4845 | 66.233% | 66 | 58 | 69 | 69 | 138 |
| `src/mvdm-host/wow32/wcall16.c` | 545 | 819 | 66.545% | 24 | 8 | 24 | 5 | 29 |
| `src/mvdm-host/softpc.new/base/video/cga.c` | 1000 | 1490 | 67.114% | 6 | 1 | 2 | 0 | 2 |
| `src/mvdm-host/wow32/wdos.c` | 555 | 823 | 67.436% | 14 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/softpc.new/base/video/ega_vide.c` | 1598 | 2356 | 67.827% | 27 | 1 | 12 | 0 | 12 |
| `src/mvdm-host/softpc.new/host/src/copy_fnc.c` | 133 | 195 | 68.205% | 5 | 3 | 3 | 3 | 6 |
| `src/mvdm-host/wow32/wmsglb.c` | 166 | 243 | 68.313% | 2 | 3 | 3 | 1 | 4 |
| `src/mvdm-host/softpc.new/base/ccpu386/c_debug.c` | 264 | 386 | 68.394% | 6 | 3 | 3 | 0 | 3 |
| `src/mvdm-host/softpc.new/base/video/v7_ports.c` | 534 | 771 | 69.261% | 4 | 2 | 2 | 2 | 4 |
| `src/mvdm-host/softpc.new/host/src/nt_graph.c` | 1609 | 2314 | 69.533% | 46 | 20 | 74 | 11 | 85 |
| `src/mvdm-host/oemuni/process.c` | 637 | 916 | 69.541% | 9 | 47 | 119 | 48 | 167 |
| `src/mvdm-host/vdmredir/vrnmpipe.c` | 2381 | 3412 | 69.783% | 32 | 9 | 22 | 29 | 51 |
| `src/mvdm-host/dos/dem/demgset.c` | 708 | 1013 | 69.891% | 18 | 3 | 6 | 1 | 7 |
| `src/mvdm-host/softpc.new/base/keymouse/mouse_io.c` | 4728 | 6664 | 70.948% | 95 | 33 | 319 | 30 | 349 |
| `src/mvdm-host/wow32/wutmr.c` | 487 | 686 | 70.991% | 36 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/softpc.new/host/src/nt_error.c` | 847 | 1190 | 71.176% | 19 | 2 | 2 | 2 | 4 |
| `src/mvdm-host/softpc.new/obj.vdm/ntvdm.c` | 253 | 354 | 71.469% | 5 | 4 | 11 | 2 | 13 |
| `src/mvdm-host/softpc.new/base/ccpu386/c_tsksw.c` | 423 | 589 | 71.817% | 3 | 2 | 2 | 0 | 2 |
| `src/mvdm-host/wow32/wmdisp32.c` | 1874 | 2602 | 72.022% | 55 | 75 | 130 | 94 | 224 |
| `src/mvdm-host/wow32/wcntl32.c` | 808 | 1116 | 72.401% | 33 | 34 | 37 | 32 | 69 |
| `src/mvdm-host/softpc.new/base/video/video.c` | 2079 | 2861 | 72.667% | 34 | 16 | 55 | 15 | 70 |
| `src/mvdm-host/wow32/wuman.c` | 210 | 287 | 73.171% | 6 | 4 | 3 | 3 | 6 |
| `src/non-mvdm/windows/core/ntuser/client/dlgmgr.c` | 998 | 1361 | 73.328% | 13 | 17 | 37 | 205 | 242 |
| `src/mvdm-host/softpc.new/base/ccpu386/call.c` | 264 | 357 | 73.950% | 3 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/wow32/wmsgem.c` | 213 | 288 | 73.958% | 2 | 4 | 4 | 1 | 5 |
| `src/mvdm-host/softpc.new/host/src/nt_event.c` | 1422 | 1918 | 74.140% | 34 | 43 | 82 | 39 | 121 |
| `src/mvdm-host/wow32/wkman.c` | 3708 | 4970 | 74.608% | 58 | 25 | 32 | 13 | 45 |
| `src/mvdm-host/dpmi32/dpmiselr.c` | 233 | 312 | 74.679% | 3 | 2 | 13 | 0 | 13 |
| `src/non-mvdm/base/win32/client/vdm.c` | 2463 | 3275 | 75.206% | 20 | 20 | 38 | 594 | 632 |
| `src/mvdm-host/softpc.new/base/ccpu386/c_main.c` | 3721 | 4940 | 75.324% | 21 | 36 | 128 | 30 | 158 |
| `src/non-mvdm/windows/core/ntuser/kernel/cleanup.c` | 352 | 457 | 77.024% | 5 | 20 | 24 | 20 | 44 |
| `src/mvdm-host/vdmredir/vrnetapi.c` | 3248 | 4213 | 77.095% | 27 | 15 | 35 | 20 | 55 |
| `src/non-mvdm/windows/core/ntuser/client/cldib.c` | 504 | 646 | 78.019% | 11 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/wow32/wowhooks.c` | 1561 | 1997 | 78.167% | 63 | 38 | 93 | 21 | 114 |
| `src/mvdm-host/dpmi32/dpmimemr.c` | 218 | 277 | 78.700% | 4 | 6 | 4 | 19 | 23 |
| `src/mvdm-host/dos/command/cmdkeyb.c` | 199 | 251 | 79.283% | 1 | 2 | 3 | 1 | 4 |
| `src/mvdm-host/dos/dem/demsrch.c` | 1937 | 2442 | 79.320% | 30 | 8 | 28 | 0 | 28 |
| `src/mvdm-host/wow32/wole2.c` | 296 | 371 | 79.784% | 6 | 13 | 27 | 10 | 37 |
| `src/mvdm-host/softpc.new/host/src/nt_pif.c` | 353 | 442 | 79.864% | 4 | 5 | 8 | 6 | 14 |
| `src/mvdm-host/softpc.new/base/ccpu386/ret.c` | 216 | 270 | 80.000% | 2 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/softpc.new/host/src/nt_com.c` | 1769 | 2206 | 80.190% | 31 | 2 | 2 | 2 | 4 |
| `src/mvdm-host/dpmi32/xmem.c` | 268 | 333 | 80.480% | 5 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/wow32/wkernel.c` | 658 | 816 | 80.637% | 14 | 11 | 25 | 5 | 30 |
| `src/mvdm-host/softpc.new/base/ccpu386/fpu.c` | 4798 | 5948 | 80.666% | 125 | 1 | 2 | 1 | 3 |
| `src/mvdm-host/softpc.new/base/ccpu386/c_seg.c` | 321 | 396 | 81.061% | 6 | 1 | 8 | 0 | 8 |
| `src/mvdm-host/softpc.new/base/ccpu386/iret.c` | 269 | 330 | 81.515% | 2 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/dos/command/cmdmisc.c` | 732 | 897 | 81.605% | 12 | 10 | 52 | 7 | 59 |
| `src/mvdm-host/wow32/wstruc.c` | 1374 | 1681 | 81.737% | 43 | 3 | 3 | 2 | 5 |
| `src/non-mvdm/windows/core/ntuser/kernel/taskman.c` | 609 | 744 | 81.855% | 8 | 17 | 24 | 21 | 45 |
| `src/mvdm-host/dpmi32/dpmi32.c` | 244 | 297 | 82.155% | 5 | 2 | 34 | 0 | 34 |
| `src/mvdm-host/dos/command/cmdconf.c` | 523 | 636 | 82.233% | 7 | 2 | 8 | 1 | 9 |
| `src/non-mvdm/windows/core/ntuser/client/dmmnem.c` | 293 | 356 | 82.303% | 3 | 5 | 11 | 1 | 12 |
| `src/mvdm-host/softpc.new/host/src/nt_keycd.c` | 498 | 605 | 82.314% | 2 | 2 | 2 | 2 | 4 |
| `src/non-mvdm/windows/core/ntuser/client/clglobal.c` | 1084 | 1316 | 82.371% | 0 | 3 | 3 | 20 | 23 |
| `src/non-mvdm/windows/core/ntuser/client/editec.c` | 2402 | 2916 | 82.373% | 34 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/wow32/wkfileio.c` | 1837 | 2214 | 82.972% | 30 | 5 | 6 | 4 | 10 |
| `src/mvdm-host/wow32/wgprnset.c` | 477 | 574 | 83.101% | 4 | 3 | 7 | 0 | 7 |
| `src/non-mvdm/ds/netapi/xactsrv/apisrv.c` | 1159 | 1393 | 83.202% | 5 | 2 | 2 | 182 | 184 |
| `src/mvdm-host/dpmi32/dpmiint.c` | 1152 | 1383 | 83.297% | 24 | 34 | 184 | 37 | 221 |
| `src/mvdm-host/wow32/wcommdlg.c` | 2081 | 2489 | 83.608% | 28 | 11 | 16 | 6 | 22 |
| `src/mvdm-host/xms.486/xmsmemr.c` | 140 | 167 | 83.832% | 3 | 2 | 4 | 0 | 4 |
| `src/mvdm-host/wow32/wgpal.c` | 246 | 292 | 84.247% | 12 | 1 | 1 | 1 | 2 |
| `src/non-mvdm/windows/core/ntuser/client/mngrayc.c` | 497 | 588 | 84.524% | 6 | 8 | 14 | 1 | 15 |
| `src/mvdm-host/wow32/wdde.c` | 972 | 1138 | 85.413% | 29 | 26 | 128 | 15 | 143 |
| `src/non-mvdm/ds/netapi/xactsrv/xsunicod.c` | 229 | 268 | 85.448% | 5 | 1 | 1 | 1 | 2 |
| `src/non-mvdm/windows/core/ntuser/client/dlgmgr2.c` | 578 | 673 | 85.884% | 5 | 15 | 25 | 0 | 25 |
| `src/mvdm-host/wow32/wgdi31.c` | 950 | 1105 | 85.973% | 41 | 1 | 3 | 1 | 4 |
| `src/mvdm-host/dos/command/cmdexec.c` | 560 | 650 | 86.154% | 7 | 17 | 67 | 15 | 82 |
| `src/mvdm-host/wow32/wspool.c` | 282 | 326 | 86.503% | 11 | 3 | 5 | 1 | 6 |
| `src/non-mvdm/windows/core/ntuser/client/lboxrare.c` | 703 | 810 | 86.790% | 10 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/softpc.new/base/ccpu386/ntthread.c` | 220 | 253 | 86.957% | 10 | 6 | 32 | 0 | 32 |
| `src/mvdm-host/wow32/wow32fax.c` | 926 | 1063 | 87.112% | 16 | 8 | 15 | 5 | 20 |
| `src/mvdm-host/wow32/wsext.c` | 779 | 893 | 87.234% | 15 | 4 | 7 | 1 | 8 |
| `src/non-mvdm/windows/core/ntuser/client/btnctl.c` | 1621 | 1856 | 87.338% | 21 | 12 | 12 | 1 | 13 |
| `src/mvdm-host/wow32/wkgthunk.c` | 511 | 583 | 87.650% | 8 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/wow32/wmsg16.c` | 1372 | 1534 | 89.439% | 14 | 37 | 111 | 35 | 146 |
| `src/mvdm-host/wow32/wdib.c` | 769 | 859 | 89.523% | 14 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/wow32/wuser31.c` | 834 | 931 | 89.581% | 19 | 3 | 3 | 2 | 5 |
| `src/mvdm-host/wow32/wgdi.c` | 4068 | 4496 | 90.480% | 131 | 1 | 1 | 0 | 1 |
| `src/mvdm-host/vdd/samples/mscdex/vdd/vcdex.c` | 1337 | 1474 | 90.706% | 26 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/vdmredir/vrremote.c` | 1396 | 1531 | 91.182% | 8 | 2 | 10 | 2 | 12 |
| `src/mvdm-host/vdmredir/vrdlc5c.c` | 3587 | 3893 | 92.140% | 26 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/oemuni/file.c` | 1051 | 1138 | 92.355% | 20 | 12 | 26 | 11 | 37 |
| `src/mvdm-host/wow32/wsdata.c` | 1733 | 1875 | 92.427% | 27 | 20 | 161 | 13 | 174 |
| `src/mvdm-host/dpmi32/int21map.c` | 2878 | 3111 | 92.510% | 54 | 16 | 29 | 6 | 35 |
| `src/non-mvdm/ds/netapi/xactsrv/apimsg.c` | 610 | 659 | 92.564% | 5 | 1 | 1 | 1 | 2 |
| `src/non-mvdm/windows/core/ntuser/client/dlgbegin.c` | 884 | 951 | 92.955% | 5 | 10 | 33 | 7 | 40 |
| `src/non-mvdm/base/win32/server/srvvdm.c` | 2723 | 2920 | 93.253% | 55 | 10 | 46 | 8 | 54 |
| `src/non-mvdm/ds/netapi/xactsrv/apiuse.c` | 696 | 739 | 94.181% | 5 | 1 | 1 | 1 | 2 |
| `src/non-mvdm/ds/netapi/xactsrv/apisvc.c` | 967 | 1021 | 94.711% | 4 | 1 | 1 | 1 | 2 |
| `src/non-mvdm/ds/netapi/xactsrv/apiwksta.c` | 971 | 1017 | 95.477% | 3 | 1 | 1 | 1 | 2 |
| `src/non-mvdm/ds/netapi/xactsrv/xssubs.c` | 1925 | 1979 | 97.271% | 13 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/softpc.new/base/cvidc/sevid000.c` | 3066 | 3148 | 97.395% | 114 | 64 | 66 | 66 | 132 |
| `src/mvdm-host/softpc.new/base/cvidc/sevid001.c` | 3457 | 3498 | 98.828% | 171 | 25 | 25 | 25 | 50 |
| `src/mvdm-host/softpc.new/base/cvidc/sevid017.c` | 3327 | 3361 | 98.988% | 129 | 11 | 18 | 18 | 36 |
| `src/mvdm-host/softpc.new/base/cvidc/sevid018.c` | 3549 | 3581 | 99.106% | 139 | 9 | 16 | 16 | 32 |
| `src/mvdm-host/softpc.new/base/cvidc/sevid016.c` | 4508 | 4530 | 99.514% | 167 | 4 | 6 | 6 | 12 |
| `src/mvdm-host/softpc.new/base/cvidc/sevid019.c` | 4572 | 4591 | 99.586% | 187 | 3 | 3 | 3 | 6 |
| `src/mvdm-host/softpc.new/base/cvidc/sevid004.c` | 5160 | 5179 | 99.633% | 223 | 3 | 3 | 3 | 6 |
## header declarations

| 文件 | 可证实接入行（下限） | 原版总行 | 下限占比 | 函数数 | 非-error点 | +行 | −行 | diff行 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| `src/mvdm-host/softpc.new/base/ccpu386/c_oprnd.h` | 0 | 1135 | 0.000% | 0 | 9 | 18 | 8 | 26 |
| `src/mvdm-host/vdmredir/vrica.h` | 0 | 24 | 0.000% | 0 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/wow32/precomp.h` | 0 | 116 | 0.000% | 0 | 3 | 13 | 1 | 14 |
| `src/non-mvdm/windows/core/ntuser/rtl/userrtl.h` | 9 | 247 | 3.644% | 0 | 12 | 7 | 41 | 48 |
| `src/mvdm-host/softpc.new/base/inc/ica.h` | 17 | 255 | 6.667% | 0 | 2 | 4 | 14 | 18 |
| `src/mvdm-host/inc/mvdm.h` | 9 | 91 | 9.890% | 0 | 2 | 4 | 0 | 4 |
| `src/mvdm-host/inc/dbgexp.h` | 2 | 19 | 10.526% | 0 | 1 | 3 | 3 | 6 |
| `src/mvdm-host/softpc.new/base/ccpu386/c_bsic.h` | 16 | 138 | 11.594% | 0 | 1 | 1 | 0 | 1 |
| `src/mvdm-host/inc/demexp.h` | 5 | 37 | 13.514% | 0 | 1 | 3 | 3 | 6 |
| `src/mvdm-host/softpc.new/base/inc/mouse_io.h` | 172 | 1145 | 15.022% | 0 | 3 | 5 | 0 | 5 |
| `src/mvdm-host/softpc.new/base/inc/timeval.h` | 10 | 66 | 15.152% | 0 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/softpc.new/host/inc/cfpu_def.h` | 33 | 215 | 15.349% | 0 | 14 | 36 | 12 | 48 |
| `src/non-mvdm/public/sdk/inc/ntexapi.h` | 324 | 1846 | 17.551% | 0 | 19 | 0 | 952 | 952 |
| `src/mvdm-host/softpc.new/base/ccpu386/c_debug.h` | 8 | 45 | 17.778% | 0 | 1 | 1 | 0 | 1 |
| `src/mvdm-host/inc/softpc.h` | 122 | 651 | 18.740% | 0 | 11 | 35 | 35 | 70 |
| `src/non-mvdm/windows/core/ntuser/inc/ntcftxt.h` | 297 | 1583 | 18.762% | 7 | 62 | 2 | 1055 | 1057 |
| `src/mvdm-host/softpc.new/base/inc/gvi.h` | 59 | 311 | 18.971% | 0 | 1 | 1 | 0 | 1 |
| `src/mvdm-host/wow32/walias.h` | 84 | 416 | 20.192% | 0 | 6 | 11 | 4 | 15 |
| `src/mvdm-host/softpc.new/host/inc/host_cpu.h` | 120 | 506 | 23.715% | 0 | 3 | 5 | 2 | 7 |
| `src/mvdm-host/softpc.new/host/inc/host_def.h` | 54 | 226 | 23.894% | 0 | 3 | 5 | 2 | 7 |
| `src/non-mvdm/windows/core/ntuser/inc/user.h` | 639 | 2640 | 24.205% | 3 | 45 | 93 | 17 | 110 |
| `src/mvdm-host/wow32/isvwow.h` | 22 | 85 | 25.882% | 0 | 1 | 1 | 1 | 2 |
| `src/non-mvdm/public/sdk/inc/ntpsapi.h` | 370 | 1293 | 28.616% | 0 | 38 | 1 | 309 | 310 |
| `src/mvdm-host/softpc.new/host/inc/nt_event.h` | 19 | 59 | 32.203% | 0 | 1 | 0 | 1 | 1 |
| `src/mvdm-host/softpc.new/base/inc/gmi.h` | 28 | 84 | 33.333% | 0 | 2 | 4 | 0 | 4 |
| `src/non-mvdm/windows/core/ntuser/client/globals.h` | 51 | 144 | 35.417% | 0 | 4 | 0 | 5 | 5 |
| `src/non-mvdm/windows/core/ntuser/client/usercli.h` | 964 | 2671 | 36.091% | 0 | 45 | 75 | 39 | 114 |
| `src/non-mvdm/windows/core/ntuser/kernel/userk.h` | 2009 | 5217 | 38.509% | 7 | 27 | 5 | 110 | 115 |
| `src/mvdm-host/inc/suballoc.h` | 47 | 111 | 42.342% | 0 | 1 | 2 | 0 | 2 |
| `src/mvdm-host/inc/vrnmpipe.h` | 179 | 409 | 43.765% | 0 | 1 | 1 | 0 | 1 |
| `src/mvdm-host/dos/dem/dem.h` | 118 | 260 | 45.385% | 0 | 1 | 1 | 0 | 1 |
| `src/mvdm-host/softpc.new/base/inc/cpu_vid.h` | 63 | 136 | 46.324% | 0 | 4 | 8 | 1 | 9 |
| `src/non-mvdm/public/sdk/inc/ntrtl.h` | 3028 | 6347 | 47.708% | 14 | 11 | 0 | 147 | 147 |
| `src/mvdm-host/softpc.new/base/cvidc/evidgen.h` | 84 | 176 | 47.727% | 0 | 4 | 4 | 0 | 4 |
| `src/non-mvdm/public/sdk/inc/ntcsrmsg.h` | 68 | 141 | 48.227% | 0 | 1 | 2 | 2 | 4 |
| `src/mvdm-host/softpc.new/base/inc/egacpu.h` | 235 | 468 | 50.214% | 0 | 2 | 4 | 0 | 4 |
| `src/non-mvdm/base/win32/server/basesrv.h` | 115 | 225 | 51.111% | 0 | 2 | 3 | 1 | 4 |
| `src/mvdm-host/dpmi32/dpmi32p.h` | 132 | 257 | 51.362% | 0 | 1 | 11 | 0 | 11 |
| `src/mvdm-host/inc/memapi.h` | 43 | 82 | 52.439% | 0 | 5 | 5 | 5 | 10 |
| `src/mvdm-host/wow32/wdde.h` | 64 | 120 | 53.333% | 0 | 5 | 20 | 4 | 24 |
| `src/mvdm-host/wow32/wowhooks.h` | 101 | 185 | 54.595% | 0 | 1 | 2 | 0 | 2 |
| `src/mvdm-host/wow32/winsockp.h` | 107 | 195 | 54.872% | 0 | 3 | 17 | 0 | 17 |
| `src/non-mvdm/public/sdk/inc/ntregapi.h` | 291 | 524 | 55.534% | 0 | 5 | 0 | 12 | 12 |
| `src/mvdm-host/wow32/wumsg.h` | 25 | 43 | 58.140% | 0 | 1 | 1 | 1 | 2 |
| `src/mvdm-host/dpmi32/dpmidata.h` | 374 | 627 | 59.649% | 0 | 4 | 10 | 25 | 35 |
| `src/mvdm-host/wow32/wmdisp32.h` | 80 | 130 | 61.538% | 0 | 2 | 2 | 2 | 4 |
| `src/mvdm-host/wow32/wkman.h` | 88 | 140 | 62.857% | 0 | 1 | 2 | 2 | 4 |
| `src/non-mvdm/public/sdk/inc/nturtl.h` | 711 | 1103 | 64.461% | 0 | 3 | 0 | 29 | 29 |
| `src/non-mvdm/base/win32/server/srvvdm.h` | 97 | 135 | 71.852% | 0 | 2 | 2 | 0 | 2 |
| `src/mvdm-host/wow32/wgdi.h` | 143 | 162 | 88.272% | 0 | 1 | 0 | 1 | 1 |
| `src/mvdm-host/softpc.new/base/ccpu386/ntthread.h` | 8 | 9 | 88.889% | 0 | 2 | 3 | 0 | 3 |
## build/export description

| 文件 | 可证实接入行（下限） | 原版总行 | 下限占比 | 函数数 | 非-error点 | +行 | −行 | diff行 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| `src/mvdm-host/softpc.new/base/ccpu386/sources` | N/A | 183 | N/A | 0 | 1 | 0 | 1 | 1 |
| `src/mvdm-host/softpc.new/host/src/sources` | N/A | 111 | N/A | 0 | 1 | 0 | 1 | 1 |
| `src/mvdm-host/wow32/wow32.def` | N/A | 43 | N/A | 0 | 1 | 1 | 0 | 1 |

## 边界

- Integration numerator counts retained original function scope rows (comments and blanks included), plus emitted matching original declaration/data rows outside functions. Excludes #error/#line/framework and new adaptation rows.
- This is evidenced source footprint, NOT exact executable runtime coverage or a complete dependency slice. Macro definitions outside those scopes are listed separately, not silently counted as used or unused.
- Headers are declaration lower bounds; macro expansion does not attribute body rows to definition headers in normal MSVC /E output. Their ratios are not directly comparable to complete C function ranges.
- Conditional #line ambiguity is reported from the existing conservative census; affected entries need local source review before an extraction decision.
- Product compile/link-input closure includes archive members, not proof every compiled function survives linker garbage collection. No inference of safety to delete.
- Two sources files and wow32.def have no C numerator; they are included with N/A, never zero.
