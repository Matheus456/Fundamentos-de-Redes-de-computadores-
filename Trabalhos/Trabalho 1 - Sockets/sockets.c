#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> /*Para tratar erro*/

/* Bibliotecas para sockets */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define PORTA 2000
#define LEN 4096

struct sockaddr_in local;
struct sockaddr_in remoto;
/*
Biblioteca -> netinet

struct sockaddr_in {
short            sin_family;   // e.g. AF_INET (Familia)
unsigned short   sin_port;     // e.g. htons(3490) (Porta)
struct in_addr   sin_addr;     // see struct in_addr, below (IP)
char             sin_zero[8];  // zero this if you want to (Zera a estrutura)
};
*/

int main(int argc, char const *argv[]) {
  int sockfd; /* descritor socket (para ler e escrever dados que foram abertos na comunicação) */
  int cliente; /* Decrição do cliente*/
  int len = sizeof(remoto);
  int slen;
  char buffer[4096];

  /* Criando o socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
   /*
    socket(tipo de domínio, tipo do protocolo de transporte, tipo do protocolo de rede)

    AF_INET e SOCK_STREAM são constantes.
    AF_INET(Address Family) -> Tipo de endereço que o seu socket irá utilizar, nesse caso o IPV4.

    IPV4 -> 32 bits.
    IPV6 -> 64 bits.

    SOCK_STREAM -> TCP.
    SOCK_DGRAM -> UDP.

    int socket(int domain(sin_name), int type, int protocol)
    0 -> Protocolo ip

    Alguns protocolos:
      0 - IP - INTERNET PROTOCOL
      1 - ICMP - INTERNET CONTROL MESSAGE PROTOCOL
      2 - IGMP - INTERNET GROUP MULTICAST PROTOCOL
      3 - GGP - GATEWAY-GATEWAY PROTOCOL
      6 - TCP - TRANSMISSION CONTROL PROTOCOL
      17 - UDP - USER DATAGRAMA PROTOCOL
    */

  if(sockfd < 0)/*Em caso de erro, socket retorna "-1"*/
  {
    perror("socket ");
    exit(1);
  }

  else
    printf("Socket criado com sucesso!\n");


  local.sin_family      = AF_INET; /*Tipo de domínio*/
  local.sin_port        = htons(PORTA);
  //local.sin_addr.s_addr = inet_addr("192.168.254.1"); /*Com o uso do IP*/
  /*
    htons -> host to network short, short trabalha com 2 bytes. Existe:
      htonl -> long, entre outros...
  */

  memset(local.sin_zero, 0x0/*ou 0*/, 8);

  if(bind(sockfd, (struct sockaddr*)&local, sizeof(local)) == -1)
  {
    perror("bind ");
    exit(1);
  }
  /*
    Função bind -> Associa o endereço IP a uma porta.
  */

  listen(sockfd, 5); /*Uma porta por vez*/

  if((cliente = accept(sockfd, (struct sockaddr*)&remoto, &len)) == -1)
  {
    perror("accept ");
    exit(1);
  }

  strcpy(buffer, "Welcome to jungle!\n\0");

  if(send(cliente, buffer, strlen(buffer), 0)) /*Função de escrita*/
  {
    printf("Aguardando resposta do cliente\n");

    while (1)
    {
      memset(buffer, 0x0, LEN);

      if((slen = recv(cliente, buffer, LEN, 0)) > 0) /*Função de leitura, menor igual a 0 é quando não recebe*/
      {
        char op[3], num1[100], num2[100], resultado[1000];
        int result_i;
        double result_db;
        sscanf(buffer, "%s %s %s", op, num1, num2);
        buffer[slen-1] = '\0';
        printf("op = %s\nnum1 = %s\nnum2 = %s\n", op, num1, num2);

        if(strcmp(op, "add") == 0){
          result_i = atoi(num1) + atoi(num2);
          printf("Resultado = %d\n", result_i);
          sprintf(resultado, "1 %d\n", result_i);
          send(cliente, resultado, strlen(resultado), 0);
        }

        else if(strcmp(op, "sub") == 0)
        {
          result_i = atoi(num1) - atoi(num2);
          printf("Resultado = %d\n", result_i);
          sprintf(resultado, "1 %d\n", result_i);
          send(cliente, resultado, strlen(resultado), 0);
        }

        else if(strcmp(op, "div") == 0)
        {
          if(atoi(num2) == 0)
          {
            perror("Divisao por zero ");
            send(cliente, "0 Divisao por zero!\n", 21, 0);
          }
          else
          {
            result_db = atoi(num1)/(double)atoi(num2);
            printf("Resultado = %.3lf\n", result_db);
            sprintf(resultado, "1 %.3lf\n", result_db);
            send(cliente, resultado, strlen(resultado), 0);
          }
        }
        else if(strcmp(op, "mul") == 0)
        {
          result_i = atoi(num1) * atoi(num2);
          printf("Resultado = %d\n", result_i);
          sprintf(resultado, "1 %d\n", result_i);
          send(cliente, resultado, strlen(resultado), 0);
        }
        printf("Mensagem recebida: %s\n", buffer);
      }
    }
  }
  
  close(sockfd);
  printf("Servidor encerrado!\n");
  return 0;
}
