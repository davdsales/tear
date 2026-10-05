// David
#ifndef _ORCAMENTO_H_
#define _ORCAMENTO_H_

#include <string>
#include <algorithm> // necessario para std::max
#include "Cliente.h"

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
        // construtor usando os setters para garantir a validacao de dados logo na criação
        Orcamento(int id = 0, const Cliente& cliente = Cliente(), double custoMateriais = 0.0, 
                  double custoMaoDeObra = 0.0, double custoAdicionais = 0.0, 
                  double margemLucroDesejada = 0.0, double desconto = 0.0)
            : id(id), cliente(cliente) {
            setCustoMateriais(custoMateriais);
            setCustoMaoDeObra(custoMaoDeObra);
            setCustoAdicionais(custoAdicionais);
            setMargemLucroDesejada(margemLucroDesejada);
            setDesconto(desconto);
        }

        // getters
        int getId() const { return id; }
        const Cliente& getCliente() const { return cliente; }
        double getCustoMateriais() const { return custoMateriais; }
        double getCustoMaoDeObra() const { return custoMaoDeObra; }
        double getCustoAdicionais() const { return custoAdicionais; }
        double getMargemLucroDesejada() const { return margemLucroDesejada; }
        double getDesconto() const { return desconto; }

        // setters protegidos para não aceitarem valores abaixo de 0
        void setCustoMateriais(double valor) { custoMateriais = std::max(0.0, valor); }
        void setCustoMaoDeObra(double valor) { custoMaoDeObra = std::max(0.0, valor); }
        void setCustoAdicionais(double valor) { custoAdicionais = std::max(0.0, valor); }
        void setMargemLucroDesejada(double valor) { margemLucroDesejada = std::max(0.0, valor); }
        void setDesconto(double valor) { desconto = std::max(0.0, valor); }

        // cálculos
        double calcularCustoTotal() const {
            return custoMateriais + custoMaoDeObra + custoAdicionais;
        }

        double calcularPrecoBruto() const {
            return calcularCustoTotal() * (1.0 + margemLucroDesejada);
        }

        double calcularPrecoFinal() const {
            double precoFinal = calcularPrecoBruto() - desconto;
            // previne preco final negativo caso o desconto seja maior que o total
            return std::max(0.0, precoFinal);
        }

        double calcularMargemPercentual() const {
            double precoFinal = calcularPrecoFinal();
            if (precoFinal == 0.0) return 0.0; // evita divisão por zero
            
            double lucroEfetivo = precoFinal - calcularCustoTotal();
            return (lucroEfetivo / precoFinal) * 100.0;
        }
};

#endif