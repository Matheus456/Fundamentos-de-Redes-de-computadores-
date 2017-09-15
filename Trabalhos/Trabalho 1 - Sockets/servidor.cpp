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
void print_client_message(int socketCliente);
void end_server(void);
void operacao(char *buffer, int socketCliente);

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
		unsigned int clienteLength = sizeof(clienteAddr);


		if((socketCliente = accept(socket_id, (struct sockaddr *) &clienteAddr, &clienteLength)) < 0)
			printf("Falha no accept().\n");

		print_client_message(socketCliente);

		close(socketCliente);
	}
	return 0;
}


void print_client_message(int socketCliente)
{
	int length;
	char *buffer;
	/*Tamanho da mensagem*/
	recv(socketCliente, &length, sizeof (length),0); //ler o tamanho da mensagem a ser recebida
	buffer = (char*) malloc (length+1);
	/*Criando string e recebendo mensagem*/
	recv(socketCliente, buffer, length, 0); //ler a mensagem

	operacao(buffer, socketCliente);
}

void operacao(char *buffer, int socketCliente)
{
	char op[3];
	int result_i=0, flag_erro=0, erro = 0;
	long long num1, num2;
	sscanf(buffer, "%s %lld %lld", op, &num1, &num2);
	printf("op = %s\nnum1 = %lld\nnum2 = %lld\n", op, num1, num2);

	if(strcmp(op, "add") == 0)
		result_i = num1 + num2;

	else if(strcmp(op, "sub") == 0)
		result_i = num1 - num2;

	else if(strcmp(op, "div") == 0)
	{

		if(num2 == 0)
		{
			erro = 1;
			flag_erro = 1;
		}

		else
			result_i = num1/num2;
	}

	else if(strcmp(op, "mul") == 0)
		result_i = num1 * num2;


	else
	{
		flag_erro = 1;
		erro = 2;
	}

	switch (flag_erro) {
		case 0:
				send(socketCliente, &flag_erro, sizeof(flag_erro), 0);
				send(socketCliente, &result_i, sizeof(result_i), 0);
		break;

		case 1:
		send(socketCliente, &flag_erro, sizeof(flag_erro), 0);
		if(erro == 1)
			send(socketCliente, &erro, sizeof(erro), 0);

		else if(erro ==2)
			send(socketCliente, &erro, sizeof(erro), 0);
		break;
	}
	printf("Mensagem recebida: %s\nMensagem enviada: %d\n", buffer, result_i);
}
