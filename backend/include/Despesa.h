#ifndef _DESPESA_H_
#define _DESPESA_H_

#include <string>
#include "Transacao.h"

using namespace std;

class Despesa : public transacao { //Herda da classe mãe transação
    private:
        string categoria; //No front se refere aos materiais
    public:
        Despesa(int id, string descricao, double valor, string data, string categoria ) : transacao(id, descricao, valor, data), categoria(categoria){
        }
        string getTipo() const override{
            return "Despesa";
        }
        string getCategoria() const {
            return categoria;
        }
        void setCategoria(string novaCategoria){
            categoria = novaCategoria;
        }
};

#endif