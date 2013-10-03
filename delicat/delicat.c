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

#include <termios.h>
#include <grp.h>
#include <pwd.h>
*/

#include <stdio.h>
#include <sys/types.h>
#include <strings.h>
#include <assert.h>
#include "system.h"
#include "delicat-opt.h"

char *xmalloc ();
char *xrealloc ();
char *xstrdup ();

#define BUFFER_LENGTH 4096
#define TOKEN_LENGTH 8
#define TOKEN_EOF 1
#define TOKEN_EXIST 2

static char TOKEN[TOKEN_LENGTH]="abqd\0fgh";
static char token_buffer[BUFFER_LENGTH+TOKEN_LENGTH+1]; 
static char*buffer=&token_buffer[TOKEN_LENGTH];

static unsigned int in_stream_position = 0;
static unsigned int out_stream_position = 0;
static unsigned int buffer_read = 0;
static unsigned int buffer_prefix = 0;


void write_values(char *values, int amount){
  if(write(1,values,amount)<0){
    // TODO: check for write<amount.
    perror("writing values");
    exit(1);
  }
  out_stream_position+=amount;
}



int find_next_token(){
   if(buffer_read+buffer_prefix<=TOKEN_LENGTH)
     return -1;
   char *buf = buffer-buffer_prefix;
   int i, max = buffer_read+buffer_prefix-TOKEN_LENGTH-1;
   for(i=0;i<=max;i++,buf++){
     if(bcmp(buf,TOKEN,TOKEN_LENGTH)==0){
	return i;
     }
   }
   return -1;
}

void skip_values(int amount){
   assert(amount>0);
   assert(amount<=buffer_read+buffer_prefix);

   in_stream_position+=amount;
   if(amount<buffer_prefix){
     buffer_prefix-=amount;
   } else {
     amount -= buffer_prefix;
     buffer_prefix=0;
     buffer_read-=amount;
     if(buffer_read>0){
	bcopy(&buffer[amount], buffer, buffer_read);
     }
   }
}

void copy_values(int amount){
   assert(amount>0);
   assert(amount<=buffer_read+buffer_prefix);
   if(write(1,buffer-buffer_prefix,amount)<0){
     // TODO: check for write<amount.
     perror("writing buffer");
     exit(1);
   }

   out_stream_position+=amount;
   skip_values(amount);
}

void write_EOF(){
  write_values(TOKEN,TOKEN_LENGTH);
  char tmp=0;
  write_values(&tmp,1);
  bzero(buffer,BUFFER_LENGTH);
  if(write(1,buffer,BUFFER_LENGTH)<0){
    // TODO: check for write<amount.
    perror("writing buffer");
    exit(1);
  }
}


int handle_token(int prefix_position, char *post_value){
  if(HAVE_OPT(REVERSE)){
    if(*post_value!=0){
      copy_values(prefix_position+TOKEN_LENGTH);
      skip_values(1);
    } else {
      copy_values(prefix_position);
      skip_values(TOKEN_LENGTH+1);
      int remain = BUFFER_LENGTH-buffer_read;
      skip_values(buffer_read);
      if(remain>0){
	int v = read(0,buffer,remain);
        if(v<=0) {
	  perror("Reading input");
	  exit(1);
        }
      }
      return 1;
    }
  } else {
    copy_values(prefix_position+TOKEN_LENGTH);
    char tmp='a';
    write_values(&tmp,1);
  }
  return 0;
}

int read_buffer(int size, int (*f)(char *)) {
   assert(size<=BUFFER_LENGTH);
   assert(buffer_read==0);
   assert(buffer_prefix>=0 && buffer_prefix<=TOKEN_LENGTH);
   int v = read(0,buffer,size);
   if(v<=0) {
	perror("Reading input");
	exit(1);
   }
   buffer_read = v;
   // find next token


      return 0;
}

int
main (int argc, char **argv)
{
  {
    int arg_ct = optionProcess( &delicatOptions, argc, argv );
    argc -= arg_ct;
        argv += arg_ct;
  }
  if(HAVE_OPT(REVERSE)){

  } else {

  }
  return EXIT_SUCCESS;
}
