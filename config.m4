dnl config.m4
PHP_ARG_ENABLE(tutorial)

if test "$PHP_TUTORIAL" != "no"; then
  PHP_NEW_EXTENSION(tutorial, tutorial.c, $ext_shared)
fi
