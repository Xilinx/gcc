/* Copyright (C) 2006, 2007 Free Software Foundation, Inc.

   This file is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3 of the License, or (at your option) 
   any later version.

   This file is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#ifndef OBJECT_FORMAT_ELF
 #error elf.h included before elfos.h
#endif

#define BSS_SECTION_ASM_OP "\t.section .bss"

#define ASM_OUTPUT_ALIGNED_BSS(FILE, DECL, NAME, SIZE, ALIGN) \
            asm_output_aligned_bss (FILE, DECL, NAME, SIZE, ALIGN)



/* Provide a STARTFILE_SPEC appropriate for GNU/Linux.  Here we add
   the GNU/Linux magical crtbegin.o file (see crtstuff.c) which
   provides part of the support for getting C++ file-scope static
   object constructed before entering `main'.  */

#undef  STARTFILE_SPEC 
#define STARTFILE_SPEC "%{mstdmain: crt2.o%s} %{!mstdmain: crt1.o%s} \
			crti.o%s crtbegin.o%s"

#undef  ENDFILE_SPEC
#define ENDFILE_SPEC   "crtend.o%s crtn.o%s"

#define PREFERRED_DEBUGGING_TYPE DWARF2_DEBUG

#define DWARF2_DEBUGGING_INFO 1
#define DWARF2_ASM_LINE_DEBUG_INFO 1

#define SET_ASM_OP		"\t.set\t"

#undef TARGET_ASM_NAMED_SECTION
#define TARGET_ASM_NAMED_SECTION  default_elf_asm_named_section

#define EH_FRAME_IN_DATA_SECTION 1

#define DRIVER_SELF_SPECS "\
  %{mcache-size=128   : -lgcc_cache128k ; \
    mcache-size=64    : -lgcc_cache64k ; \
    mcache-size=32    : -lgcc_cache32k ; \
    mcache-size=16    : -lgcc_cache16k ; \
    mcache-size=8     : -lgcc_cache8k ; \
    		      : -lgcc_cache64k } \
  %<mcache-size=* \
  %{mno-atomic-updates:-lgcc_cachemgr_nonatomic; :-lgcc_cachemgr} \
  %<matomic-updates %<mno-atomic-updates"

#define LINK_SPEC "%{mlarge-mem: --defsym __stack=0xfffffff0 }"

/* Match each of the mutually exclusive cache<n>k libraries because
   lgcc_cache* did not seem to work -- perhaps a bug in the specs
   handling?  */
#define LIB_SPEC "-( %{!shared:%{g*:-lg}} -lc -lgloss -) \
    %{lgcc_cachemgr*:-lgcc_cachemgr%*} \
    %{lgcc_cache128k} %{lgcc_cache64k} %{lgcc_cache32k} \
    %{lgcc_cache16k} %{lgcc_cache8k}"

/* Turn off warnings in the assembler too. */
#undef ASM_SPEC
#define ASM_SPEC  "%{w:-W}"

