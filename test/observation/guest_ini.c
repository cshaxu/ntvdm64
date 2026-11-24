#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The same validated text owner serves network keys and enhanced drivers.
   Driver entries are repeated INI keys: preserve their original line bytes. */
static char *line_end(char *line)
{
    char *end=strchr(line,'\n');
    return end?end:line+strlen(line);
}
static int header(char *line,char *end,const char *name)
{
    size_t n=strlen(name);
    if((size_t)(end-line)<n || _strnicmp(line,name,n))return 0;
    line+=n;
    while(line<end && (*line==' ' || *line=='\t' || *line=='\r'))++line;
    return line==end;
}
static unsigned sections(char *text,const char *name)
{
    unsigned count=0; char *line,*end;
    for(line=text;*line;line=*end?end+1:end){end=line_end(line);if(header(line,end,name))++count;}
    return count;
}
static void trim(const char **a,const char **b)
{
    while(*a<*b && (**a==' ' || **a=='\t' || **a=='\r'))++*a;
    while(*b>*a && ((*b)[-1]==' ' || (*b)[-1]=='\t' || (*b)[-1]=='\r'))--*b;
}
static int suffix(const char *a,const char *b,const char *name)
{
    size_t n=strlen(name);
    return (size_t)(b-a)>=n && !_strnicmp(b-n,name,n);
}
static int remove_driver(char *line,char *end,int enhanced,int mode,unsigned flags)
{
    const char *a=line,*b=end,*raw,*comment,*base,*p;
    trim(&a,&b);
    if(b-a<6 || _strnicmp(a,"device",6))return 0;
    a+=6;trim(&a,&b);if(a==b || *a!='=')return 0;raw=++a;
    if(mode==95 || enhanced){comment=memchr(a,';',(size_t)(b-a));if(comment)b=comment;}
    trim(&a,&b);
    if(mode==95)return *end && (suffix(a,b,"ntvdm64.386") || suffix(a,b,"ntvdm64.vxd") || suffix(a,b,"insignia.386") || suffix(a,b,"ints.386"));
    if(enhanced){
        while(a<b && *a=='"')++a;while(b>a && b[-1]=='"')--b;
        base=a;for(p=a;p<b;++p)if(*p=='\\' || *p=='/')base=p+1;
        if(b-base==11 && !_strnicmp(base,"NTVDM64.386",11))return 1;
    }
    a=raw;b=end;trim(&a,&b);
    return (flags&1) && suffix(a,b,"insignia.386");
}
static int emit(HANDLE file,const char *text,size_t length)
{
    DWORD written;
    return length<=MAXDWORD && WriteFile(file,text,(DWORD)length,&written,NULL) && written==length;
}
static int file_entry(const char *line,const char *end,const char *key,const char *name)
{
    const char *base,*comment,*p;
    size_t key_length=strlen(key);
    trim(&line,&end);
    if(line<end && *line==';'){++line;trim(&line,&end);}
    if((size_t)(end-line)<=key_length || _strnicmp(line,key,key_length))return 0;
    line+=key_length;trim(&line,&end);
    if(line==end || *line!='=')return 0;
    ++line;
    comment=memchr(line,';',(size_t)(end-line));
    if(comment)end=comment;
    trim(&line,&end);
    if(end-line>=2 && *line=='"' && end[-1]=='"'){++line;--end;}
    base=line;
    for(p=line;p<end;++p)if(*p=='\\' || *p=='/')base=p+1;
    return (size_t)(end-base)==strlen(name) && !_strnicmp(base,name,(size_t)(end-base));
}
static int driver_entry(HANDLE file,const char *root,const char *name,int leading)
{
    char line[MAX_PATH+64];
    int n=_snprintf_s(line,sizeof(line),_TRUNCATE,"%sdevice=%s\\PATCH\\%s%s",leading?"\r\n":"",root,name,leading?"":"\r\n");
    return n>=0 && emit(file,line,(size_t)n);
}
/* Literal operations on one unambiguous section. Set requires one existing
   key; append preserves repeated keys such as device. Unrelated bytes and
   the edited line's terminator are retained. Always write a fresh candidate. */
static int literal_profile(char *text,const char *output,const char *operation,
                           const char *section,const char *key,const char *value)
{
    char heading[256],*line,*end,*next,*value_start=NULL,*value_end=NULL;
    char *insertion=NULL;const char *newline="\r\n";
    size_t key_length=strlen(key),newline_length=2;
    unsigned matches=0;int selected=0,append=!strcmp(operation,"append"),drop=!strcmp(operation,"drop-file"),result=1;
    HANDLE file;
    if((!append && !drop && strcmp(operation,"set")) || !*section || !key_length ||
       strpbrk(section,"[]\r\n") || strpbrk(key,"=\r\n") || strpbrk(value,"\r\n") ||
       (drop && (!*value || strpbrk(value,"\\/:;"))) ||
       _snprintf_s(heading,sizeof(heading),_TRUNCATE,"[%s]",section)<0 ||
       sections(text,heading)!=1)return 2;
    for(line=text;*line;line=next){
        char *content_end;
        end=line_end(line);next=*end?end+1:end;
        content_end=end;if(content_end>line && content_end[-1]=='\r')--content_end;
        if(*line=='['){
            selected=header(line,end,heading);
            if(selected){
                insertion=next;
                if(*end && (end==line || end[-1]!='\r')){newline="\n";newline_length=1;}
            }
        }else if(selected && (size_t)(content_end-line)>key_length &&
                 !_strnicmp(line,key,key_length) && line[key_length]=='='){
            ++matches;value_start=line+key_length+1;value_end=content_end;
        }
    }
    if(!append && !drop && matches!=1)return 2;
    file=CreateFileA(output,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE)return 1;
    if(drop){
        selected=0;
        for(line=text;*line;line=next){
            end=line_end(line);next=*end?end+1:end;
            if(*line=='[')selected=header(line,end,heading);
            if(selected && file_entry(line,end,key,value))continue;
            if(!emit(file,line,(size_t)(next-line)))goto done;
        }
    }else if(append){
        if(!emit(file,text,(size_t)(insertion-text)))goto done;
        if(insertion>text && insertion[-1]!='\n' && !emit(file,newline,newline_length))goto done;
        if(!emit(file,key,key_length) || !emit(file,"=",1) ||
           !emit(file,value,strlen(value)) || !emit(file,newline,newline_length) ||
           !emit(file,insertion,strlen(insertion)))goto done;
    }else if(!emit(file,text,(size_t)(value_start-text)) ||
             !emit(file,value,strlen(value)) || !emit(file,value_end,strlen(value_end)))goto done;
    if(FlushFileBuffers(file))result=0;
done:
    CloseHandle(file);if(result)DeleteFileA(output);return result;
}
static int driver_profile(char *text,const char *output,const char *root,const char *version,const char *mask)
{
    unsigned flags;char *last,*line,*end,*next;int mode,enhanced=0,boot=0,boots=0,mouse=0,result=1;
    HANDLE file=INVALID_HANDLE_VALUE;
    if(strcmp(version,"31") && strcmp(version,"95"))return 2;
    mode=atoi(version);flags=(unsigned)strtoul(mask,&last,10);
    if(!*mask || *last || flags>15 || (mode==95 && (flags&4)) || strlen(root)>=MAX_PATH || sections(text,"[386Enh]")!=1)return 2;
    for(line=text;*line;line=*end?end+1:end){
        end=line_end(line);
        if(*line=='['){boot=0;if(header(line,end,"[boot]") && !boots++)boot=1;}
        if(boot && end-line>=9 && !_strnicmp(line,"mouse.drv",9)){
            const char *a=line+9,*b=end;trim(&a,&b);
            if(a<b && *a=='='){++a;trim(&a,&b);mouse+=(b-a==9 && !_strnicmp(a,"mouse.drv",9))?1:2;}
        }
    }
    if(mode==95 && (flags&8) && mouse!=1)return 2;
    file=CreateFileA(output,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE)return 1;
    for(line=text;*line;line=next){
        end=line_end(line);next=*end?end+1:end;
        if(*line=='[')enhanced=header(line,end,"[386Enh]");
        if(enhanced && *line=='['){
            if(!emit(file,"[386Enh]",8))goto done;
            if(mode==31){
                if(!emit(file,"\r\n",2))goto done;
                if((flags&4) && !driver_entry(file,root,"RETIRE.386",0))goto done;
                if((flags&2) && !driver_entry(file,root,"INTS.386",0))goto done;
                if((flags&1) && !driver_entry(file,root,"INSIGNIA.386",0))goto done;
                if(!driver_entry(file,root,"NTVDM64.386",0))goto done;
            }else{
                if((flags&1) && !driver_entry(file,root,"INSIGNIA.386",1))goto done;
                if((flags&2) && !driver_entry(file,root,"INTS.386",1))goto done;
                if(!driver_entry(file,root,"NTVDM64.386",1) || !emit(file,line+8,(size_t)(next-line-8)))goto done;
            }
        }else if(!remove_driver(line,end,enhanced,mode,flags) && !emit(file,line,(size_t)(next-line)))goto done;
    }
    if(FlushFileBuffers(file))result=0;
done:
    CloseHandle(file);if(result)DeleteFileA(output);return result;
}
/* Test preparation only: existing network operation or driver candidate. */
int main(int argc, char **argv)
{
    FILE *file;
    long size;
    char *text;
    const char *value=NULL;
    unsigned boots = 0;
    char actual[32];
    char temporary[MAX_PATH];
    int result = 1;
    if (argc != 3 && argc != 6 && argc != 7) return 2;
    if (argc==6 || argc==7) { /* Candidate operations, never in-place. */ }
    else if (!strcmp(argv[2], "Empty")) value = "";
    else if (!strcmp(argv[2], "WfwNet")) value = "wfwnet.drv";
    else return 2;
    file = fopen(argv[1], "rb");
    if (!file) return 1;
    if (fseek(file, 0, SEEK_END) || (size = ftell(file)) < 0 ||
        fseek(file, 0, SEEK_SET)) { fclose(file); return 1; }
    text = malloc((size_t)size + 1);
    if (!text) { fclose(file); return 1; }
    if (fread(text, 1, (size_t)size, file) != (size_t)size) {
        free(text); fclose(file); return 1;
    }
    fclose(file);
    text[size] = 0;
    if (strlen(text) != (size_t)size) { free(text); return 2; }
    if(argc==7){result=literal_profile(text,argv[2],argv[3],argv[4],argv[5],argv[6]);free(text);return result;}
    if(argc==6){result=driver_profile(text,argv[2],argv[3],argv[4],argv[5]);free(text);return result;}
    boots=sections(text,"[boot]");
    free(text);
    if (boots != 1) return 2;
    /* SYSTEM.INI has host registry mappings. Edit a private sibling name,
       then replace the owned file; never pass SYSTEM.INI to profile APIs. */
    if (strlen(argv[1]) + sizeof(".ntvdm-network") > sizeof(temporary)) return 2;
    strcpy(temporary, argv[1]);
    strcat(temporary, ".ntvdm-network");
    if (!CopyFileA(argv[1], temporary, TRUE)) return 1;
    if (!WritePrivateProfileStringA("boot", "network.drv", value, temporary)) goto done;
    /* Flush the API cache before the filesystem rename. This operation's
       return value is not the success contract of a key write. */
    WritePrivateProfileStringA(NULL, NULL, NULL, temporary);
    GetPrivateProfileStringA("boot", "network.drv", "missing", actual,
                            sizeof(actual), temporary);
    if (strcmp(actual, value)) goto done;
    if (MoveFileExA(temporary, argv[1], MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) result = 0;
done:
    if (result) DeleteFileA(temporary);
    return result;
}
