#ifndef _GLOBAL_DEFINE_H_
#define _GLOBAL_DEFINE_H_

#ifdef __CPLUSPLUS
extern "C" {
#endif

#define TRUE 1
#define FALSE 0
#define INVALID_U32 0xFFFF
#define MAX_MESSAGE_LEN 367280
	 
//#define LOG_DEBUG  printf
//#define LOG_WARN   printf
//#define LOG_ERROR  printf
	/*global type defintions*/
	typedef unsigned char   u8_t;
	typedef unsigned short  u16_t;
	typedef unsigned int    u32_t;
	typedef unsigned char   i8_t;
	//typedef char     i8_t;
	typedef int             i32_t;
	typedef float           f32_t;
	typedef double          f64_t;
	typedef long double     f128_t;

	typedef unsigned char   BOOL;

	
	

#ifdef __CPLUSLUS
}
#endif

#endif /*_GLOBAL_DEFINE_H_*/