#ifndef A0TMPLCLASS2_U_H
#define A0TMPLCLASS2_U_H

double tmplclass_user2()
{
    base<char> variable1;
    base<int> variable2;
    base<double> variable3;
    return
        variable1.dynamic_in_class() + base<char>::static_in_class()
        + variable1.dynamic_early_inline() + base<char>::static_early_inline()
        + variable1.dynamic_late_inline() + base<char>::static_late_inline()
        + variable1.dynamic_non_inline() + base<char>::static_non_inline()
        + variable2.dynamic_in_class() + base<int>::static_in_class()
        + variable2.dynamic_early_inline() + base<int>::static_early_inline()
        + variable2.dynamic_late_inline() + base<int>::static_late_inline()
        + variable2.dynamic_non_inline() + base<int>::static_non_inline()
        + variable3.d_dynamic_in_class()
        + base<double>::d_static_in_class()
        + variable3.d_dynamic_early_inline()
        + base<double>::d_static_early_inline()
        + variable3.d_dynamic_late_inline()
        + base<double>::d_static_late_inline()
        + variable3.d_dynamic_non_inline()
        + base<double>::d_static_non_inline();
}

#endif

