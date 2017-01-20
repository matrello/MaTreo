// Stub out unused macros
#define weak_alias(x,y)
#define strong_alias(x,y)

#define __HI32(x) *((u_int32_t *) &x)
#define __LO32(x) *((u_int32_t *) &x + 1)

/* Get two 32 bit ints from a double.  */
#define EXTRACT_WORDS(ix0,ix1,d)                                \
do {															\
	(ix0) = __HI32(d);											\
	(ix1) = __LO32(d);											\
} while (0)

/* Get the more significant 32 bit int from a double.  */
#define GET_HIGH_WORD(i,d)                                      \
do {                                                            \
	(i) = __HI32(d);											\
} while (0)

/* Get the less significant 32 bit int from a double.  */
#define GET_LOW_WORD(i,d)                                     	\
do {                                                            \
	(i) = __LO32(d);											\
} while (0)

/* Set a double from two 32 bit ints.  */
#define INSERT_WORDS(d,ix0,ix1)                                 \
do {                                                            \
	__HI32(d) = (ix0);											\
	__LO32(d) = (ix1);											\
} while (0)

/* Set the more significant 32 bits of a double from an int.  */
#define SET_HIGH_WORD(d,v)		                                \
do {                                                            \
	__HI32(d) = (v);											\
} while (0)

/* Set the less significant 32 bits of a double from an int.  */
#define SET_LOW_WORD(d,v)		                                \
do {                                                            \
	__LO32(d) = (v);											\
} while (0)
