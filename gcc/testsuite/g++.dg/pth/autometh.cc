class base {
    virtual int method() {
        return 0;
    }
    int field;
};
void function() {
    base var1;
    base var2( var1 );
    var1 = var2;
}
