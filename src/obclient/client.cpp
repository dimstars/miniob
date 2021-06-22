/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA */

//
// Created by Longda on 2021
//

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "common/defs.h"
#include "common/lang/string.h"

#define MAX_MEM_BUFFER_SIZE 8192
#define PORT_DEFAULT 6789

using namespace common;

bool is_exit_command(const char *cmd) {
  return 0 == strncasecmp("exit", cmd, 4) ||
         0 == strncasecmp("bye", cmd, 3);
}

int main(int argc, char *argv[]) {
  char *server_host = (char *)LOCAL_HOST;
  if (argc >= 2) {
    server_host = argv[1];
  }
  int server_port = PORT_DEFAULT;
  if (argc >= 3) {
    server_port = atoi(argv[2]);
  }

  const char *prompt_str = "miniob > ";

  int sockfd, send_bytes;
  // char send[MAXLINE];

  char send_buf[MAX_MEM_BUFFER_SIZE];
  // char buf[MAXDATASIZE];
  struct hostent *host;
  struct sockaddr_in serv_addr;

  if ((host = gethostbyname(server_host)) == NULL) {
    fprintf(stderr, "gethostbyname failed. errmsg=%d:%s\n", errno, strerror(errno));
    exit(1);
  }
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    fprintf(stderr, "create socket error. errmsg=%d:%s\n", errno, strerror(errno));
    exit(1);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(server_port);
  serv_addr.sin_addr = *((struct in_addr *)host->h_addr);
  bzero(&(serv_addr.sin_zero), 8);

  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) ==
      -1) {
    fprintf(stderr, "Failed to connect. errmsg=%d:%s\n", errno, strerror(errno));
    exit(1);
  }

  fputs(prompt_str, stdout);
  while (fgets(send_buf, MAX_MEM_BUFFER_SIZE, stdin) != NULL) {
    if (common::is_blank(send_buf)) {
      fputs(prompt_str, stdout);
      continue;
    }

    if (is_exit_command(send_buf)) {
      break;
    }

    if ((send_bytes = send(sockfd, send_buf, strlen(send_buf) + 1, 0)) == -1) {
      fprintf(stderr, "send error: %d:%s \n", errno, strerror(errno));
      exit(1);
    }
    memset(send_buf, 0, sizeof(send_buf));

    int len = 0;
    bool broken = true;
    while((len = recv(sockfd, send_buf, MAX_MEM_BUFFER_SIZE, 0)) > 0){  
      for (int i = 0; i < len; i++) {
        printf("%c", send_buf[i]);
      }
      if (send_buf[len - 1] != '\n') {
        putchar('\n');
      }
      if(send_buf[len]==0){
        break;
      }
      memset(send_buf, 0, MAX_MEM_BUFFER_SIZE);
    }

    if (len < 0 && broken) {
      fprintf(stderr, "Connection was broken: %s\n", strerror(errno));
      break;
    }
    if (0 == len) {
      printf("Connection has been closed\n");
      break;
    }
    fputs(prompt_str, stdout);
  }
  close(sockfd);

  return 0;
}
