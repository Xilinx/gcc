/* Taken from MESA library */

struct gl_accum_attrib {
	float ClearColor[4];	/* Accumulation buffer clear color */
};

typedef struct gl_context {
  struct gl_accum_attrib Accum;
} GLcontext;

void gl_ClearAccum( GLcontext *ctx )
{
   ctx->Accum.ClearColor[3] = 1.0;
}

int main()
{
    GLcontext *bob = (GLcontext *) malloc( sizeof(GLcontext) );
    gl_ClearAccum( bob );

	if (bob->Accum.ClearColor[3] != 1.0) return 1;

	free(bob);
    return 0;
}
