/* File: binoper.c */

#include "binoper.h"

byte_t and(byte_t a, byte_t b) {
    return a & b;
}

byte_t or(byte_t a, byte_t b) {
    return a | b;
}

byte_t xor(byte_t a, byte_t b) {
    return a ^ b;
}

byte_t not(byte_t a){
    return ~a;
}

byte_t lshift(byte_t a, byte_t n) {
    return a << n;
}

byte_t rshift(byte_t a, byte_t n) {
    return a >> n;
}
