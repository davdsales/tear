#ifndef _TRANSACAO_H_
#define _TRANSACAO_H_

#include <string>
#include <chrono>
using namespace std;


class transacao{
    protected:
        int id;
        string descricao;
        double valor;
        string data; // Data no tipo chrono
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

        // Método virtual saldo
        virtual double ContribuicaoSaldo() const = 0;
        virtual string getTipo() const = 0;
};

#endif