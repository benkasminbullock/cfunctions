#ifndef C_EXTENSIONS_H
#define C_EXTENSIONS_H
#if defined (__GNUC__) && ! defined (__STRICT_ANSI__)
#define X_NO_RETURN __attribute__((noreturn))
#define X_PRINT_FORMAT(a,b) __attribute__((format(printf,a,b)))
#define X_CONST __attribute__((const))
#else 
#define X_NO_RETURN 
#define X_PRINT_FORMAT(a,b) 
#define X_CONST
#endif 
#define NO_RETURN void
#define NO_SIDE_FX
#define PRINT_FORMAT(a,b)
#define LOCAL
#endif /* C_EXTENSIONS_H */
