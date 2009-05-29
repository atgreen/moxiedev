AC_INIT(configure.ac)

# Run a quick sanity check on the build environment before we get too
# far into the build.

rm -f FAIL

AC_CHECK_PROGS([bison], [bison], FAIL)
if test "x$bison" == xFAIL; then
  touch FAIL
  AC_MSG_ERROR([**** Install missing BISON tool ****]);
fi
AC_CHECK_PROGS([flex], [flex], FAIL)
if test "x$flex" == xFAIL; then
  touch FAIL
  AC_MSG_ERROR([**** Install missing FLEX tool ****]);
fi
AC_CHECK_PROGS([m4], [gm4 gnum4 m4], FAIL)
if test "x$m4" == xFAIL; then
  touch FAIL
  AC_MSG_ERROR([**** Install missing M4 tool ****]);
fi
AC_CHECK_PROGS([makeinfo], makeinfo, FAIL)
if test "x$makeinfo" == xFAIL; then
  touch FAIL
  AC_MSG_ERROR([**** Install missing MAKEINFO tool ****]);
fi
AC_CHECK_PROGS([expect], expect, FAIL)
if test "x$expect" == xFAIL; then
  touch FAIL
  AC_MSG_ERROR([**** Install missing EXPECT tool ****]);
fi
AC_CHECK_PROGS([runtest], runtest, FAIL)
if test "x$runtest" == xFAIL; then
  touch FAIL
  AC_MSG_ERROR([**** Install missing RUNTEST tool ****]);
fi
AC_CHECK_PROGS([gcc], gcc, FAIL)
if test "x$gcc" == xFAIL; then
  touch FAIL
  AC_MSG_ERROR([**** Install missing GCC tool ****]);
fi
AC_CHECK_PROGS([gperf], gperf, FAIL)
if test "x$gperf" == xFAIL; then
  touch FAIL
  AC_MSG_ERROR([**** Install missing GPERF tool ****]);
fi


