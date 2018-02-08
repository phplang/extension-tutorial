#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"

ZEND_BEGIN_MODULE_GLOBALS(tutorial)
    char *default_url;
ZEND_END_MODULE_GLOBALS(tutorial)

ZEND_EXTERN_MODULE_GLOBALS(tutorial)

#define TUTORIALG(v) ZEND_MODULE_GLOBALS_ACCESSOR(tutorial, v)
