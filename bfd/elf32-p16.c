/* BFD back-end for ISEL's P16 architecture (ELF)
   Copyright (C) 2007-2025 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "bfdlink.h"
#include "libbfd.h"
#include "libiberty.h"
#include "elf-bfd.h"
#include "elf/p16.h"
#include "elf32-p16.h"

/* Map between the BFD relocation types to P16 ELF relocation types */
struct p16_reloc_map
{
  bfd_reloc_code_real_type bfd_reloc_enum; /* BFD relocation */
  unsigned short p16_reloc_type; /* P16 relocation type */
};

/* The map */
static const struct p16_reloc_map p16_reloc_map[R_P16_MAX] =
{
	{BFD_RELOC_NONE,			R_P16_NONE},
	{BFD_RELOC_P16_UIMM3,		R_P16_UIMM3},
	{BFD_RELOC_P16_UIMM4,		R_P16_UIMM4},
	{BFD_RELOC_P16_UIMM4_EVEN,	R_P16_UIMM4_EVEN},
	{BFD_RELOC_P16_UIMM7_EVEN,	R_P16_UIMM7_EVEN},
	{BFD_RELOC_P16_UIMM8,		R_P16_UIMM8},
	{BFD_RELOC_P16_IMM11_EVEN,	R_P16_IMM11_EVEN},
};

static reloc_howto_type p16_elf_howto_table[] =
{
	HOWTO (
		R_P16_NONE,			/* type */
		0,					/* rightshift */
		0,					/* size */
		0,					/* bitsize */
		false,				/* pc_relative */
		0,					/* bitpos */
		complain_overflow_dont,		/* complain_on_overflow */
		bfd_elf_generic_reloc,		/* special_function */
		"R_P16_NONE",		/* name */
		false,				/* partial_inplace */
		0,					/* src_mask */
		0,					/* dst_mask */
		false				/* pcrel_offset */
	),
	HOWTO (
		R_P16_UIMM3,		/* type */
		0,					/* rightshift */
		1,					/* size */
		3,					/* bitsize */
		false,				/* pc_relative */
		7,					/* bitpos */
		complain_overflow_bitfield,	/* complain_on_overflow */
		bfd_elf_generic_reloc,		/* special_function */
		"R_P16_UIMM3",		/* name */
		false,				/* partial_inplace */
		0,					/* src_mask */
		0x0380,				/* dst_mask */
		false				/* pcrel_offset */
	),
	HOWTO (
		R_P16_UIMM4,		/* type */
		0,					/* rightshift */
		1,					/* size */
		4,					/* bitsize */
		false,				/* pc_relative */
		7,					/* bitpos */
		complain_overflow_bitfield,	/* complain_on_overflow */
		bfd_elf_generic_reloc,		/* special_function */
		"R_P16_UIMM4",		/* name */
		false,				/* partial_inplace */
		0,					/* src_mask */
		0x0780,				/* dst_mask */
		false				/* pcrel_offset */
	),
	HOWTO (
		R_P16_UIMM4_EVEN,	/* type */
		1,					/* rightshift */
		1,					/* size */
		3,					/* bitsize */
		false,				/* pc_relative */
		7,					/* bitpos */
		complain_overflow_bitfield,	/* complain_on_overflow */
		bfd_elf_generic_reloc,		/* special_function */
		"R_P16_UIMM4_EVEN",	/* name */
		false,				/* partial_inplace */
		0,					/* src_mask */
		0x0380,				/* dst_mask */
		false				/* pcrel_offset */
	),
	HOWTO (
		R_P16_UIMM7_EVEN,	/* type */
		1,					/* rightshift */
		1,					/* size */
		6,					/* bitsize */
		true,				/* pc_relative */
		4,					/* bitpos */
		complain_overflow_bitfield,	/* complain_on_overflow */
		bfd_elf_generic_reloc,		/* special_function */
		"R_P16_UIMM7_EVEN",	/* name */
		false,				/* partial_inplace */
		0,					/* src_mask */
		0x7F,				/* dst_mask */
		true				/* pcrel_offset */
	),
	HOWTO (
		R_P16_UIMM8,		/* type */
		0,					/* rightshift */
		1,					/* size */
		8,					/* bitsize */
		false,				/* pc_relative */
		4,					/* bitpos */
		complain_overflow_bitfield,	/* complain_on_overflow */
		bfd_elf_generic_reloc,		/* special_function */
		"R_P16_UIMM8",		/* name */
		false,				/* partial_inplace */
		0,					/* src_mask */
		0x0FF0,				/* dst_mask */
		false				/* pcrel_offset */
	),
	HOWTO (
		R_P16_IMM11_EVEN,	/* type */
		1,					/* rightshift */
		2,					/* size */
		10,					/* bitsize */
		true,				/* pc_relative */
		0,					/* bitpos */
		complain_overflow_bitfield,	/* complain_on_overflow */
		bfd_elf_generic_reloc,		/* special_function */
		"R_P16_IMM11_EVEN",	/* name */
		false,				/* partial_inplace */
		0,					/* src_mask */
		0x3FF,				/* dst_mask */
		true				/* pcrel_offset */
	),
};

/* Retrieve a howto pointer from an internal relocation entry */
static bool elf_p16_info_to_howto(
	bfd *abfd,
	arelent *cache_ptr,
	Elf_Internal_Rela *dst
) {
  unsigned int r_type = ELF32_R_TYPE (dst->r_info);

  if (r_type >= R_P16_MAX)
    {
      /* xgettext:c-format */
      _bfd_error_handler (_("%pB: unsupported relocation type %#x"),
			  abfd, r_type);
      bfd_set_error (bfd_error_bad_value);
      return false;
    }

  cache_ptr->howto = p16_elf_howto_table + r_type;
  return true;
}

/* Retrieves a howto pointer from a BFD relocation code*/
static reloc_howto_type *elf_p16_reloc_type_lookup(
	bfd *abfd,
	bfd_reloc_code_real_type code
) {
  unsigned int i;

  for (i = 0; i < R_P16_MAX; i++)
    if (code == p16_reloc_map[i].bfd_reloc_enum)
      return &p16_elf_howto_table[p16_reloc_map[i].p16_reloc_type];

  _bfd_error_handler (_("%pB: unsupported relocation type %#x"),
		      abfd, code);
  return NULL;
}

/* Retrieves a howto pointer by searching its name */
static reloc_howto_type *elf_p16_reloc_name_lookup(
	bfd *abfd ATTRIBUTE_UNUSED,
	const char *r_name
) {
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE (p16_elf_howto_table); i++)
    if (p16_elf_howto_table[i].name != NULL
	&& strcasecmp (p16_elf_howto_table[i].name, r_name) == 0)
      return p16_elf_howto_table + i;

  return NULL;
}

/* Performs a single relocation, called from elf32_p16_relocate_section */
static bfd_reloc_status_type p16_elf_final_link_relocate(
	reloc_howto_type *howto,
	bfd *input_bfd,
	bfd *output_bfd ATTRIBUTE_UNUSED,
	asection *input_section,
	bfd_byte *contents,
	bfd_vma offset,
	bfd_vma Rvalue,
	bfd_vma addend,
	struct bfd_link_info *info ATTRIBUTE_UNUSED,
	asection *sec ATTRIBUTE_UNUSED,
	int is_local ATTRIBUTE_UNUSED	
) {
	/* An important quirk of the P16 processor is that when an instruction is being processed,
	the program counter already advances to the next instruction, example:
	
	some_label:
		add r0, r1, r2
		b some_label
		sub r0, r1, r2

	
	The value used in the branch would be -4 bytes instead of -2 bytes, since the program
	counter is already in the sub instruction (needs to go 2 instructions/4 bytes upwards)
	*/

	if (howto->pc_relative) {
		addend -= 2;
	}

	bfd_reloc_status_type r;

	r = _bfd_final_link_relocate(
		howto, input_bfd, input_section,
		contents, offset,
		Rvalue, addend);
	
	return r;
}

/* Relocate a P16 ELF section. */
static int elf32_p16_relocate_section (
	bfd *output_bfd,
	struct bfd_link_info *info,
	bfd *input_bfd,
	asection *input_section,
	bfd_byte *contents,
	Elf_Internal_Rela *relocs,
	Elf_Internal_Sym *local_syms,
	asection **local_sections
) {
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *rel, *relend;

  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);

  rel = relocs;
  relend = relocs + input_section->reloc_count;
  for (; rel < relend; rel++)
    {
      int r_type;
      reloc_howto_type *howto;
      unsigned long r_symndx;
      Elf_Internal_Sym *sym;
      asection *sec;
      struct elf_link_hash_entry *h;
      bfd_vma relocation;
      bfd_reloc_status_type r;

      r_symndx = ELF32_R_SYM (rel->r_info);
      r_type = ELF32_R_TYPE (rel->r_info);
      howto = p16_elf_howto_table + (r_type);

      h = NULL;
      sym = NULL;
      sec = NULL;
      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections[r_symndx];
	  relocation = _bfd_elf_rela_local_sym (output_bfd, sym, &sec, rel);
	}
      else
	{
	  bool unresolved_reloc, warned, ignored;

	  RELOC_FOR_GLOBAL_SYMBOL (info, input_bfd, input_section, rel,
				   r_symndx, symtab_hdr, sym_hashes,
				   h, sec, relocation,
				   unresolved_reloc, warned, ignored);
	}

      if (sec != NULL && discarded_section (sec))
	RELOC_AGAINST_DISCARDED_SECTION (info, input_bfd, input_section,
					 rel, 1, relend, howto, 0, contents);

      if (bfd_link_relocatable (info))
	continue;

      r = p16_elf_final_link_relocate (howto, input_bfd, output_bfd,
					input_section,
					contents, rel->r_offset,
					relocation, rel->r_addend,
					info, sec, h == NULL);

      if (r != bfd_reloc_ok)
	{
	  const char *name;
	  const char *msg = NULL;

	  if (h != NULL)
	    name = h->root.root.string;
	  else
	    {
	      name = (bfd_elf_string_from_elf_section
		      (input_bfd, symtab_hdr->sh_link, sym->st_name));
	      if (name == NULL || *name == '\0')
		name = bfd_section_name (sec);
	    }

	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      (*info->callbacks->reloc_overflow)
		(info, (h ? &h->root : NULL), name, howto->name,
		 (bfd_vma) 0, input_bfd, input_section, rel->r_offset);
	      break;

	    case bfd_reloc_undefined:
	      (*info->callbacks->undefined_symbol)
		(info, name, input_bfd, input_section, rel->r_offset, true);
	      break;

	    case bfd_reloc_outofrange:
	      msg = _("internal error: out of range error");
	      goto common_error;

	    case bfd_reloc_notsupported:
	      msg = _("internal error: unsupported relocation error");
	      goto common_error;

	    case bfd_reloc_dangerous:
	      msg = _("internal error: dangerous error");
	      goto common_error;

	    default:
	      msg = _("internal error: unknown error");
	      /* Fall through.  */

	    common_error:
	      (*info->callbacks->warning) (info, msg, name, input_bfd,
					   input_section, rel->r_offset);
	      break;
	    }
	}
    }

  return true;
}

/* This is a version of bfd_generic_get_relocated_section_contents
   which uses elf32_p16_relocate_section. */
static bfd_byte *elf32_p16_get_relocated_section_contents(
	bfd *output_bfd,
	struct bfd_link_info *link_info,
	struct bfd_link_order *link_order,
	bfd_byte *data,
	bool relocatable,
	asymbol **symbols
) {
	 Elf_Internal_Shdr *symtab_hdr;
	 asection *input_section = link_order->u.indirect.section;
	 bfd *input_bfd = input_section->owner;
	 asection **sections = NULL;
	 Elf_Internal_Rela *internal_relocs = NULL;
	 Elf_Internal_Sym *isymbuf = NULL;
   
	 /* We only need to handle the case of relaxing, or of having a
		particular set of section contents, specially.  */
	 if (relocatable
		 || elf_section_data (input_section)->this_hdr.contents == NULL)
	   return bfd_generic_get_relocated_section_contents (output_bfd, link_info,
								  link_order, data,
								  relocatable,
								  symbols);
   
	 symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
   
	 bfd_byte *orig_data = data;
	 if (data == NULL)
	   {
		 data = bfd_malloc (input_section->size);
		 if (data == NULL)
	   return NULL;
	   }
	 memcpy (data, elf_section_data (input_section)->this_hdr.contents,
		 (size_t) input_section->size);
   
	 if ((input_section->flags & SEC_RELOC) != 0
		 && input_section->reloc_count > 0)
	   {
		 Elf_Internal_Sym *isym;
		 Elf_Internal_Sym *isymend;
		 asection **secpp;
		 bfd_size_type amt;
   
		 internal_relocs = _bfd_elf_link_read_relocs (input_bfd, input_section,
							  NULL, NULL, false);
		 if (internal_relocs == NULL)
	   goto error_return;
   
		 if (symtab_hdr->sh_info != 0)
	   {
		 isymbuf = (Elf_Internal_Sym *) symtab_hdr->contents;
		 if (isymbuf == NULL)
		   isymbuf = bfd_elf_get_elf_syms (input_bfd, symtab_hdr,
						   symtab_hdr->sh_info, 0,
						   NULL, NULL, NULL);
		 if (isymbuf == NULL)
		   goto error_return;
	   }
   
		 amt = symtab_hdr->sh_info;
		 amt *= sizeof (asection *);
		 sections = bfd_malloc (amt);
		 if (sections == NULL && amt != 0)
	   goto error_return;
   
		 isymend = isymbuf + symtab_hdr->sh_info;
		 for (isym = isymbuf, secpp = sections; isym < isymend; ++isym, ++secpp)
	   {
		 asection *isec;
   
		 if (isym->st_shndx == SHN_UNDEF)
		   isec = bfd_und_section_ptr;
		 else if (isym->st_shndx == SHN_ABS)
		   isec = bfd_abs_section_ptr;
		 else if (isym->st_shndx == SHN_COMMON)
		   isec = bfd_com_section_ptr;
		 else
		   isec = bfd_section_from_elf_index (input_bfd, isym->st_shndx);
   
		 *secpp = isec;
	   }
   
		 if (! elf32_p16_relocate_section (output_bfd, link_info, input_bfd,
						input_section, data, internal_relocs,
						isymbuf, sections))
	   goto error_return;
   
		 free (sections);
		 if (symtab_hdr->contents != (unsigned char *) isymbuf)
	   free (isymbuf);
		 if (elf_section_data (input_section)->relocs != internal_relocs)
	   free (internal_relocs);
	   }
   
	 return data;
   
	error_return:
	 free (sections);
	 if (symtab_hdr->contents != (unsigned char *) isymbuf)
	   free (isymbuf);
	 if (elf_section_data (input_section)->relocs != internal_relocs)
	   free (internal_relocs);
	 if (orig_data == NULL)
	   free (data);
	 return NULL;
}

/* Definitions for setting P16 target vector.  */
#define TARGET_LITTLE_SYM		p16_elf32_vec
#define TARGET_LITTLE_NAME		"elf32-p16"

#define ELF_ARCH				bfd_arch_p16
#define ELF_TARGET_ID			P16_ELF_DATA
#define ELF_MACHINE_CODE 		EM_P16
#define ELF_MAXPAGESIZE  		0x1
#define elf_symbol_leading_char '_'

#define elf_info_to_howto		elf_p16_info_to_howto
#define elf_info_to_howto_rel	NULL  

#define bfd_elf32_bfd_reloc_type_lookup elf_p16_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup elf_p16_reloc_name_lookup

#define elf_backend_relocate_section	elf32_p16_relocate_section
// #define elf_backend_gc_mark_hook 
// #define elf_backend_gc_sweep_hook 
#define elf_backend_can_gc_sections		1
#define elf_backend_rela_normal			1
// #define elf_backend_check_relocs   
// #define elf_backend_final_write_processing
// #define elf_backend_object_p 
// #define elf_backend_create_dynamic_sections
// #define elf_backend_adjust_dynamic_symbol 
// #define elf_backend_size_dynamic_sections 
// #define elf_backend_omit_section_dynsym 
// #define elf_backend_finish_dynamic_sections 
// #define elf_backend_reloc_type_class  
// #define elf_backend_want_got_plt     
// #define elf_backend_plt_readonly    
// #define elf_backend_want_plt_sym   
// #define elf_backend_got_header_size 

#define bfd_elf32_bfd_relax_section		bfd_generic_relax_section
#define bfd_elf32_bfd_get_relocated_section_contents \
		elf32_p16_get_relocated_section_contents
// #define bfd_elf32_bfd_merge_private_bfd_data
// #define bfd_elf32_bfd_link_hash_table_create 
// #define bfd_elf32_bfd_link_hash_table_free 

#include "elf32-target.h"