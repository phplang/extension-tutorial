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

static PHP_FUNCTION(tutorial_curl_escape) {
    char *str, *escaped;
    size_t len;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &str, &len) == FAILURE) {
        return;
    }

    escaped = curl_escape(str, len);
    if (escaped) {
        RETVAL_STRING(escaped);
        curl_free(escaped);
        return;
    }
}

static PHP_FUNCTION(tutorial_curl_info) {
    curl_version_info_data *info = curl_version_info(CURLVERSION_NOW);

    if (!info) {
        return;
    }

    array_init(return_value);
    add_assoc_long(return_value, "age", info->age);
    add_assoc_string(return_value, "version", info->version);
    add_assoc_long(return_value, "version_num", info->version_num);
    add_assoc_string(return_value, "host", info->host);
    add_assoc_long(return_value, "features", info->features);
    if (info->ssl_version) {
        zval ssl;
        array_init(&ssl);
        add_assoc_string(&ssl, "version", info->ssl_version);
        add_assoc_long(&ssl, "version_num", info->ssl_version_num);
        add_assoc_zval(return_value, "ssl", &ssl);
    }
    if (info->libz_version) {
        add_assoc_string(return_value, "libz", info->libz_version);
    }
    if (info->protocols) {
        const char* const* p = info->protocols;
        zval protos;
        array_init(&protos);
        while (*p) {
            add_next_index_string(&protos, *p);
            ++p;
        }
        add_assoc_zval(return_value, "protocols", &protos);
    }
}

static zend_function_entry tutorial_functions[] = {
    PHP_FE(tutorial_curl_version, NULL)
    PHP_FE(tutorial_curl_ver, NULL)
    PHP_FE(tutorial_curl_escape, NULL)
    PHP_FE(tutorial_curl_info, NULL)
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

