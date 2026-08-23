/**
 * Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Bare utilities to get various kind of inet service
 */

#include <assert.h>
#include <getopt.h>
#include <libgen.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool searchServ = false;
bool searchProto = false;
char *progName = NULL;

static void doPrintUsage(FILE *);
static void doSearchProto(void);
static void doSearchServ(void);

int main(int argc, char *argv[]) {
  struct option long_options[] = {{"help", no_argument, 0, 'h'},
				  {"proto", no_argument, 0, 'p'},
				  {"serv", no_argument, 0, 's'},
				  {0, 0, 0, 0}};
  
  progName = basename(argv[0]);

  int c;
  while ((c = getopt_long(argc, argv, "hps", long_options, NULL)) != -1) {
    switch (c) {
    case 'h':
      doPrintUsage(stdout);
      exit(0);

    case 'p':
      searchProto = true;
      break;

    case 's':
      searchServ = true;
      break;
      
    case '?':
      doPrintUsage(stderr);
      exit(1);
    }
  }

  if (searchProto) {
    doSearchProto();
  }

  if (searchServ) {
    doSearchServ();
  }

  exit(0);
}

void doPrintUsage(FILE *out) {
  assert(NULL != out);

  fprintf(out, "%s: usage: %s [directory|file]\n", progName, progName);
}

void doSearchProto(void) {
  struct protoent *ent = NULL;

  setprotoent(0);

  ent = getprotoent();
  while (NULL != ent) {
    printf("name = %s, proto = %d",
	   ent->p_name, ent->p_proto);
	   
    if (NULL != ent->p_aliases && NULL != ent->p_aliases[0]) {
      printf(", alias = ");

      int index = 0;
      do {
        printf("%s", ent->p_aliases[index++]);

	if (NULL == ent->p_aliases[index]) {
          break;
	} else {
          printf(", ");
	}
      } while (true);
    }

    printf("\n");

    ent = getprotoent();
  }
	 
  endprotoent();
}

void doSearchServ(void) {
  struct servent *ent = NULL;

  setservent(0);

  ent = getservent();
  while (NULL != ent) {
    printf("name = %s, port = %d, proto = %s",
	   ent->s_name, ent->s_port, ent->s_proto);
	   
    if (NULL != ent->s_aliases && NULL != ent->s_aliases[0]) {
      printf(", alias = ");

      int index = 0;
      do {
        printf("%s", ent->s_aliases[index++]);

	if (NULL == ent->s_aliases[index]) {
          break;
	} else {
          printf(", ");
	}
      } while (true);
    }

    printf("\n");

    ent = getservent();
  }
	 
  endservent();
}
