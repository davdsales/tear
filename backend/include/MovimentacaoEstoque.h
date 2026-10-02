#ifndef _MOVIMENTACAO_ESTOQUE_H_
#define _MOVIMENTACAO_ESTOQUE_H_

#include <string>
#include "Material.h"

// tipos de movimentacao: o saldo do estoque e a soma de todas elas
enum class TipoMovimentacao {
    ENTRADA = 0,  // compra ou devolucao (quantidade positiva)
    CONSUMO = 1,  // material usado na producao (quantidade positiva, subtrai)
    AJUSTE = 2    // correcao manual (quantidade pode ser negativa)
};

class MovimentacaoEstoque {
private:
    int id;
    int idMaterial;
    TipoMovimentacao tipo;
    double quantidade;
    std::string data;
    std::string observacao;

public:
    MovimentacaoEstoque(int id = 0, int idMaterial = 0,
                        TipoMovimentacao tipo = TipoMovimentacao::ENTRADA,
                        double quantidade = 0.0, const std::string& data = "",
                        const std::string& observacao = "")
        : id(id), idMaterial(idMaterial), tipo(tipo), quantidade(quantidade),
          data(limparCampo(data)), observacao(limparCampo(observacao)) {}

    // getters
    int getId() const { return id; }
    int getIdMaterial() const { return idMaterial; }
    TipoMovimentacao getTipo() const { return tipo; }
    double getQuantidade() const { return quantidade; }
    std::string getData() const { return data; }
    std::string getObservacao() const { return observacao; }

    std::string getTipoTexto() const {
        switch (tipo) {
            case TipoMovimentacao::ENTRADA: return "Entrada";
            case TipoMovimentacao::CONSUMO: return "Consumo";
            case TipoMovimentacao::AJUSTE:  return "Ajuste";
            default:                        return "Desconhecido";
        }
    }

    // quanto essa movimentacao soma (ou subtrai) no saldo
    double getVariacao() const {
        if (tipo == TipoMovimentacao::CONSUMO) return -quantidade;
        return quantidade;
    }
};

#endif
