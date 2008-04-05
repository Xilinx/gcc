/* Taken from MESA library */

struct gl_accum_attrib {
	float ClearColor[4];	/* Accumulation buffer clear color */
};

typedef struct gl_context {
  struct gl_accum_attrib Accum;
} GLcontext;

void gl_ClearAccum( GLcontext *ctx )
{
   ctx->Accum.ClearColor[4] = 1.0;
}

int main()
{
    GLcontext bob;
    gl_ClearAccum( &bob );

    if (bob.Accum.ClearColor[4] != 1.0) return 1;
    return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
