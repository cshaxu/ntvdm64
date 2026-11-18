/* OpenNT winuserp.h dialog and directory-list flags shared by WOW and USER. */
#ifndef SCDLG_CLIENT
#define SCDLG_CLIENT 0x0001
#define SCDLG_ANSI 0x0002
#define SCDLG_NOREVALIDATE 0x0004
#define SCDLG_16BIT 0x0008
#endif
#ifndef DDL_VALID
#define DDL_VALID (DDL_READWRITE | DDL_READONLY | DDL_HIDDEN | DDL_SYSTEM | \
                   DDL_DIRECTORY | DDL_ARCHIVE | DDL_POSTMSGS | DDL_DRIVES | \
                   DDL_EXCLUSIVE)
#endif
