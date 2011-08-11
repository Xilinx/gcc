#ifndef A0TMPLCLASS2_S_H
#define A0TMPLCLASS2_S_H

template<>
short base< short >::variable = 2;

template<>
short base< short >::dynamic_non_inline()
{ return 2 * field; }

template<>
short base< short >::static_non_inline()
{ return 2 * variable; }


double base< double >::d_variable = 3;

double base< double >::d_dynamic_non_inline()
{ return d_field; }

double base< double >::d_static_non_inline()
{ return d_variable; }

#endif
