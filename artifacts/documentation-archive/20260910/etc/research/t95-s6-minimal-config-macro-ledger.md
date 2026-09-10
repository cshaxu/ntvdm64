# M0 T95 S6: Minimal Candidate Configuration Macro Ledger

## Question And Scope

This is the complete configuration ledger for the declared composition,
SIM/tree, logging, PC-system, memory, empty-port, and CPU3 decoder candidate
translation units.  It covers every `config.h` macro read directly by
`bochs.h`, `osdep.h`, `siminterface.h`, `cpu.h`, `memory.h`, `iodev.h`, or the
candidate source conditionals.  It does not assign a generated configuration.

## Method

Read the candidate files and `config.h.in`/imported `config.h` only as source
and comparison evidence.  No artifact was created or changed; no compiler,
linker, configure, VS project, or executable was invoked.  Each macro or
closed macro set appears once below with exactly one classification.

## Source-Forced

| Macros | Classification and condition |
| --- | --- |
| `BX_CPU_LEVEL=3`; `BX_SUPPORT_X86_64=0`; `BX_SUPPORT_SMP=0`; `BX_SUPPORT_APIC=0` | Source-forced: the registered singleton `bx_cpu` and CPU init must omit CPUID/APIC paths. |
| `BX_CONFIGURE_MSRS=0`; `BX_WITH_WX=0`; `BX_SUPPORT_VMX=0`; `BX_SUPPORT_SVM=0`; `BX_SUPPORT_AVX=0` | Source-forced: otherwise CPU initialization reaches SIM/state or unadmitted extensions. |
| `BX_SUPPORT_FPU=0`; `BX_SUPPORT_3DNOW=0`; `BX_SUPPORT_MISALIGNED_SSE=0`; `BX_SUPPORT_MONITOR_MWAIT=0`; `BX_SUPPORT_ALIGNMENT_CHECK=0`; `BX_SUPPORT_REPEAT_SPEEDUPS=0`; `BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS=0` | Source-forced: no such fixture contract is admitted; FPU also removes reset's FPU edge. |
| `BX_PHY_ADDRESS_LONG=0`; derived `BX_PHY_ADDRESS_WIDTH=32`, `BX_LIN_ADDRESS_WIDTH=32` | Source-forced reset-window address model. |
| `BX_LARGE_RAMFILE=0`; `BX_NO_LOGGING=0`; `BX_TIMER_DEBUG=0`; `BX_DEBUG_LINUX=0`; `BX_SHOW_IPS=0` | Source-forced: no file backing or debug/timer mode; original logging accessors remain live. |
| `BX_WITH_NOGUI=1`; `BX_WITH_WIN32=BX_WITH_X11=BX_WITH_MACOS=BX_WITH_CARBON=BX_WITH_TERM=BX_WITH_RFB=BX_WITH_SDL=BX_WITH_SVGA=BX_WITH_AMIGAOS=BX_WITH_WX=BX_USE_TEXTCONFIG=0` | Source-forced denial of all Bochs GUI/configuration front ends. |
| `BX_PLUGINS=BX_HAVE_LTDL=BX_HAVE_DLFCN_H=0`; `BX_DEBUGGER=BX_GDBSTUB=BX_DISASM=BX_DEBUGGER_GUI=BX_X86_DEBUGGER=BX_INSTRUMENTATION=0` | Source-forced product/debug exclusion. |
| `BX_DMA_FLOPPY_IO=BX_SUPPORT_PCI=BX_SUPPORT_PCIDEV=BX_SUPPORT_USB_UHCI=BX_SUPPORT_USB_OHCI=BX_SUPPORT_USB_XHCI=BX_SUPPORT_PCIUSB=BX_SUPPORT_CLGD54XX=BX_NETWORKING=BX_SUPPORT_PCIPNIC=BX_SUPPORT_GAMEPORT=BX_SUPPORT_SOUNDLOW=BX_SUPPORT_BUSMOUSE=BX_SUPPORT_IODEBUG=0` | Source-forced no-device boundary. |

## MSVC-Platform-Probed

| Macros | Proof method |
| --- | --- |
| `_MSC_VER`, `_WIN32`, `WIN32` | Record a non-building MSVC predefined-macro observation. `_WIN32` is compiler fact; `WIN32` is the Bochs/platform mapping only after that observation. Neither selects `BX_WITH_WIN32`. |
| `SIZEOF_UNSIGNED_CHAR`, `SIZEOF_UNSIGNED_SHORT`, `SIZEOF_UNSIGNED_INT`, `SIZEOF_UNSIGNED_LONG`, `SIZEOF_UNSIGNED_LONG_LONG`, `SIZEOF_INT_P`; `WORDS_BIGENDIAN`; `Bit8u..Bit64s`, `bx_ptr_equiv_t`, `bx_bool`, `bx_ptr_t` | Record compiler ABI/predefined-type probe; derive typedef branch only from that record. |
| `CDECL`, `BOCHSAPI`, `BOCHSAPI_CYGONLY`, `BOCHSAPI_MSVCONLY`, `BX_CONST64`, `inline`, `BX_CPP_INLINE`, `BX_CPP_AlignN`, `BX_CPP_AttrPrintf`, `BX_CPP_AttrNoReturn`, `GCC_ATTRIBUTE` | Record MSVC preprocessor and calling-convention result. |
| `BX_HAVE_UNISTD_H`, `BX_HAVE_SNPRINTF`, `BX_HAVE_VSNPRINTF`, `BX_HAVE_STRTOULL`, `BX_HAVE_STRTOUQ`, `BX_HAVE_STRDUP`, `BX_HAVE_STRREV`, `BX_HAVE_STRICMP`, `BX_HAVE_STRCASECMP`, `BX_HAVE_ABORT`, `BX_HAVE_REALTIME_USEC`, `BX_HAVE_GETTIMEOFDAY`, `BX_HAVE_SLEEP`, `BX_HAVE_MSLEEP`, `BX_HAVE_USLEEP`, `BX_HAVE_NANOSLEEP`, `BX_HAVE_MKSTEMP`, `BX_HAVE_TMPFILE64`, `BX_HAVE_FSEEK64`, `BX_HAVE_FSEEKO64`, `BX_HAVE_SOCKLEN_T`, `BX_HAVE_GETENV`, `BX_HAVE_SETENV`, `BX_HAVE_SELECT`, `BX_HAVE_SYS_MMAN_H`, `BX_HAVE_XPM_H`, `BX_HAVE_TIMELOCAL`, `BX_HAVE_GMTIME`, `BX_HAVE_MKTIME`, `BX_HAVE___BUILTIN_BSWAP32`, `BX_HAVE___BUILTIN_BSWAP64`, `HAVE_LOCALE_H`, `HAVE_LIBREADLINE`, `HAVE_READLINE_HISTORY_H` | Record an MSVC header/API/preprocessor probe for each; never infer from Unix or historical generated output. |

## Retained-Symbol-Dependent

| Macros | Why retained-symbol proof is required |
| --- | --- |
| `BX_USE_CPU_SMF`, `BX_USE_MEM_SMF`, `BX_USE_DEV_SMF`, every `BX_USE_*_SMF` device submacro | They change static/member linkage and declarations; choose only after a candidate-wide symbol graph proves one compatible setting. |
| `BX_SUPPORT_A20`, `BX_FAST_FUNC_CALL`, `BX_ASSERT_ENABLE`, `BX_USE_IDLE_HACK` | They alter live mechanics or emitted code; decide only after retained sections and fixture inputs are proven. |
| `BX_EXIT`, `BX_DEFAULT_CONFIG_INTERFACE`, `BX_DEFAULT_DISPLAY_LIBRARY`, `BX_GUI_SIGHANDLER`, `BX_SHARE_PATH` | Their relevance depends on retained fatal/quit/UI sections; historical front-end values are prohibited. |
| `BX_MAX_ATA_CHANNEL`, `BX_DEFAULT_MEM_MEGS`, `BX_MIN_IPS`, `BX_SMP_QUANTUM_MIN`, `BX_SMP_QUANTUM_MAX`, `BX_BOOTSTRAP_PROCESSOR`, `BX_MAX_SMP_THREADS_SUPPORTED`, `BX_CPUID_VENDOR_LEN`, `BX_CPUID_BRAND_LEN`, `BX_CPU_VENDOR_INTEL` | No direct composition input fixes them; resolve only if a retained declaration/code edge proves they are needed. |

## Unresolved Or Rejected

`LOWLEVEL_CDROM`, `__WXMSW__`, `__MINGW32__`, `__QNXNTO__`, `__sun__`,
`macintosh`, `BX_BIG_ENDIAN`, `BX_TRACE_CACHE_NO_SPECULATIVE_TRACING`, and
all unlisted platform compatibility selectors are rejected unless a recorded
MSVC preprocessor observation proves they are compiler facts.  They are not
Bochs feature choices.  Any config macro newly discovered by the required
preprocessor inventory is unresolved by default and must receive exactly one
of the classifications above before config generation.

## Non-Circular First-Configuration Procedure

1. Freeze this ledger and obtain non-building MSVC predefined-macro/header API
   observations for every platform-probed row.
2. Expand candidate includes under a documentation-only macro dependency scan;
   add any discovered macro to this ledger before assigning it.
3. For every retained-symbol-dependent row, derive the prospective value from
   source reachability, then obtain coordinator approval for a generated
   artifact configuration.
4. Only after steps 1--3 may a fresh artifact root create the first config;
   it must record every ledger value, its evidence, and its hash before `/Gy`.

No historical `config.h`, VS property, artifact attempt value, GUI setting, or
product library can satisfy any row by default.
