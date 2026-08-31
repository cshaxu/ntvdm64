#include <string.h>
#include <wchar.h>

#include "session/session.h"
#include "mvdm_redirector_guest_copy.h"

typedef struct fixture_memory { unsigned char bytes[0x200]; } fixture_memory;
static int read_memory(void *c, uint32_t a, uint8_t *b, uint32_t n) { fixture_memory *m=c; if (!m||!b||a>sizeof(m->bytes)||n>sizeof(m->bytes)-a) return 0; memcpy(b,m->bytes+a,n); return 1; }
static int write_memory(void *c, uint32_t a, uint8_t const *b, uint32_t n) { fixture_memory *m=c; if (!m||!b||a>sizeof(m->bytes)||n>sizeof(m->bytes)-a) return 0; memcpy(m->bytes+a,b,n); return 1; }
int main(void) {
 fixture_memory m; session s; memset(&m,0,sizeof(m)); session_initialize(&s,1u);
 if (mvdm_redirector_copy_ansi_to_guest(0,0,"x",2)) return 1;
 if (!session_activate(&s)||!session_guest_memory_begin(&s,&m,read_memory,write_memory)||!session_thread_bind(&s)) return 2;
 if (!mvdm_redirector_copy_ansi_to_guest(0,0x10,"ok",3)||memcmp(m.bytes+0x10,"ok",3)) return 3;
 if (mvdm_redirector_copy_ansi_to_guest(0,0x1ff,"x",2)) return 4;
 if (!mvdm_redirector_copy_wide_to_guest(0,0x20,L"user")||memcmp(m.bytes+0x20,"user",5)) return 5;
 session_guest_memory_end(&s);
 if (mvdm_redirector_copy_ansi_to_guest(0,0x10,"x",2)) return 6;
 return !session_thread_unbind(&s)||!session_dispose(&s) ? 7 : 0;
}
