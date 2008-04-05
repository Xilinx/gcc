/* Taken from MESA library */

class gl_accum_attrib {
public:
	float ClearColor[4];	/* Accumulation buffer clear color */
};

class GLcontext {
public:
  gl_accum_attrib Accum;
};

void gl_ClearAccum( GLcontext *ctx )
{
   ctx->Accum.ClearColor[3] = 1.0;
}

int main()
{
    GLcontext bob;
    gl_ClearAccum( &bob );

	if (bob.Accum.ClearColor[3] != 1.0) return 1;
    return 0;
}

