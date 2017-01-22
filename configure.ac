dnl Copyright (C) 2016, 2017 Ben Asselstine
dnl 
dnl This program is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation, either version 3 of the License, or
dnl (at your option) any later version.
dnl 
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([fituvalu],[0.0.3],[fituvalu@nym.hush.com])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE
AC_PREREQ(2.60)
AM_CONFIG_HEADER(config.h:config.in)
AC_CONFIG_SRCDIR([src/gen-progression.c])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC

AC_CHECK_LIB(gmp, __gmpz_mul_si, , [AC_MSG_ERROR( [GNU MP not found, or not 3.1 or up, see https://gmplib.org/])]) 

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC

AC_CONFIG_FILES([Makefile 
                 src/Makefile
                 tests/Makefile])
AC_OUTPUT
