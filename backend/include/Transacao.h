#ifndef _TRANSACAO_H_
#define _TRANSACAO_H_

#include <string>
using namespace std;

// A classe transacao representa qualquer movimetação finaceira 
// Ela é a classe mãe e possuí método virtual
class transacao{
    protected: // Foi usado protected para que as classes filhas pudessem utilizar os atributos privados 
        int id;
        string descricao;
        double valor;
        string data; 
    public:
        transacao(int id, string descricao, double valor, string data): id(id), descricao(descricao), valor(valor), data(data) {}
        virtual ~ transacao() {} // Destrutor virtual das transaçôes no construtor

        //Getters
        int getId() const{
            return id;
        }
        string getDescricao() const{
            return descricao;
        }
        double getValor() const{
            return valor;
        }
        string getData() const{
            return data;
        }
        
        void setDescricao(string novaDescricao){
            descricao = novaDescricao;
        }
        void setValor(double novoValor){
            valor = novoValor;
        }
        void setData(string novaData){
            data = novaData;
        }


        // Método que define se a transação é receita ou despesa
        virtual string getTipo() const = 0;
};

#endif