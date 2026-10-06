#ifndef _RECEITA_H_
#define _RECEITA_H_

#include <string>
#include "Transacao.h"
using namespace std;

class Receita : public transacao{ // Herda de transação
    private:
        string origem; // No front com opções venda, encomenda, serviço..
    public:
        Receita(int id, string descricao, double valor, string data, string origem ) : transacao(id, descricao, valor, data), origem(origem){

        }

        string getTipo() const override{ //implementa o método virtual da classe mãe
            return "Receita";
        }
        string getOrigem() const {
            return origem;
        }
        void setOrigem(string novaOrigem){
            origem = novaOrigem;
        }
};

#endif