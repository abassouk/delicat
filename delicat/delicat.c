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

int permaread(void *ptr,size_t size){
  size_t len;
  while((len=read(0,ptr,size))>0){
    ptr+=len;
    size-=len;
  }
  if(len<0)
    return -1;
  return 0;
}

void write_values(char *values, int amount){
  if(fwrite(values,1,amount,stdout)!=amount){
    perror("writing values");
    exit(1);
  }
}

int token_depth=0;
void pack_next(int nextChar){
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
      return;
  }
  char tmp=nextChar;
  write_values(&tmp,1);
}

void unpack_next(int nextChar){
  if(token_depth==8) {
    if(nextChar==-1){
      // should not happen;
      fprintf(stderr,"Premature end of input stream");
      fflush(stdout);
      exit(1);
      return;
    } else if(nextChar==0) {
      fflush(stdout);
      if(!HAVE_OPT(NO_PAD)){
        buffer_read-=buffer_pos;
        permaread(buffer,BUFFER_LENGTH-buffer_read);
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
      write_values(TOKEN,TOKEN_LENGTH);
      char tmp=0;
      write_values(&tmp,1);
      bzero(buffer,BUFFER_LENGTH);
      write_values(buffer,BUFFER_LENGTH);
      return;
  }
  char tmp=nextChar;
  write_values(&tmp,1);
}

int next_char() {
  if(buffer_pos==buffer_read){
    buffer_pos=0;
    buffer_read=read(0,buffer,read_size);
    if(buffer_read==0){
      return -1;
    }
  }
  return buffer[buffer_pos++];
}

int main (int argc, char **argv) {
  {
    int arg_ct = optionProcess( &delicatOptions, argc, argv );
    argc -= arg_ct;
        argv += arg_ct;
  }
  read_size = HAVE_OPT(NO_PAD)?1:BUFFER_LENGTH;
  if(HAVE_OPT(REVERSE)){
    do {
      int n = next_char();
      unpack_next(n);
      if(n == -1)
        break;
    } while(1);
  } else {
    do {
      int n = next_char();
      pack_next(n);
      if(n == -1)
        break;
    } while(1);
  }
  return EXIT_SUCCESS;
}
