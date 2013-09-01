#ifndef __WASABI_COMPARE_H_
#define __WASABI_COMPARE_H_

// Declares all comparison operators on a type for you using specified compare function.

#define DECL_COMPARE_OPERATORS(CLASS,COMPARE) \
inline int operator <(const CLASS &a, const CLASS &b)  { return COMPARE(a,b) <  0; } \
inline int operator >(const CLASS &a, const CLASS &b)  { return COMPARE(a,b) >  0; } \
inline int operator <=(const CLASS &a, const CLASS &b) { return COMPARE(a,b) <= 0; } \
inline int operator >=(const CLASS &a, const CLASS &b) { return COMPARE(a,b) >= 0; } \
inline int operator ==(const CLASS &a, const CLASS &b) { return COMPARE(a,b) == 0; } \
inline int operator !=(const CLASS &a, const CLASS &b) { return COMPARE(a,b) != 0; }

#endif//__WASABI_COMPARE_H_
