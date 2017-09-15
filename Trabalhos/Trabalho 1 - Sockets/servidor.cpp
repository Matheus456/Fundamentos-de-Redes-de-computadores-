// Servidor Local
#include <stdio.h>
#include <stdlib.h>
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

	printf(stderr, "Abrindo o socket local... ");

	/*Criando o socket*/
	socket_id = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	/*Erro na criação do socket*/
	if(socket_id < 0)
	{
		printf(stderr, "Erro na criacao do socket!\n");
		exit(0);
	}
	printf(stderr, "Feito!\n");
	printf(stderr, "Ligando o socket a porta %d... ", servidorPorta);

	/*Configurando o socket: Familia, Porta e endereço ip*/
	memset(&servidorAddr, 0, sizeof(servidorAddr)); // Zerando a estrutura de dados
	servidorAddr.sin_family = AF_INET;
	servidorAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servidorAddr.sin_port = htons(servidorPorta);

	/*Associando o endereço IP a uma porta*/
	if(bind(socket_id, (struct sockaddr *) &servidorAddr, sizeof(servidorAddr)) < 0)
	{
		printf(stderr, "Erro na ligacao!\n");
		exit(0);
	}
	printf(stderr, "Feito!\n");


	printf(stderr, "Tornando o socket passivo (para virar um servidor)... ");
	if(listen(socket_id, 10) < 0)
	{
		printf(stderr, "Erro!\n");
		exit(0);
	}
	printf(stderr, "Feito!\n");

	while(1)
	{
		int socketCliente;
		struct sockaddr_in clienteAddr;
		unsigned int clienteLength;

		printf(stderr, "Aguardando a conexao de um cliente... ");
		clienteLength = sizeof(clienteAddr);

		if((socketCliente = accept(socket_id, (struct sockaddr *) &clienteAddr, &clienteLength)) < 0)
			printf(stderr, "Falha no accept().\n");
		printf(stderr, "Feito!\n");

		print_client_message(socketCliente);

		close(socketCliente);
	}
	return 0;
}

void sigint_handler(int signum)
{
	printf(stderr, "\nRecebido o sinal CTRL+C... vamos desligar o servidor!\n");
	end_server();
}

void print_client_message(int client_socket)
{
	int length;
	char *text;
	printf(stderr, "\nMensagem enviada pelo cliente tem ");
	read(client_socket, &length, sizeof (length));
	printf(stderr, "%d bytes.", length);
	text = (char*) malloc (length);
	read(client_socket, text, length);
	printf(stderr,"\n\n   Mensagem = %s\n\n", text);
	free (text);
	if (!strcmp (text, "sair"))
	{
		printf(stderr, "Cliente pediu para o servidor fechar.\n");
		end_server();
	}
}

void end_server(void)
{
	printf(stderr, "Fechando o socket local... ");
	close(socket_id);
	printf(stderr, "Feito!\n");
	exit(0);
}
