#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Bibliotecas para sockets */
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>


#define PORTA 2000

void Mensagem_Cliente(int socketCliente);
void operacao(char *mensagem, int socketCliente);

int main (int argc, char* const argv[])
{

	int socket_id; /* descritor socket (para ler e escrever dados que foram abertos na comunicação) */
	struct sockaddr_in servidorAddr;

	/*
	struct sockaddr_in {
	short            sin_family;   // e.g. AF_INET (Familia)
	unsigned short   sin_port;     // e.g. htons(3490) (Porta)
	struct in_addr   sin_addr;     // see struct in_addr, below (IP)
	char             sin_zero[8];  // zero this if you want to (Zera a estrutura)
	};
	*/

	/*Criando o socket*/
	socket_id = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	/*Erro na criação do socket*/
	if(socket_id < 0)
	{
		printf("Nao foi possivel criar o socket!\n");
		exit(0);
	}

	printf("Socket criado com sucesso!\nLigando o socket a porta %d.\n", PORTA);

	/*Configurando o socket: Familia, Porta e endereço ip*/
	memset(&servidorAddr, 0, sizeof(servidorAddr)); // Zerando a estrutura de dados

	servidorAddr.sin_family = AF_INET; /*Tipo de domínio*/
	servidorAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servidorAddr.sin_port = htons(PORTA);
	/*
	local.sin_addr.s_addr = inet_addr("192.168.254.1"); /*Com o uso do IP
    htons -> host to network short, short trabalha com 2 bytes. Existe:
    htonl -> long, entre outros...
  */

	/*Associando o endereço IP a uma porta*/
	if(bind(socket_id, (struct sockaddr *) &servidorAddr, sizeof(servidorAddr)) < 0)
	{
		printf("Erro na ligacao!\n");
		exit(0);
	}

	/*Pode escutar até 10 portas por vez*/
	if(listen(socket_id, 10) < 0)
		exit(0);

	while(1)
	{
		int socketCliente;
		struct sockaddr_in clienteAddr;
		unsigned int clientetamanho = sizeof(clienteAddr);

		/* Iniciando uma conexão com o cliente */
		if((socketCliente = accept(socket_id, (struct sockaddr *) &clienteAddr, &clientetamanho)) < 0)
			printf("Erro na conexao com o cliente.\n");

		Mensagem_Cliente(socketCliente);

		close(socketCliente);
	}
	return 0;
}


void Mensagem_Cliente(int socketCliente)
{
	/*Criando string e recebendo mensagem*/
	int tamanho = 1000;
	char mensagem[tamanho];

	recv(socketCliente, mensagem, tamanho, 0); //ler a mensagem
	operacao(mensagem, socketCliente);
}

void operacao(char *mensagem, int socketCliente)
{
	char op[3];
	int resultado_inteiro=0, flag_erro=0, erro = 0, erro_sscanf = 0;
	int num1, num2;
	erro_sscanf = sscanf(mensagem, "%s %d %d", op, &num1, &num2);
	if(erro_sscanf == 3)
	{
		if(strcmp(op, "add") == 0)
			resultado_inteiro = num1 + num2;

		else if(strcmp(op, "sub") == 0)
			resultado_inteiro = num1 - num2;

		else if(strcmp(op, "div") == 0)
		{

			if(num2 == 0)
			{
				erro = 1;
				flag_erro = 1;
			}

			else
				resultado_inteiro = num1/num2;
		}

		else if(strcmp(op, "mul") == 0)
		resultado_inteiro = num1 * num2;


		else
		{
			flag_erro = 1;
			erro = 2;
		}
	}

	else
	{
		erro = 3;
		flag_erro = 1;
	}

	switch (flag_erro)
	{
		case 0:
				send(socketCliente, &flag_erro, sizeof(flag_erro), 0);
				send(socketCliente, &resultado_inteiro, sizeof(resultado_inteiro), 0);
		break;

		case 1:
			send(socketCliente, &flag_erro, sizeof(flag_erro), 0);
			if(erro == 1) /*Erro de divisão por zero*/
				send(socketCliente, &erro, sizeof(erro), 0);

			else if(erro ==2)/*Erro de operação incorreta*/
				send(socketCliente, &erro, sizeof(erro), 0);

			else if(erro ==3)/*Erro de mensagem incompleta*/
				send(socketCliente, &erro, sizeof(erro), 0);
		break;
	}
	printf("Mensagem recebida: %s\nMensagem enviada: %d\n\n", mensagem, resultado_inteiro);
}
