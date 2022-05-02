#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

int main() {
  int sockfd, cli_fd;
  struct sockaddr_in addr;
  int addr_len = sizeof(struct sockaddr_in);
  char buffer[256];
  char *send = "server sendto";

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8080);
  addr.sin_addr.s_addr = htonl(INADDR_ANY) ;
  bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
  listen(sockfd, 1);
  cli_fd = accept(sockfd, NULL, NULL);

  memset(buffer, 0, sizeof(buffer));
  read(cli_fd, buffer, sizeof(buffer));
  write(cli_fd, send, strlen(send));

  close(cli_fd);
  close(sockfd);
  return 0;
}
