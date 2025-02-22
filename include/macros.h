#ifndef _MACROS_H_
#define _MACROS_H_

#ifndef __sgi
#define GLOBAL_ASM(...)
#endif

#if !defined(__sgi) && (!defined(NON_MATCHING) || !defined(AVOID_UB))
// asm-process isn't supported outside of IDO, and undefined behavior causes
// crashes.
#error Matching build is only possible on IDO; please build with NON_MATCHING=1.
#endif

#define ARRAY_COUNT(arr) (s32)(sizeof(arr) / sizeof(arr[0]))

#define GLUE(a, b) a ## b
#define GLUE2(a, b) GLUE(a, b)

// Avoid compiler warnings for unused variables
#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

// Avoid undefined behaviour for non-returning functions
#ifdef __GNUC__
#define NORETURN __attribute__((noreturn))
#else
#define NORETURN
#endif

// Static assertions
#ifdef __GNUC__
#define STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#else
#define STATIC_ASSERT(cond, msg) typedef char GLUE2(static_assertion_failed, __LINE__)[(cond) ? 1 : -1]
#endif


#ifdef __GNUC__
#define ALIGNED(x) __attribute__((aligned(x)))
#else
#define ALIGNED(x)
#endif

// Align to 8-byte boundary for DMA requirements
#ifdef __GNUC__
#define ALIGNED8 __attribute__((aligned(8)))
#else
#define ALIGNED8
#endif

// Align to 16-byte boundary for audio lib requirements
#ifdef __GNUC__
#define ALIGNED16 __attribute__((aligned(16)))
#else
#define ALIGNED16
#endif

// convert a virtual address to physical.
#define VIRTUAL_TO_PHYSICAL(addr)   ((uintptr_t)(addr) & 0x1FFFFFFF)

// convert a physical address to virtual.
#define PHYSICAL_TO_VIRTUAL(addr)   ((uintptr_t)(addr) | 0x80000000)

// another way of converting virtual to physical
#define VIRTUAL_TO_PHYSICAL2(addr)  ((u8 *)(addr) - 0x80000000U)

#if defined(__sgi)
    #define ABSF(x) (x < 0.f ? -x : x)
#else
    #define ABSF(x) fabsf(x)
    #define INLINE static inline
#endif

// Used to suppress warnings in the ./generate_ctx.sh script.
#define INCONSISTENT 

// Used to make a u32 colour value look clearer. Transforms 0xFF0000FF to 255, 0, 0, 255
#define COLOUR_RGBA32(r, g, b, a) (((r << 24) | (g << 16) |  (b << 8) | a))

#define ALIGN8(val) (((val) + 7) & ~7)

#define STACK(stack, size) \
    u64 stack[ALIGN8(size) / sizeof(u64)]

#define STACK_START(stack) \
    ((u8*)(stack) + sizeof(stack))


#if !defined(__sgi)
INLINE void CreateDirtyExclusiveOne(void* addr) {
    asm volatile (
        "cache 0xD, 0x00(%0);"
        :            
        : "r"(addr)        
    );
}
INLINE void CreateDirtyExclusiveOneRoundUp(void* addr) {
    asm volatile (
        "cache 0xD, 0x08(%0);"
        :            
        : "r"(addr)        
    );
}
INLINE void CreateDirtyExclusiveTwoRoundUp(void* addr) {
    asm volatile (
        "cache 0xD, 0x08(%0);"
        "cache 0xD, 0x18(%0);"
        :            
        : "r"(addr)        
    );
}
 
INLINE void HitInvalidate(void* addr) {
    asm volatile (
        "cache 0x11, 0x00(%0);"
        :            
        : "r"(addr)        
    );
}
 
INLINE void HitWritebackInvalidate(void* addr) {
    asm volatile (
        "cache 0x15, 0x00(%0);"
        :            
        : "r"(addr)        
    );
}
INLINE void HitWritebackInvalidateTwo(void* addr) {
    asm volatile (
        "cache 0x15, 0x00(%0);"
        "cache 0x15, 0x10(%0);"
        :            
        : "r"(addr)        
    );
}
 
INLINE void HitWritebackInvalidateRoundUp(void* addr) {
    asm volatile (
        "cache 0x15, 0x08(%0);"
        :            
        : "r"(addr)        
    );
}
 
INLINE void HitWritebackInvalidateFour(void* addr) {
    asm volatile (
        "cache 0x15, 0x00(%0);"
        "cache 0x15, 0x10(%0);"
        "cache 0x15, 0x20(%0);"
        "cache 0x15, 0x30(%0);"
        :            
        : "r"(addr)        
    );
}
 
 
// Cen64 dev suggested this might be faster (Create_Dirty_Exclusive)
INLINE void CreateDirtyExclusiveTwo(void* addr) {
    asm volatile (
        "cache 0xD, 0x00(%0);"
        "cache 0xD, 0x10(%0);"
        :            
        : "r"(addr)        
    );
}
// Cen64 dev suggested this might be faster (Create_Dirty_Exclusive)
INLINE void CreateDirtyExclusiveThree(void* addr) {
    asm volatile (
        "cache 0xD, 0x00(%0);"
        "cache 0xD, 0x10(%0);"
        "cache 0xD, 0x20(%0);"
        :            
        : "r"(addr)        
    );
}
 
// Cen64 dev suggested this might be faster (Create_Dirty_Exclusive)
INLINE void CreateDirtyExclusiveFour(void* addr) {
    asm volatile (
        "cache 0xD, 0x00(%0);"
        "cache 0xD, 0x10(%0);"
        "cache 0xD, 0x20(%0);"
        "cache 0xD, 0x30(%0);"
        :            
        : "r"(addr)        
    );
}
 
INLINE void InvalidateICacheLine(void* addr) {
    asm volatile (
        "cache 0x10, 0x00(%0);"
        :            
        : "r"(addr)        
    );
}
#endif

#endif
