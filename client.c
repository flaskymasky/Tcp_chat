#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define handle(str) \
        do { perror(str);exit(EXIT_FAILURE);}while(0)

static __thread char buffer[4096];

void *reader(void*a)
{
        int *sockfd = (int*)a;
        char buffer[4096];
        char full[4096];

        for (;; ) {

                read(*sockfd, buffer, sizeof(buffer));

                snprintf(full, sizeof(full), "%sfriend%s: %s\n",KGRN, KNRM, buffer);

                write(STDOUT_FILENO, full, strlen(full));
                write(STDOUT_FILENO, "\n", sizeof("\n"));

                memset(buffer, 0, sizeof(buffer));

        }

        pthread_exit(NULL);

}
void *writer(void*a)
{
        int *sockfd = (int*)a;
        char color_on[256];
        char color_off[256];

        sprintf(color_on, "%s", KCYN);
        sprintf(color_off, "%s", KNRM);

        for (;;) {


                write(STDOUT_FILENO, color_on, strlen(color_on));

                read(STDIN_FILENO, buffer, sizeof(buffer));
                write(STDOUT_FILENO, "\n", sizeof("\n"));

                write(STDOUT_FILENO, color_off, strlen(color_off));

                write(*sockfd, buffer, strlen(buffer));

                memset(buffer, 0, sizeof(buffer));
        }

        pthread_exit(NULL);
}

int main(int argc, char **argv)
{
        pthread_t t1, t2;
        int s1,s2;

        int sockfd;
        struct sockaddr_in servaddr;

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(13);

        if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) == -1)
                handle("inet_pton");

        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                handle("socket");

        if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
                handle("connect");

        s1 = pthread_create(&t1, NULL, reader, (void*)&sockfd);
        s2 = pthread_create(&t2, NULL, writer, (void*)&sockfd);
        if (s1 != 0 || s2 != 0)
                handle("pthread_create");



        pthread_exit(NULL);
}
