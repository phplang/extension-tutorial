/* tutorial.c */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"

#include <curl/curl.h>

static PHP_FUNCTION(tutorial_curl_version) {
    php_printf("%s\n", curl_version());
}

static PHP_FUNCTION(tutorial_curl_ver) {
    RETURN_STRING(curl_version());
}

static zend_function_entry tutorial_functions[] = {
    PHP_FE(tutorial_curl_version, NULL)
    PHP_FE(tutorial_curl_ver, NULL)
    PHP_FE_END
};

static PHP_MINIT_FUNCTION(tutorial) {
    if (CURLE_OK != curl_global_init(CURL_GLOBAL_DEFAULT)) {
        return FAILURE;
    }

    return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(tutorial) {
    curl_global_cleanup();

    return SUCCESS;
}

zend_module_entry tutorial_module_entry = {
    STANDARD_MODULE_HEADER,
    "tutorial",
    tutorial_functions,
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

