// Servidor Local
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

int socket_id;
void sigint_handler(int signum);
void print_client_message(int client_socket);
void end_server(void);

int main (int argc, char* const argv[])
{
	unsigned short servidorPorta;
	struct sockaddr_in servidorAddr;

	servidorPorta = atoi(argv[1]);

	/*Criando o socket*/
	socket_id = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	/*Erro na criação do socket*/
	if(socket_id < 0)
	{
		printf("Erro na criacao do socket!\n");
		exit(0);
	}
	printf("Feito!\n");
	printf("Ligando o socket a porta %d... ", servidorPorta);

	/*Configurando o socket: Familia, Porta e endereço ip*/
	memset(&servidorAddr, 0, sizeof(servidorAddr)); // Zerando a estrutura de dados
	servidorAddr.sin_family = AF_INET;
	servidorAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servidorAddr.sin_port = htons(servidorPorta);

	/*Associando o endereço IP a uma porta*/
	if(bind(socket_id, (struct sockaddr *) &servidorAddr, sizeof(servidorAddr)) < 0)
	{
		printf("Erro na ligacao!\n");
		exit(0);
	}

	if(listen(socket_id, 10) < 0)
	{
		exit(0);
	}

	while(1)
	{
		int socketCliente;
		struct sockaddr_in clienteAddr;
		unsigned int clienteLength;

		clienteLength = sizeof(clienteAddr);

		if((socketCliente = accept(socket_id, (struct sockaddr *) &clienteAddr, &clienteLength)) < 0)
			printf("Falha no accept().\n");

		print_client_message(socketCliente);

		close(socketCliente);
	}
	return 0;
}

void sigint_handler(int signum)
{
	end_server();
}

void print_client_message(int client_socket)
{
	int length;
	char *text;
	/*Tamanho da mensagem*/
	read(client_socket, &length, sizeof (length));

	/*Criando string e recebendo mensagem*/
	text = (char*) malloc (length);
	read(client_socket, text, length);

	free (text);
	if (!strcmp (text, "sair"))
	{
		end_server();
	}
}

void end_server(void)
{
	close(socket_id);
	exit(0);
}
