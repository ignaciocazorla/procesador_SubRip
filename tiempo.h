#ifndef TIEMPO_H
#define TIEMPO_H 1
#include <stdlib.h>

typedef struct {
  short hh;    /* Horas. */
  short mm;    /* Minutos.	[0-59] */
  short ss;	  /* Segundos.	[0-59] (1 leap second) */
  short ms;		/* Milisegundos. [0-999]*/
} t_tiempo;

size_t * tm_to_millisec(short hs, short mins, short segs, short msegs);

void millisec_to_tm(size_t millis, t_tiempo* tm);

#endif
