/**
 * Grant current hard thread to specified scheduler. The specified
 * scheduler must be non-null and a registered scheduler of the
 * current scheduler. This function never returns unless child is
 * NULL, in which case it sets errno appropriately and returns -1.
 */
#ifdef __PIC__
#define lithe_sched_enter(child)                                      \
({                                                                    \
  asm volatile ("mov %0, %%rdi;"                                      \
		"jmp ___lithe_sched_enter@PLT"                        \
		: : "r" (child));                                     \
  -1;                                                                 \
})
#else
#define lithe_sched_enter(child)                                      \
({                                                                    \
  asm volatile ("mov %0, %%rdi;"                                      \
                "jmp ___lithe_sched_enter"                            \
		: : "r" (child));                                     \
  -1;                                                                 \
})
#endif /* __PIC__ */

/**
 * Yield current hard thread to parent scheduler. This function should
 * never return.
 */
#ifdef __PIC__
#define lithe_sched_yield()                                           \
({                                                                    \
  asm volatile ("jmp ___lithe_sched_yield@PLT");                      \
  -1;                                                                 \
})
#else
#define lithe_sched_yield()                                           \
({                                                                    \
  asm volatile ("jmp ___lithe_sched_yield");                          \
  -1;                                                                 \
})
#endif /* __PIC__ */

/**
 * Reenter current scheduler. This function should never return.
 */
#ifdef __PIC__
#define lithe_sched_reenter()                                         \
({                                                                    \
  asm volatile ("jmp ___lithe_sched_reenter@PLT");                    \
  -1;                                                                 \
})
#else
#define lithe_sched_reenter()                                         \
({                                                                    \
  asm volatile ("jmp ___lithe_sched_reenter");                        \
  -1;                                                                 \
})
#endif /* __PIC__ */
