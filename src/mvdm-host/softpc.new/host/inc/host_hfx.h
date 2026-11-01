/*
 * VPC-XT Revision 1.0
 *
 * Title	: host_hfx.h 
 *
 * Description	: Host dependent definitions for HFX.
 *
 * Author	: J. Koprowski + L. Dworkin
 *
 * Notes	:
 *
 * Mods		:
 */

#ifdef SCCSID
#error DIVERGENCE MVDM-HOST-DIV-518: SCCSID identification strings are not selected; see mirror register.
#endif

#ifdef HFX
#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-504: non-PROD header branch is not selected; see mirror register.
#endif

/*
 * Return values from host_map_file function.
 */

/*
 * Returned if a match was required and was successful.
 */
#define FILE_MATCH 0
/*
 * Returned if a match was required and failed.
 */
#define MATCH_FAIL 1
/*
 * Returned if no match was required and no mapping took place.
 */
#define NAME_LEGAL 2
/*
 * Returned if no match was required and mapping took place.
 */
#define NAME_MAPPED 3

/*
 * Tables for conversion from base forty one to legal DOS characters.
 */
#define HOST_CHAR_TABLE1 "!#$%&@^_~0123456789XYZADFGHIJKLMNOPQRSUVW"
#define HOST_CHAR_TABLE2 "!#$%&@^_~0123456789ABCDFGHJKLMNPQRTUVWXYZ"
#define HOST_CHAR_TABLE3 "!#$%&@^_~0123456789ADFGHIJKLMNOPQRSUVWXYZ"

/*
 * Illegal file name specification.  This is the name
 * used when a host filename is completely illegal under
 * DOS.
 */
#define ILLEGAL_NAME "ILLEGAL"
#define ILLEGAL_NAME_LENGTH 7
/*
 * Codes passed to host_validate_path function.
 */
/*
 * HFX_NEW_FILE indicates that the path may be mapped, but not the filename
 * itself.  In this case the last field is not validated, but is simply
 * concatenated to the host name generated.
 */
#define HFX_NEW_FILE 0
/*
 * HFX_OLD_FILE indicates that the file concerned may already exist
 * and require mapping.  Thus, checks are made to see if the last field
 * exists, doing a directory search for mapped names if necessary.
 */
#define HFX_OLD_FILE 1
/*
 * HFX_PATH_ONLY acts in the same way as HFX_NEW_FILE except that the
 * final name field is not concatenated to the host name output.
 * In the current version the path will be output with a slash as the final
 * character.
 */
#define HFX_PATH_ONLY 2
/*
 * External function declarations.
 */
extern boolean host_file_search();
extern word host_gen_err();

#ifndef access
#include <io.h>     /* IO.H contains define of access to _acccess */
#endif

#define host_access access

#define host_opendir opendir
#define host_readdir readdir
#define host_closedir closedir
#define host_malloc malloc
#define host_free free
#define host_getpid getpid

/*
 * Global variable external references.
 */
/* extern char *hfx_root[]; */
extern char *get_hfx_root  IPT1(half_word, hfx_entry);

/*
 * Directory type definitions.
 */
 #define HOST_DIR int           /*ADE*/
 typedef struct hfx_found_dir_entry
 {
       half_word                       attr;
       char                            *host_name;
       char                            *dos_name;
       int                             direntry;
       struct hfx_found_dir_entry      *next;
 } HFX_FOUND_DIR_ENT;

/* This is a base structure defined in the host
 * include file due to the dependence on the host
 * specific HOST_DIR type
 */
typedef struct hfx_direntry
{
	HOST_DIR			*dir;
	char				*name;
        char                            *template;
	int				direntry;
        HFX_FOUND_DIR_ENT               *found_list_head;
        boolean                         first_find;
	struct hfx_direntry		*next;
	struct hfx_direntry		*last;
} HFX_DIR;


/*
 * Host maximum file name length including path.  N.B. This may
 * need increasing.
 */
#define MAX_PATHLEN 256

#endif /* HFX */
