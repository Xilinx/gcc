struct base {
    int field;
};
base function();
int base_from_var() {
    base variable;
}
int base_from_func() {
    function();
}
struct derived : base {
    int method();
};
int derived::method() {
    return field;
}
struct vderived : virtual base {
};
