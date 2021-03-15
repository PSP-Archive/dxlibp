/*
 * liblzr.h
 * LZR Decompress Library
 * Version 0.11 by BenHur - http://www.psp-programming.com/benhur
 *
 * This work is licensed under the Creative Commons Attribution-Share Alike 3.0 License.
 */

#ifndef LIBLZR_H__
#define LIBLZR_H__

#ifdef __cplusplus
extern "C" {
#endif

int LZRDecompress(void *out, unsigned int out_capacity, void *in, void *in_end);

#ifdef __cplusplus
}
#endif

#endif