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
        double margemLucroDesejada;
        double desconto;
    public:
        //constutor
        Orcamento(int id = 0, const Cliente& cliente = Cliente(), double custoMateriais = 0.0, double custoMaoDeObra = 0.0, double custoAdicionais = 0.0, double margemLucroDesejada = 0.0, double desconto = 0.0)
        : id(id), cliente(cliente), custoMateriais(custoMateriais), custoMaoDeObra(custoMaoDeObra), custoAdicionais(custoAdicionais), margemLucroDesejada(margemLucroDesejada), desconto(desconto) {}

        // get
        int getId() const { return id;}
        const Cliente& getCliente() const { return cliente; }
        double getCustoMateriais() const { return custoMateriais; }
        double getCustoMaoDeObra() const { return custoMaoDeObra; }
        double getCustoAdicionais() const { return custoAdicionais; }
        double getMargemLucroDesejada() const { return margemLucroDesejada; }
        double getDesconto() const { return desconto; }

        //set
        void setCustoMateriais(double novoCustoMateriais) { custoMateriais = novoCustoMateriais; }
        void setCustoMaoDeObra(double novoCustoMaoDeObra) { custoMaoDeObra = novoCustoMaoDeObra; }
        void setCustoAdicionais(double novoCustoAdicionais) { custoAdicionais = novoCustoAdicionais; }
        void setMargemLucroDesejada(double novaMargemLucroDesejada) { margemLucroDesejada = novaMargemLucroDesejada; }
        void setDesconto(double novoDesconto) { desconto = novoDesconto; }

        // cálculo de custo total 
        double calcularCustoTotal() const {
            return custoMateriais + custoMaoDeObra + custoAdicionais;
        }
        // cálculo de preço bruto (antes do desconto)
        double calcularPrecoBruto() const {
            return calcularCustoTotal() * (1.0 + margemLucroDesejada);
        }
        // cálculo preço final (após o desconto)
        double calcularPrecoFinal() const {
            return calcularPrecoBruto() - desconto;
        }
        // cálculo de margem percentual
        double calcularMargemPercentual() const {
            double precoFinal = calcularPrecoFinal();
            if (precoFinal == 0.0) return 0.0; //evita divisão por 0
            double lucroEfetivo = precoFinal - calcularCustoTotal();
            return (lucroEfetivo /precoFinal) * 100.0;
        }
};

#endif