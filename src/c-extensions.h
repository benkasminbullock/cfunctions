
#ifndef C_EXTENSIONS_H
#define C_EXTENSIONS_H
#if defined (__GNUC__) && ! defined (__STRICT_ANSI__)
#define X_NO_RETURN __attribute__((noreturn))
#define X_PRINT_FORMAT(a,b) __attribute__((format(printf,a,b)))
#define X_CONST __attribute__((const))
#define X_INLINE
#else 
#define X_NO_RETURN 
#define X_PRINT_FORMAT(a,b) 
#define X_CONST
#endif 
#define INLINE
#define NO_RETURN void
#define NO_SIDE_FX
#define PRINT_FORMAT(a,b)
#define LOCAL
#ifndef PROTO
#if defined(__STDC__) && __STDC__ == 1
#define PROTO(a) a
#else
#define PROTO(a) ()
#endif 
#endif 
#endif 
