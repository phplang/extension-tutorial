/* tutorial.c */

#include "tutorial.h"
#include "zend_exceptions.h"

#include <curl/curl.h>

static zend_class_entry *curl_easy_ce = NULL;
static zend_object_handlers curl_easy_handlers;
ZEND_DECLARE_MODULE_GLOBALS(tutorial);

typedef struct _curl_easy_object {
    CURL *handle;
    zend_object std;
} curl_easy_object;

static zend_object* curl_easy_to_zend_object(curl_easy_object *objval) {
    return ((zend_object*)(objval + 1)) - 1;
}
static curl_easy_object* curl_easy_from_zend_object(zend_object *objval) {
    return ((curl_easy_object*)(objval + 1)) - 1;
}

static PHP_METHOD(CurlEasy, __construct) {
    curl_easy_object *objval = curl_easy_from_zend_object(Z_OBJ_P(getThis()));
    char *url = TUTORIALG(default_url);
    size_t url_len = 0;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|p", &url, &url_len) == FAILURE) {
        return;
    }

    if (url && url[0]) {
        CURLcode ret = curl_easy_setopt(objval->handle, CURLOPT_URL, url);
        if (ret != CURLE_OK) {
            zend_throw_exception(zend_ce_exception, "Failed setting URL", (zend_long)ret);
        }
    }
}

static PHP_METHOD(CurlEasy, setOpt) {
    curl_easy_object *objval = curl_easy_from_zend_object(Z_OBJ_P(getThis()));
    zend_long opt;
    zval *value;
    CURLcode ret = CURLE_OK;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "lz", &opt, &value) == FAILURE) {
        return;
    }

    switch (opt) {
        case CURLOPT_URL: {
            zend_string *strval = zval_get_string(value);
            ret = curl_easy_setopt(objval->handle, opt, ZSTR_VAL(strval));
            zend_string_release(strval);
            break;
        }
        case CURLOPT_TIMEOUT: {
            zend_long lval = zval_get_long(value);
            ret = curl_easy_setopt(objval->handle, opt, lval);
            break;
        }
        default:
            zend_throw_exception_ex(zend_ce_exception, opt, "Unknown curl_easy_setopt() option: %ld", (long)opt);
            return;
    }
    if (ret != CURLE_OK) {
        zend_throw_exception_ex(zend_ce_exception, ret, "Failed setting option: %ld", (long)opt);
    }
    RETURN_ZVAL(getThis(), 1, 0);
}

static PHP_METHOD(CurlEasy, perform) {
    if (zend_parse_parameters_none_throw() == FAILURE) {
        return;
    }

    curl_easy_object *objval = curl_easy_from_zend_object(Z_OBJ_P(getThis()));
    curl_easy_perform(objval->handle);
}

static PHP_METHOD(CurlEasy, escape) {
    zend_string *str;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
        return;
    }

    char *escaped = curl_escape(ZSTR_VAL(str), ZSTR_LEN(str));
    if (!escaped) {
        zend_throw_exception_ex(zend_ce_exception, 0, "Failed escaping %s", ZSTR_VAL(str));
        return;
    }
    RETVAL_STRING(escaped);
    curl_free(escaped);
}

static zend_function_entry curl_easy_methods[] = {
    PHP_ME(CurlEasy, __construct, NULL, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
    PHP_ME(CurlEasy, setOpt, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(CurlEasy, perform, NULL, ZEND_ACC_PUBLIC)

    PHP_ME(CurlEasy, escape, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

static zend_object* curl_easy_ctor(zend_class_entry *ce) {
    curl_easy_object *objval = ecalloc(1, sizeof(curl_easy_object) + zend_object_properties_size(ce));
    objval->handle = curl_easy_init();

    zend_object* ret = curl_easy_to_zend_object(objval);
    zend_object_std_init(ret, ce);
    object_properties_init(ret, ce);
    ret->handlers = &curl_easy_handlers;

    return ret;
}

static zend_object* curl_easy_clone(zval *srcval) {
    zend_object *zsrc = Z_OBJ_P(srcval);
    zend_object *zdst = curl_easy_ctor(zsrc->ce);
    zend_objects_clone_members(zdst, zsrc);

    curl_easy_object *src = curl_easy_from_zend_object(zsrc);
    curl_easy_object *dst = curl_easy_from_zend_object(zdst);
    dst->handle = curl_easy_duphandle(src->handle);

    return zdst;
}

static void curl_easy_free(zend_object *zobj) {
    curl_easy_object *obj = curl_easy_from_zend_object(zobj);
    curl_easy_cleanup(obj->handle);

    zend_object_std_dtor(zobj);
}

PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("tutorial.default_url", "", PHP_INI_ALL,
                      OnUpdateString, default_url, zend_tutorial_globals, tutorial_globals)
PHP_INI_END()

static PHP_MINIT_FUNCTION(tutorial) {
    zend_class_entry ce;

    if (CURLE_OK != curl_global_init(CURL_GLOBAL_DEFAULT)) {
        return FAILURE;
    }

    INIT_CLASS_ENTRY(ce, "Tutorial\\CURLEasy", curl_easy_methods);
    curl_easy_ce = zend_register_internal_class(&ce);
    curl_easy_ce->create_object = curl_easy_ctor;

    memcpy(&curl_easy_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    curl_easy_handlers.offset = XtOffsetOf(curl_easy_object, std);
    curl_easy_handlers.clone_obj = curl_easy_clone;
    curl_easy_handlers.free_obj = curl_easy_free;

    zend_declare_class_constant_long(curl_easy_ce, "OPT_URL", strlen("OPT_URL"), CURLOPT_URL);
    zend_declare_class_constant_long(curl_easy_ce, "OPT_TIMEOUT", strlen("OPT_TIMEOUT"), CURLOPT_TIMEOUT);

    REGISTER_INI_ENTRIES();

    return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(tutorial) {
    UNREGISTER_INI_ENTRIES();
    curl_global_cleanup();

    return SUCCESS;
}

static PHP_GINIT_FUNCTION(tutorial) {
#if defined(COMPILE_DL_ASTKIT) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif
    tutorial_globals->default_url = NULL;
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
    PHP_MODULE_GLOBALS(tutorial),
    PHP_GINIT(tutorial),
    NULL, /* GSHUTDOWN */
    NULL, /* RPOSTSHUTDOWN */
    STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_TUTORIAL
ZEND_GET_MODULE(tutorial)
#endif

