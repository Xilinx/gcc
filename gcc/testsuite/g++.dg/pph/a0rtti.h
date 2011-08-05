#ifndef A0RTTI_H
#define A0RTTI_H

struct polymorphic
{
    int field;
    virtual int method() { return field; }
    const std::type_info& hidden() { return typeid( field ); }
    virtual ~polymorphic() { }
};

struct non_polymorphic
{
    int field;
    int method() { return field; }
    const std::type_info& hidden() { return typeid( field ); }
    ~non_polymorphic() { }
};

#endif
