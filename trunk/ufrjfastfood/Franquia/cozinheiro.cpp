#include "cozinheiro.h"

void *cozinheiro (void *ptr)
{
    int id = (int) ptr;
    msg_t msg;
	unsigned int nome_cliente;
    char temp[256];

	while (1)
	{
        msg.mtype = 1;

        conversa[id + num_caixas_pedido + num_caixas_pagamento].clear();

        //Recebe um pedido
        sprintf (temp, "Cozinheiro (%d) livre.\n", id + 1);
        conversa[id + num_caixas_pedido + num_caixas_pagamento].push_back(temp);
        if (msgrcv(fila_pedidos, &msg, MAX_MSG_SIZE, 1, 0) < 0) //Recebe id do cliente, para buscar pedido no map.
        {
            puts("Nao ouco o que o caixa de pagamento me diz! Eu me demito!\n");
            pthread_exit(NULL);
        }
        sscanf(msg.mtext, "%u", &nome_cliente); //Copia id do cliente recebido.
			
		pthread_mutex_lock(&relatorio_dia);
			at_cozinheiro[id]++;
		pthread_mutex_unlock(&relatorio_dia);

        //Cozinheiro usa o map pedido, com o id do cliente
        //Calcula todos os ingrediente a partir de pedido[nome_cliente], e retira dos ingredientes separados.
        separados.wait();
            separados.retira_ingredientes(pedido[nome_cliente]);
        separados.post();

        sprintf(temp, "Cozinheiro (%d) conferiu e retirou os ingredientes separados, relacionado ao pedido de %u.\n\n", id
        + 1, nome_cliente);
        conversa[id + num_caixas_pedido + num_caixas_pagamento].push_back(temp);

        //Cozinheiro passou o sanduiche ao entregador
        if (msgsnd(fila_produtos, &msg, MAX_MSG_SIZE, 0) < 0)
        {
            puts("Esses entregadores não recebem meu produto! Eu me demito!\n");
            pthread_exit(NULL);
        }
    
        //Imprime os dados sobre a ação do cozinheiro
        pthread_mutex_lock(&mutex_IO);
		for (int i = 0; i < (int) conversa[id + num_caixas_pedido + num_caixas_pagamento].size(); i++)
		{
			printf ("%s", conversa[id + num_caixas_pedido + num_caixas_pagamento][i].c_str());
		}
		printf ("\n");
		pthread_mutex_unlock(&mutex_IO);
    }

	return NULL;
}
