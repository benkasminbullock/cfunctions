#include <string.h>
char * a = "\"";
char * b = "\\\"";
char * c = "hallo \"";
char * d = "hallo \\\"\"\\\"";
int moo ()
{
    int lch = '&';
    int rch = '(';
    /* Bug from line 1336 of figlet.c */
    if (strchr("/\\", lch) && strchr("[]{}()<>", rch))
        return rch;
    return lch;
}
