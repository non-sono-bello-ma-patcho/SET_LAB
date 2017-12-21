/* 
 * incApache_aux.c: funzioni ausiliarie per il web server
 *
 * Programma sviluppato a supporto del laboratorio di
 * Sistemi di Elaborazione e Trasmissione del corso di laurea
 * in Informatica classe L-31 presso l'Universita` degli Studi di
 * Genova.
 *
 * Copyright (C) 2012-2014, 2016 by Giovanni Chiola <chiolag@acm.org>
 * Copyright (C) 2015-2016 by Giovanni Lagorio <giovanni.lagorio@unige.it>
 * Copyright (C) 2017 by Giovanni Chiola <chiolag@acm.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "incApache.h"

void fail(const char *const msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}

void fail_errno(const char *const msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

void *my_malloc(size_t size)
{
	void *result = malloc(size);
	if (!result)
		fail_errno("Cannot allocate memory with malloc");
	return result;
}

char *my_strdup(const char *const s)
{
	char *result = strdup(s);
	if (!result)
		fail_errno("Cannot allocate memory for strdup");
	return result;
}

