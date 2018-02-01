dnl config.m4
PHP_ARG_WITH(tutorial)

if test "$PHP_TUTORIAL" != "no"; then

  dnl Check for required libcurl library
  AC_MSG_CHECKING([for libcurl])
  for i in $PHP_TUTORIAL /usr/local /usr; do
    if test -f "$i/include/curl/easy.h"; then
      PHP_CURL_DIR=$i
      break
    fi
  done
  if test -z "$PHP_CURL_DIR"; then
    AC_MSG_ERROR([not found])
  fi

  dnl Found libcurl's headers
  AC_MSG_RESULT([found in $PHP_CURL_DIR])

  dnl Update library list and include paths for libcurl
  PHP_ADD_INCLUDE($PHP_CURL_DIR/include)
  PHP_ADD_LIBRARY_WITH_PATH(curl, $PHP_CURL_DIR/$PHP_LIB_DIR, TUTORIAL_SHARED_LIBADD)
  PHP_SUBST(TUTORIAL_SHARED_LIBADD)

  PHP_NEW_EXTENSION(tutorial, tutorial.c, $ext_shared)
fi
