#ifndef __ASM_H__
#define __ASM_H__

#ifndef ENTRY
#define ENTRY(name) \
        .globl name ASM_NL \
        ALIGN ASM_NL \
        name:
#endif


#ifndef ENDPROC
#define ENDPROC(name) \
        .type name, @function ASM_NL \
        END(name)
#endif

#ifndef END
#define END(name) \
        .size name, .-name
#endif

/* Some toolchains use other characters (e.g. '`') to mark new line in macro */
#ifndef ASM_NL
#define ASM_NL           ;
#endif

#define CTR_CWG_SHIFT           24
#define CTR_CWG_MASK            15

#define ALIGN __ALIGN
#define __ALIGN           .align 4


/* 64k page size. */
#define PAGE_SIZE (64 * 1024)

#endif
