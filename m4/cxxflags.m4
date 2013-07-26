AC_PREREQ([2.60])

# AC_CXX_FLAGS
# ------------
# Attempts to recognize specific compilers to set, if availables, extra
# flags for debugging, optimization and strict conformance to language
# specifications.

# This macro checks for the following compilers :
#   - GNU C++ (g++)
#   - Sun WorkShop C++ (Sun/CC)
#   - Intel C++ (icc)
#   - Comeau C++ (como)
# and sets the following autoconf variables:
#   CXXFLAGS_DEBUG
#   CXXFLAGS_STRICT
#   CXXFLAGS_STRICT_ERRORS
#   CXXFLAGS_OPTIMIZE
AC_DEFUN([AC_CXX_FLAGS],
[dnl
   AC_REQUIRE([AC_PROG_CXX])
   AC_LANG_PUSH([C++])
   AC_CACHE_CHECK([for C++ compiler-specific extra flags],
                  [ac_cv_cxx_style],
                  [ac_cv_cxx_style=unknown
                   if $CXX -V 2>&1 | grep -i "Intel(R) C++">/dev/null 2>&1;
                   then
                      ac_cv_cxx_style=Intel
                   elif test "x$ac_compiler_gnu" != xno; then
                      if $CXX --version | grep '^2\.' >/dev/null ; then
                        ac_cv_cxx_style=weakGNU
                      else
                        ac_cv_cxx_style=GNU
                      fi
                   elif $CXX -V 2>&1 | grep -i "WorkShop">/dev/null 2>&1; then
                      ac_cv_cxx_style=Sun
                   else
                      echo "int main() {}" >conftest.cc
                      if $CXX --version conftest.cc 2>&1 \
                         | grep -i "Comeau C/C++" >/dev/null 2>&1; then
                         ac_cv_cxx_style=Comeau
                      fi
                      rm -f conftest.*
                   fi])
   AC_LANG_POP([C++])

   case $ac_cv_cxx_style in
     GNU)
      _CXXFLAGS_DEBUG="-g"
      _CXXFLAGS_OPTIMIZE="-O3 -finline-limit-1500"
      _CXXFLAGS_STRICT="-W -Wall"
      _CXXFLAGS_STRICT_ERRORS="-W -Wall -Werror"
      ;;
     weakGNU)
      _CXXFLAGS_DEBUG="-g"
      _CXXFLAGS_OPTIMIZE="-O2 -felide-constructors -funroll-loops"
      _CXXFLAGS_STRICT="-W -Wall"
      _CXXFLAGS_STRICT_ERRORS="-W -Wall -Werror"
      ;;
     Sun)
      _CXXFLAGS_DEBUG="-g"
      _CXXFLAGS_OPTIMIZE="-fast"
      _CXXFLAGS_STRICT="-v"
      _CXXFLAGS_STRICT_ERRORS="-v -xwe"
      ;;
     Comeau)
      _CXXFLAGS_DEBUG="-g"
      _CXXFLAGS_STRICT="-r"
      _CXXFLAGS_STRICT_ERRORS="-r"
      ;;
     Intel)
      _CXXFLAGS_OPTIMIZE="-O3"
      _CXXFLAGS_DEBUG="-g"
      _CXXFLAGS_STRICT="-wd111,193,279,383,444,654,810,981,1418,327"
      _CXXFLAGS_STRICT_ERRORS="-wd111,193,279,383,444,654,810,981,1418,327"
      ;;
   esac

   if test "x$CXXFLAGS_OPTIMIZE" = "x"; then
        CXXFLAGS_OPTIMIZE=$_CXXFLAGS_OPTIMIZE
   fi
   if test "x$CXXFLAGS_DEBUG" = "x"; then
        CXXFLAGS_DEBUG=$_CXXFLAGS_DEBUG
   fi
   if test "x$CXXFLAGS_STRICT" = "x"; then
        CXXFLAGS_STRICT=$_CXXFLAGS_STRICT
   fi
   if test "x$CXXFLAGS_STRICT_ERRORS" = "x"; then
        CXXFLAGS_STRICT_ERRORS=$_CXXFLAGS_STRICT_ERRORS
   fi

   AC_SUBST([CXXFLAGS_DEBUG])
   AC_SUBST([CXXFLAGS_OPTIMIZE])
   AC_SUBST([CXXFLAGS_STRICT])
   AC_SUBST([CXXFLAGS_STRICT_ERRORS])
])
