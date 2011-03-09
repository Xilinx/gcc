struct D {
    int method() { return mbr; }
    static int mbr;
};
int D::mbr = 4;
