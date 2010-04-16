/* Pathological comments.
Added to tests
BKB 2010-04-16 15:33:38 
*/

const char * pathological_string = "/*";
const char * pathological_string_again = "//";


/\
*  this is a comment */
/\
/ this is a C++ style comment.
#include <stdio.h>
int crazyfunction ()
{
    printf ("/*\
");
    printf ("//");
/\
*  this is a comment */
/\
/ this is a C++ style comment.
    return 0;
}
