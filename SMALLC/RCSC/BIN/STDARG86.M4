divert(-1)
#********************************************************************
#
#M4 definitions for simulating ANSI variable arguments macros
#8086 Version
#Copyright 1997 Andy W. K. Yuen
#
#********************************************************************
define(`va_list', `int *')
define(`va_end', `')
define(`va_arg', `*$1--')
define(`va_start', `$1 = CCARGC() + &$2 - 1')
divert
