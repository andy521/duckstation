/*
 * time.h -- time declarations
 *
 * Copyright (C) 2004, 2005, 2006, 2007 Svend Sorensen
 * For license terms, see the file COPYING in this distribution.
 */

#ifndef CUETIME_H
#define CUETIME_H

long time_msf_to_frame(int m, int s, int f);
void time_frame_to_msf(long frame, int *m, int *s, int *f);
char *time_frame_to_mmssff(long f);

#endif
