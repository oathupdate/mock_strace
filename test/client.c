#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

int main ()
{
  int sockfd;
  struct sockaddr_in addr;
  int addr_len =sizeof(struct sockaddr_in);
  char buffer[256];
  char *send = "client sendto";

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8080);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));

  memset(buffer, 0, sizeof(buffer));
  write(sockfd, send, strlen(send));
  read(sockfd, buffer, sizeof(buffer));

  close(sockfd);
  return 0;
}
