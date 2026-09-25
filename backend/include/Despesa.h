#ifndef _DESPESA_H_
#define _DESPESA_H_

#include <string>
#include "transacao.h"

using namespace std;

class Despesa : public transacao {
    private:
        string categoria;
    public:
        Despesa(int id, string descricao, double valor, string data, string categoria ) : transacao(id, descricao, valor, data), categoria(categoria){
        }
        double ContribuicaoSaldo() const override {
            return -valor;
        }
        string getTipo() const override{
            return "Despesa";
        }
        string getCategoria() const {
            return categoria;
}
};

#endif