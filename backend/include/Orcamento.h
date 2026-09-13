// David
#ifndef _ORCAMENTO_H_
#define _ORCAMENTO_H_
#include <string>
#include "Cliente.h"
using namespace std;

class Orcamento {
    private:
        int id;
        Cliente cliente;
        double custoMateriais;
        double custoMaoDeObra;
        double custoAdicionais;
        double desconto;
    public:
        //constutor
        Orcamento(int id = 0, Cliente cliente = Cliente(), double custoMateriais = 0.0, double custoMaoDeObra = 0.0, double custoAdicionais = 0.0, double desconto = 0.0)
        : id(id), cliente(cliente), custoMateriais(custoMateriais), custoMaoDeObra(custoMaoDeObra), custoAdicionais(custoAdicionais), desconto(desconto) {}

        // get
        int getId() const { return id;}
        Cliente getIdCliente() const { return cliente; }
        double getCustoMateriais() const { return custoMateriais; }
        double getCustoMaoDeObra() const { return custoMaoDeObra; }
        double getCustoAdicionais() const { return custoAdicionais; }
        double getDesconto() const { return desconto; }

        // cálculo
        double calcularCustoTotal() const {
            return custoMateriais + custoMaoDeObra + custoAdicionais;
        }
        double calcularPrecoFinal() const {
            return calcularCustoTotal() - desconto;
        }
        double calcularMargemPercentual() const {
            double preco = calcularPrecoFinal();
            if (preco == 0) return 0.0; //evita divisão por 0
            double lucro = preco - calcularCustoTotal();
            return (lucro /preco) * 100.0;
        }
};

#endif