#ifndef X0_STRING_ESCAPES_H
#define X0_STRING_ESCAPES_H
#define FOO(s) \
  asm(".pushsection \".section_name\", \"MS\",%progbits,1\n" \
      ".asciz \"" s "\"\n" \
      ".popsection \n");
#endif
