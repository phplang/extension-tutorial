# PHP Extension Writing Tutorial

## Prerequisites

Before beginning this tutorial, you'll need a development environment with the following packages:

  * Compiler: `gcc` and `clang` are both excellent choices.
  * Build tools: `autoconf`, `automake`, `libtool`
  * (Optional) parser generators: `re2c` and `bison`
  * PHP 7.x with development headers:
    * Either distro packages such as: `php7` and `php7-devel`
    * PHP compiled from source and installed with `sudo make install`. If compiling from source, then the `--enable-debug` switch is recommended, as well as the optional re2c and bison packages.
  * `libcurl` and development headers: As part of this tutorial, we'll eventually be linking against libcurl.  Install the library and it's associated development headers from your distro or install from source.


Obviously, you'll want this repo checked out in your build environment as well.
Be sure to `git pull` to make sure you have the most recent version.

## Format

This tutorial is split into steps, with one commit per step.
At each commit, you should be able to compile the extension code
as-provided using the following steps:

### Generate ./configure script using phpize

```sh
$ phpize
Configuring for:
PHP Api Version:         20170718
Zend Module Api No:      20170718
Zend Extension Api No:   320170718
```

### Generate Makefile using ./configure

```sh
$ ./configure
checking ...
checking ...
checking ...
[ lots of output ]
configure: creating ./config.status
config.status: creating config.h
```

### Build your extension

```sh
$ make
/bin/bash .../libtool --mode=compile cc  -I. -DPHP_ATOM_INC -I/usr/local/include/php -I/usr/local/include/php/main -I/usr/local/include/php/TSRM -I/usr/local/include/php/Zend -I/usr/local/include/php/ext -DHAVE_CONFIG_H -g -O0 -c tutorial.c modules/tutorial.so

----------------------------------------------------------------------
Libraries have been installed in:
   .../modules

Build complete.
Don't forget to run 'make test'.
```

### Check to be sure it loads

```sh
$ php -d extension=modules/tutorial.so --re tutorial
Extension [ <persistent> extension #40 tutorial version <no_version> ] {
}
```

