/* The following code generated `c_word_table'. */

#include <ctype.h>
#include <stdio.h>

int
main() 
{ 
    int i; 

    printf ( "unsigned char c_word_table [256] = {\n" );

    for ( i = 0; i < 256; i++ ) {
	if ( isalpha ( i ) || i == '_' )
	    printf( "2");
	else if ( isdigit ( i ) )
	    printf ( "1" );
	else
	    printf ( "0" );
	if ( i != 255 ) 
	    printf (", ");
	if ( i % 16 == 15 ) 
	    printf ("\n");
    }
    printf ("};\n");
}
