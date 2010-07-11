/* This is a Cfunctions (version 0.25) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `http://www.kasmin.org/cfunctions/'. */

/* This file was generated with:
`cfunctions -inmb rc-rw.c' */
#ifndef CFH_RC_RW_H
#define CFH_RC_RW_H

/* From `rc-rw.c': */
/* Initialize all the fields of the named structure to their default
   values. */

#line 93 "rc-rw.c"
void rc_initialize (struct cfunctionsrc * rc );
/* Read the rc file and set the values of the struct. */

#line 103 "rc-rw.c"
void rc_read (struct cfunctionsrc * rc , FILE * rc_file );
/* Write the rc file. */

#line 194 "rc-rw.c"
void rc_write (struct cfunctionsrc * rc , FILE * rc_out );
/* Read the `rc' file. */

#line 260 "rc-rw.c"
void read_rc_file (struct cfunctionsrc * rc );
/* Does an rc file exist? */

#line 286 "rc-rw.c"
int rc_file_exists (void);
/* Write the `rc' file. */

#line 311 "rc-rw.c"
void write_rc_file (struct cfunctionsrc * rc );

#endif /* CFH_RC_RW_H */
