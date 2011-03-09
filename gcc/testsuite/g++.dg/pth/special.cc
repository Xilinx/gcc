struct B {
    B( int );
    B( double );
    B( const B& );
    B& operator=( const B& );
    B& operator=( int );
    operator double();
    ~B();
};

struct D {
    D( const B& );
};

int F( D );
int G( double );

B b(1);
//B q("hhh");

int H() {
    F(b);
    B a(3.2);
    B c = b;
    B d(b);
    c = b;
    d = 4;
    //G(double(""));
    G(d);
}
