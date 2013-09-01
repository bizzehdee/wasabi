# Automake macros for wasabi

# WA_SCAN_WASABICFG(wasabicfg_path)
# checks for #define WASABI_COMPILE_* in $(wasabicfg_path)/wasabicfg.h
# and sets wa_*=true for each.
#----------------------------------------
AC_DEFUN([WA_SCAN_WASABICFG],
[if test ! -f $1/wasabicfg.h ; then
  AC_MSG_ERROR([$1/wasabicfg.h not found])
fi
for x in `sed -n \
          '/^[[:blank:]]*\#define[[:blank:]]*WASABI_COMPILE_\([[:alnum:]]*\).*$/s//\1/p' \
          $1/wasabicfg.h`;
do
  AC_MSG_NOTICE([enabling wasabi api: $x])
  declare wa_${x}=true
done
])
