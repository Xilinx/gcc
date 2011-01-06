extern int extern_only(int);		//
extern int extern_then_body(int);	//
inline int extern_inline(int i)		// lazy body
{ return extern_then_body(i); }
int extern_then_body(int i)		// need body, merge head to body
{ return extern_only( i ); }
static int fwdref_static(int);
int fwdref_static(int i)
{ return extern_then_body( i ); }	// need body, merge head to body
struct type {
    int mbr_decl_only(int);
    int mbr_decl_then_def(int);
    inline int mbr_inl_then_def(int);
    int mbr_decl_inline(int i)		// lazy body
    { return mbr_decl_only( i ); }
    virtual int mbr_virtual_inline()	// lazy body, but circular dependence
    { return mbr_decl_only( 1 ); }
};
int type::mbr_decl_then_def(int i)	// need body
{ return mbr_decl_inline( i ); }
int type::mbr_inl_then_def(int i)	// lazy body
{ return mbr_decl_then_def( i ); }
