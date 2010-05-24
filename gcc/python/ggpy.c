#include "config.h"
#include "system.h"
#include "gcc.h"

#include "coretypes.h"
#include "tm.h"

void lang_specific_driver( int *argc, const char *const **argv,
			   int *in_added_libraries )
{
  return;
}

/* Called before linking.  Returns 0 on success and -1 on failure.  */
int lang_specific_pre_link( void )
{
  return 0;
}

/* Number of extra output files that lang_specific_pre_link may generate.  */
int lang_specific_extra_outfiles = 0;	
