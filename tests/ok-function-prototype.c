#include "ok-function-prototype.h"
#ifdef HEADER
typedef int bool;
typedef int custom_type_t;
#define NULL 0
#endif /* HEADER */
bool (*libraryfunction_d)(custom_type_t *location) = NULL;
//extern bool (*libraryfunction_d)(custom_type_t *location);
