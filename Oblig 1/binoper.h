/* File: binoper.h */

#ifndef _BINOPER_H
#define _BINOPER_H

#define MASK_SIMPLE         0xF00F
#define MASK_BIT_N(n)       (1 << n)
#define MASK_NOT_BIT_N(n)   ~(1 << n)

/* Toggles a bit within an N-bit value ON or OFF.
 * In other words:
 *      Sets the specified bit to the oposite
 *      value of what it originally was.
 *
 * Example:
 *      TOGGLE_BIT_N(0xFF, 2)
 *
 *          1111 1111   (0xFF)
 *      ^   0000 0100   (1 << 2)
 *          ---------
 *      =   1111 1011   (0xFB)
 *          =========
 *
 *      By calling TOGGLE_BIT_N(0xFB, 2)
 *      we will get 0xFF back.
 */
#define TOGGLE_BIT_N(a, n)  (a ^= MASK_BIT_N(n))

/* Value of this macro is 1 if the specified bit
 * is set, else it will be 0.
 *
 * Example:
 *      GET_BIT_N(0x30, 4)
 *
 *          0011 0000   (0x30)
 *      &   0001 0000   (1 << 4)
 *          ---------
 *      >>4 0001 0000   (0x10)
 *          ---------
 *      =   0000 0001   (0x01)
 *          =========
 *
 *      We get 1 as a result which means that bit
 *      number 4 (range 0 - 7) was set.
 */
#define GET_BIT_N(a, n)     ((a & MASK_BIT_N(n)) >> n)

/* Sets the specified bit regardless to what its
 * previous value was.
 *
 * Example:
 *      SET_BIT_N(0x7F, 7)
 *
 *          0111 1111   (0x7F, 7)
 *      |   1000 0000   (1 << 7)
 *          ---------
 *      =   1111 1111   (0xFF)
 *          =========
 *
 *      If the would have tried to do
 *      SET_BIT_N(0xFF, 7) we would still get
 *      0xFF due to how the or operation works.
 */
#define SET_BIT_N(a, n)     (a | MASK_BIT_N(n))

/* A redefention of the type "unsigned char" */
typedef unsigned char byte_t;

/* Prototypes for the functions defined in binoper.c */
byte_t and(byte_t a, byte_t b);
byte_t or(byte_t a, byte_t b);
byte_t xor(byte_t a, byte_t b);
byte_t not(byte_t a);
byte_t lshift(byte_t a, byte_t n);
byte_t rshift(byte_t a, byte_t n);

#endif
