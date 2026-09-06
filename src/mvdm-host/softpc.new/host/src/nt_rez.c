#include "insignia.h"
#include "host_def.h"

/*                      INSIGNIA MODULE SPECIFICATION
                        -----------------------------

FILE NAME       : nt_rez.c
MODULE NAME     : nt CMOS read/write routines

        THIS PROGRAM SOURCE FILE IS SUPPLIED IN CONFIDENCE TO THE
        CUSTOMER, THE CONTENTS  OR  DETAILS  OF ITS OPERATION MAY
        ONLY BE DISCLOSED TO PERSONS EMPLOYED BY THE CUSTOMER WHO
        REQUIRE A KNOWLEDGE OF THE  SOFTWARE  CODING TO CARRY OUT
        THEIR JOB. DISCLOSURE TO ANY OTHER PERSON MUST HAVE PRIOR
        AUTHORISATION FROM THE DIRECTORS OF INSIGNIA SOLUTIONS INC.

DESIGNER        :
DATE            :

PURPOSE         :



The Following Routines are defined:
                1. host_read_resource
                2. host_write_resource

=========================================================================

AMENDMENTS      :

=========================================================================
*/


#include <stdio.h>
#include <io.h>
#include <sys\types.h>
#include <fcntl.h>
#include <sys\stat.h>

#include "xt.h"
#include "error.h"
#include "spcfile.h"
#include "timer.h"

/* CPU40 starts with exactly bios1.rom, bios4.rom and v7vga.rom.  They are
 * immutable executable resources of the product, not SoftPC profile data.
 * Keep every other resource type on the historical file path: in particular
 * profile.spc and writable cmos.ram must never be taken from the EXE. */
static long host_read_embedded_rom(char *name, byte *addr, int maxsize)
{
        HMODULE module;
        HRSRC resource;
        HGLOBAL loaded;
        DWORD size;
        void *bytes;
        char *resource_name = NULL;

        if (strcmp(name, "bios1.rom") == 0) resource_name = "SOFTPC_BIOS1";
        else if (strcmp(name, "bios4.rom") == 0) resource_name = "SOFTPC_BIOS4";
        else if (strcmp(name, "v7vga.rom") == 0) resource_name = "SOFTPC_V7VGA";
        else return 0;

        module = GetModuleHandleA(NULL);
        resource = module == NULL ? NULL : FindResourceA(module, resource_name,
            RT_RCDATA);
        if (resource == NULL) return 0;
        size = SizeofResource(module, resource);
        if (size == 0u || size > (DWORD)maxsize) return 0;
        loaded = LoadResource(module, resource);
        bytes = loaded == NULL ? NULL : LockResource(loaded);
        if (bytes == NULL) return 0;
        memcpy(addr, bytes, size);
        return (long)size;
}


/*
 * Allow a suitable default for the CMOS file name.
 */

#ifndef CMOS_FILE_NAME
#define CMOS_FILE_NAME "cmos.ram"
#endif

long host_read_resource(int type, char *name, byte *addr, int maxsize, int display_error)
/* int type;                     Unused */
/* char *name;                   Name of resource */
/* byte *addr;                   Address to read data into */
/* int maxsize;                  Max size that should be read */
/* int display_error;            Flag to control error message output */
{

        int file_fd;
        long size=0;
        char full_path[MAXPATHLEN];
        extern char *host_find_file(char *name, char *path, int disp_err);

        if (type == ROMS_REZ_ID)
            return host_read_embedded_rom(name, addr, maxsize);

#ifdef DELTA            //STF - make change to 8.3 compatible name
        if (strcmp(name, ".spcprofile") == 0)
            name = "profile.spc";
#endif

        file_fd = _open(host_find_file (name, full_path, display_error), O_RDONLY|O_BINARY);

        if (file_fd != -1)      /* Opened successfully */       {
                /* seek to end to get size */
                size = _lseek (file_fd, 0L, 2);

                if (size > maxsize)     // corrupted file???
                    return(0);

                /* Seek back to start before reading! */
                _lseek (file_fd, 0L, 0);

                _read(file_fd,addr,size);
                _close(file_fd);
        }

        return (size);
}



/********************************************************/

void host_write_resource(type,name,addr,size)
int type;               /* Unused */
char *name;             /* Name of resource */
byte *addr;             /* Address of data to write */
long size;              /* Quantity of data to write */
{
        int file_fd;
        char full_path[MAXPATHLEN];
        char *hff_ret;
        extern char *host_find_file(char *name, char *path, int disp_err);

        host_block_timer ();

#ifdef DELTA            //STF - make change to 8.3 compatible name
        if (strcmp(name, ".spcprofile") == 0)
            name = "profile.spc";
#endif

        hff_ret = host_find_file (name,full_path,SILENT);

        if (hff_ret != NULL)
        {
                file_fd = _open (hff_ret,O_WRONLY);

                if (file_fd != -1)
                {
                        _write (file_fd, addr, size);
                        _close (file_fd);
                }
                else
                {

#ifndef HUNTER
                        host_error (EG_REZ_UPDATE,ERR_CONT,name);
#endif

                        /* Continuing => try the selected resource location.
                         * DIVERGENCE(MVDM-HOST-DIV-133): the original retry
                         * used the bare resource name, which redirects an
                         * already-resolved session resource into process CWD.
                         * Keep the original create/retry and error ordering,
                         * but retain the path returned by host_find_file. */
                        file_fd = _open(hff_ret,O_RDWR|O_CREAT,S_IREAD|S_IWRITE);

                        if (file_fd != -1)
                        {
                                _write (file_fd, addr, size);
                                _close (file_fd);
                        }

#ifndef HUNTER
                        else
                        {
                                /* Tell the user we cannot update */
                                host_error (EG_NO_REZ_UPDATE, ERR_CONT, CMOS_FILE_NAME);
                        }
#endif

                }
        }
        else
        {
                /* host find file has failed and we have
                 * reached this point with no error panels
                 */

#ifndef HUNTER
                host_error (EG_REZ_UPDATE,(ERR_QUIT|ERR_CONT),name);
#endif

                /* Continuing => try to create a new file */
                file_fd = _open(name,O_RDWR|O_CREAT,S_IREAD|S_IWRITE);

                if (file_fd != -1)
                {
                        _write (file_fd, addr, size);
                        _close (file_fd);
                }

#ifndef HUNTER
                else
                {
                        /* Tell the user we cannot update */
                        host_error (EG_NO_REZ_UPDATE, ERR_CONT,
                                    CMOS_FILE_NAME);
                }
#endif

        }

        host_release_timer ();
}
