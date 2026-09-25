#ifndef _RECEITA_H_
#define _RECEITA_H_

#include <string>
#include "transacao.h"
using namespace std;

class Receita : public transacao{
    private:
        string origem;
    public:
        Receita(int id, string descricao, double valor, string data, string origem ) : transacao(id, descricao, valor, data), origem(origem){

        }
        double ContribuicaoSaldo() const override {
            return valor;
        }
        string getTipo() const override{
            return "Receita";
        }

};    

#endif