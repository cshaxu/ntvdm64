/* Synthetic fixture only: the byte patterns are not guest media. */
#include <stddef.h>
extern const unsigned char t198_s25_ntdos_bytes[27858];
extern const unsigned char t198_s93_command_bytes[50384];
extern const unsigned char t198_s93_share_bytes[882];
int main(void) {
    size_t i;
    for(i=0;i<27858;i++)if(t198_s25_ntdos_bytes[i]!=(unsigned char)(i*3+1))return 1;
    for(i=0;i<50384;i++)if(t198_s93_command_bytes[i]!=(unsigned char)(i*5+2))return 2;
    for(i=0;i<882;i++)if(t198_s93_share_bytes[i]!=(unsigned char)(i*7+3))return 3;
    return 0;
}
