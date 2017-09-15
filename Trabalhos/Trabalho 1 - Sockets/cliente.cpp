#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>

int main (int argc, char* const argv[])
{
	int socket_id;
	struct sockaddr_in servidorAddr;
	int length;
	unsigned short servidorPorta;
	char *IP_Servidor;

	IP_Servidor = argv[1];
	servidorPorta = atoi(argv[2]);

	printf("Abrindo o socket para o cliente... \n");

	while(1)
	{
		char mensagem[1000];
		scanf(" %[^\n]", mensagem);

		if(strcmp(mensagem, "^C"))
		{
			socket_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if(socket_id < 0)
			{
				fprintf(stderr, "Erro na criacao do socket!\n");
				exit(0);
			}

			memset(&servidorAddr, 0, sizeof(servidorAddr)); // Zerando a estrutura de dados
			servidorAddr.sin_family = AF_INET;
			servidorAddr.sin_addr.s_addr = inet_addr(IP_Servidor);
			servidorAddr.sin_port = htons(servidorPorta);
			if(connect(socket_id, (struct sockaddr *) &servidorAddr, sizeof(servidorAddr)) < 0)
			{
				fprintf(stderr, "Erro na conexao!\n");
				exit(0);
			}

				length = strlen(mensagem) + 1;
				send(socket_id, &length, sizeof(length), 0);
				send(socket_id, mensagem, length, 0);
				int resultado, erro;
				recv(socket_id, &erro, sizeof(erro), 0); //ler a mensagem
				recv(socket_id, &resultado, sizeof(resultado), 0); //ler a mensagem
				printf("%d %d\n", erro, resultado);
		}
		close(socket_id);
	}
	return 0;
}
