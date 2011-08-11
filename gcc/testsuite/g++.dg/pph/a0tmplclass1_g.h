#ifndef A0TMPLCLASS1_G_H
#define A0TMPLCLASS1_G_H

template< typename T >
struct base
{
    T field;
    static T variable;

    T dynamic_in_class() { return field; }
    inline T dynamic_early_inline();
    T dynamic_late_inline();
    T dynamic_non_inline();

    static T static_in_class() { return variable; }
    static inline T static_early_inline();
    static T static_late_inline();
    static T static_non_inline();
};

template< typename T >
T base< T >::variable = 1;

template< typename T >
T base< T >::dynamic_early_inline()
{ return field; }

template< typename T >
inline T base< T >::dynamic_late_inline()
{ return field; }

template< typename T >
T base< T >::dynamic_non_inline()
{ return field; }

template< typename T >
T base< T >::static_early_inline()
{ return variable; }

template< typename T >
inline T base< T >::static_late_inline()
{ return variable; }

template< typename T >
T base< T >::static_non_inline()
{ return variable; }


inline char instantiator1(char c)
{
    base<char> variable;
    return
        variable.dynamic_in_class() + base<char>::static_in_class()
        + variable.dynamic_early_inline() + base<char>::static_early_inline()
        + variable.dynamic_late_inline() + base<char>::static_late_inline()
        + variable.dynamic_non_inline() + base<char>::static_non_inline();
}

#endif
