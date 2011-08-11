#ifndef A0TMPLCLASS2_G_H
#define A0TMPLCLASS2_G_H

template<>
short base< short >::variable;

template<>
short base< short >::dynamic_early_inline()
{ return 2 * field; }

template<>
inline short base< short >::dynamic_late_inline()
{ return 2 * field; }

template<>
short base< short >::static_early_inline()
{ return 2 * variable; }

template<>
inline short base< short >::static_late_inline()
{ return 2 * variable; }


template<>
struct base<double>
{
    double d_field;
    static double d_variable;

    double d_dynamic_in_class() { return d_field; }
    inline double d_dynamic_early_inline();
    double d_dynamic_late_inline();
    double d_dynamic_non_inline();

    static double d_static_in_class() { return d_variable; }
    static inline double d_static_early_inline();
    static double d_static_late_inline();
    static double d_static_non_inline();
};

double base< double >::d_dynamic_early_inline()
{ return d_field; }

inline double base< double >::d_dynamic_late_inline()
{ return d_field; }

double base< double >::d_static_early_inline()
{ return d_variable; }

inline double base< double >::d_static_late_inline()
{ return d_variable; }

#endif
