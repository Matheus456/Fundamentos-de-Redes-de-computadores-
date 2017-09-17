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
	int tamanho;
	unsigned int servidorPorta;
	char *IP_Servidor;

	/* Passando valores de IP do cliente e Porta */
	scanf("%s %d",IP_Servidor,&servidorPorta);
	printf("Abrindo o soquete cliente\n");

	while(1)
	{
		char mensagem[1000];
		printf("Insira a mensagem a ser enviada\n");
		scanf(" %[^\n]", mensagem);

		if(strcmp(mensagem, "^C"))
		{
			/* Passando descritor do socket para socket_id */
			socket_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			/* Em caso de valor negativo, o inicio nao foi correto */
			if(socket_id < 0)
			{
				printf("Erro na criacao do socket!\n");
				exit(0);
			}

			/* Zerando a estrutura de dados */
			memset(&servidorAddr, 0, sizeof(servidorAddr));
			servidorAddr.sin_family = AF_INET;
			servidorAddr.sin_addr.s_addr = inet_addr(IP_Servidor);
			servidorAddr.sin_port = htons(servidorPorta);

			/* Iniciando conexao do socket(socket_id) ao endereço presente em servidorAddr*/
			/* Em caso de falha, o programa é encerrado */
			if(connect(socket_id, (struct sockaddr *) &servidorAddr, sizeof(servidorAddr)) < 0)
			{
				printf("Falha !\n");
				exit(0);
			}

			tamanho = strlen(mensagem) + 1;

			/* Enviando a mensagem para o cliente*/
			send(socket_id, mensagem, tamanho, 0);

			int resultado, erro;

			/* Recebendo o tamanho da mensagem enviada pelo servidor */
			recv(socket_id, &erro, sizeof(erro), 0);
			/* Recebendo a resposta do servidor em relacao a operacao enviada */
			recv(socket_id, &resultado, sizeof(resultado), 0);

			if(erro == 1)
				printf("Erro!!\nDivisao por zero!\n");
			else if(erro == 2)
				printf("Erro!!\nOperacao incorreta!\n");
			else if(erro ==3)
				printf("Erro!!\nMensagem incompleta!\n");
			else
				printf("Resultado = %d\n",resultado);
		}
		close(socket_id);
	}
	return 0;
}
