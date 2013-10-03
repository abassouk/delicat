/* 
   delicat - Allows concatenation of streams so that de-concatenation is possible.

   Copyright (C) 2013 Tassos Bassoukos

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

*/

#include <stdio.h>
#include <sys/types.h>
#include <strings.h>
#include "system.h"
#include "delicat-opt.h"
#define VOID void
#include "xmalloc.h"

#define BUFFER_LENGTH 4096
#define TOKEN_LENGTH 8

static char TOKEN[TOKEN_LENGTH]="abqd\1fgh";
static char buffer[BUFFER_LENGTH];
static int read_size = 1;

static unsigned int buffer_read = 0;
static unsigned int buffer_pos = 0;

static char out_buffer[BUFFER_LENGTH];
static int out_size;

static int read_all(void *ptr,size_t size){
  size_t len,remain=size;
  while(remain>0 && (len=read(0,ptr,remain))>0){
    ptr    += len;
    remain -= len;
  }
  if(len<0) {
    return -1;
  }
  return size-remain;
}

static void write_all(char *buffer, int size) {
  int len;
  while(size>0 && (len = write(1, buffer, size))>0){
    buffer += len;
    size   -= len;
  }
  if(size>0){
    perror("writing values");
    exit(1);
  }
}

static void write_flush() {
  write_all(out_buffer,out_size);
  out_size=0;
}

static void write_values(char *values, int amount){
  do {
    int size = amount+out_size>BUFFER_LENGTH?BUFFER_LENGTH-out_size:amount;
    amount-=size;
    char *buf = out_buffer+out_size;
    out_size += size;
    while(size-->0) *buf++ = *values++;
    if(out_size==BUFFER_LENGTH) write_flush();
  } while(amount>0);
}

static int token_depth=0;
static void pack_next(int nextChar){
  if(nextChar==TOKEN[token_depth] && ++token_depth<TOKEN_LENGTH){
    return;
  }
  if(token_depth>0){
    write_values(TOKEN,token_depth);
    if(token_depth==TOKEN_LENGTH){
      char tmp=1;
      write_values(&tmp,1);
    }
    token_depth=0;
  }
  if(nextChar == -1){
      // EOF: write token + 0;
      write_values(TOKEN,TOKEN_LENGTH);
      char tmp=0;
      write_values(&tmp,1);
      if(!HAVE_OPT(NO_PAD)){
        // then write buffer_length zeroes
        bzero(buffer,BUFFER_LENGTH);
        write_values(buffer,BUFFER_LENGTH);
      }
      write_flush();
      exit(0);
      return;
  }
  char tmp=nextChar;
  write_values(&tmp,1);
}

static void unpack_next(int nextChar){
  if(token_depth==8) {
    if(nextChar==-1){
      // should not happen;
      fprintf(stderr,"Error: Premature end of input stream");
      write_flush();
      exit(1);
      return;
    } else if(nextChar==0) {
      write_flush();
      if(!HAVE_OPT(NO_PAD)){
        buffer_read-=buffer_pos;
        read_all(buffer,BUFFER_LENGTH-buffer_read);
      }
      exit(0);
      return;
    } else {
        // ignore non-0 character.
    	// pass-through to token_depth reset.
    }
  } else if(nextChar==TOKEN[token_depth]){
    ++token_depth;
    return;
  }
  if(token_depth>0){
    write_values(TOKEN,token_depth);
    token_depth=0;
  }
  if(nextChar == -1){
      // should not happen;
      fprintf(stderr,"Error: Premature end of input stream\n");
      write_flush();
      exit(1);
      return;
  }
  char tmp=nextChar;
  write_values(&tmp,1);
}

static int next_char() {
  if(buffer_pos==buffer_read){
    buffer_pos=0;
    buffer_read=read_all(buffer,read_size);
    if(buffer_read==0){
      return -1;
    }
    if(buffer_read==-1){
      perror("reading stdin");
      write_flush();
      exit(1);
    }
  }
  return buffer[buffer_pos++] & 0xff;
}

int main (int argc, char **argv) {
  {
    int arg_ct = optionProcess( &delicatOptions, argc, argv );
    argc -= arg_ct;
        argv += arg_ct;
  }
  read_size = HAVE_OPT(NO_PAD)?1:BUFFER_LENGTH;
  if(HAVE_OPT(REVERSE)){
    int n;
    do {
      n = next_char();
      unpack_next(n);
    } while(n!=-1);
  } else {
    int n;
    do {
      n = next_char();
      pack_next(n);
    } while(n!=-1);
  }
  return EXIT_SUCCESS;
}
