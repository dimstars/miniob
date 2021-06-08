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
#include "common/metrics/metrics.h"
#include "common/metrics/metrics_registry.h"
#include "common/metrics/console_reporter.h"

#define MAX_MEM_BUFFER_SIZE 8192
#define PORT_DEFAULT 66789

using namespace common;
char *serverHost = (char *)LOCAL_HOST;

void* testServer(void *param) {
  Meter  *tpsMeter = (Meter *)param;

  std::cout << "Begin to connect server. " << std::endl;
  int sockfd, sendbytes;
  // char send[MAXLINE];

  char sendBuf[MAX_MEM_BUFFER_SIZE] = {0};
  strcpy(sendBuf, "select count(*) from test");
  // char buf[MAXDATASIZE];
  struct hostent *host;
  struct sockaddr_in serv_addr;

  if ((host = gethostbyname(serverHost)) == NULL) {
    perror("gethostbyname");
    exit(1);
  }
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket error \n");
    exit(1);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT_DEFAULT);
  serv_addr.sin_addr = *((struct in_addr *)host->h_addr);
  bzero(&(serv_addr.sin_zero), 8);

  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) ==
      -1) {
    perror("Failed to connect \n");
    exit(1);
  }

  while (true) {
    if ((sendbytes = send(sockfd, sendBuf, strlen(sendBuf) + 1, 0)) == -1) {
      perror("send error \n");
      exit(1);
    }

    memset(sendBuf, 0, sizeof(sendBuf));

    int len = recv(sockfd, sendBuf, MAX_MEM_BUFFER_SIZE, 0);
    if (len < 0) {
      printf("Close connection");
      break;
    }

    tpsMeter->inc();
  }
  close(sockfd);
  return NULL;
}

int main(int argc, char *argv[]) {

  if (argc >= 2) {
    serverHost = argv[1];
  }

  MetricsRegistry &metricRegistry = get_metrics_registry();
  ConsoleReporter *consoleReporter = get_console_reporter();
  metricRegistry.add_reporter(consoleReporter);

  Meter  *tpsMeter = new Meter();

  metricRegistry.register_metric("client.sendtps", tpsMeter);

  for (int i = 0; i < 8; i++) {
    pthread_t pid;
    pthread_create(&pid, NULL, testServer, tpsMeter);
  }

  while(1) {
    sleep(60);
    metricRegistry.snapshot();
    metricRegistry.report();
  }
  return 0;
}
