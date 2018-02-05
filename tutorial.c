/* tutorial.c */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"

#include <curl/curl.h>

static zend_class_entry *curl_easy_ce = NULL;
static zend_object_handlers curl_easy_handlers;

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
    zend_string *url = NULL;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|P", &url) == FAILURE) {
        return;
    }

    if (url) {
        CURLcode ret = curl_easy_setopt(objval->handle, CURLOPT_URL, ZSTR_VAL(url));
        if (ret != CURLE_OK) {
            zend_throw_exception(zend_ce_exception, "Failed setting URL", (zend_long)ret);
        }
    }
}

static zend_function_entry curl_easy_methods[] = {
    PHP_ME(CurlEasy, __construct, NULL, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
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

