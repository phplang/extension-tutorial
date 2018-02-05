/* tutorial.c */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"

#include <curl/curl.h>

static zend_class_entry *curl_easy_ce = NULL;

static zend_function_entry curl_easy_methods[] = {
    PHP_FE_END
};

static PHP_MINIT_FUNCTION(tutorial) {
    zend_class_entry ce;

    if (CURLE_OK != curl_global_init(CURL_GLOBAL_DEFAULT)) {
        return FAILURE;
    }

    INIT_CLASS_ENTRY(ce, "Tutorial\\CURLEasy", curl_easy_methods);
    curl_easy_ce = zend_register_internal_class(&ce);

    return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(tutorial) {
    curl_global_cleanup();

    return SUCCESS;
}

zend_module_entry tutorial_module_entry = {
    STANDARD_MODULE_HEADER,
    "tutorial",
    NULL, /* functions */
    PHP_MINIT(tutorial),
    PHP_MSHUTDOWN(tutorial),
    NULL, /* RINIT */
    NULL, /* RSHUTDOWN */
    NULL, /* MINFO */
    NO_VERSION_YET,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_TUTORIAL
ZEND_GET_MODULE(tutorial)
#endif

