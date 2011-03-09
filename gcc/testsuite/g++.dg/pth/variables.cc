extern int gbl_init_extern;		// need body			pass
extern int gbl_uninit_extern;		// head only			pass
int gbl_tentative;			// need body			pass
int gbl_initial = 1;			// need body			pass
extern const int gbl_extern_const;	// head only			pass
const float gbl_init_const = 1.5;	// need body			pass
const int gbl_manifest = 2;		// lazy body	merge head	pass
struct D {
    static int mbr_init_plain;		// head only			pass
    static int mbr_uninit_plain;	// head only			pass
    static const int mbr_init_const;	// head only			pass
    static const int mbr_uninit_const;	// head only			pass
    static const int mbr_manifest = 3;	// lazy body	merge head	okay
};
int D::mbr_init_plain = 4;		// need body	merge body	pass
int D::mbr_uninit_plain;		// need body	merge body	FAIL
const int D::mbr_init_const = 5;	// need body	merge body	FAIL
