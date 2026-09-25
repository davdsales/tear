// David
#ifndef _PEDIDO_H_
#define _PEDIDO_H_

#include <string>
#include "Orcamento.h"

// enum para representar as colunas do kanban no frontend
enum class StatusPedido {
    EM_ABERTO = 0,    // rascunho / em análise
    APROVADO = 1,     // aguardando início da execução
    EM_PRODUCAO = 2,  // sendo executado/fabricado
    CONCLUIDO = 3,    // pronto e entregue
    CANCELADO = 4     // pedido recusado/cancelado
};

class Pedido {
private:
    int id;
    Orcamento orcamento;
    StatusPedido status;
    std::string dataCriacao;

public:
    // construtor
    Pedido(int id = 0, const Orcamento& orcamento = Orcamento(), 
           StatusPedido status = StatusPedido::EM_ABERTO, 
           const std::string& dataCriacao = "")
        : id(id), orcamento(orcamento), status(status), dataCriacao(dataCriacao) {}

    // getters
    int getId() const { return id; }
    const Orcamento& getOrcamento() const { return orcamento; }
    StatusPedido getStatus() const { return status; }
    std::string getDataCriacao() const { return dataCriacao; }

    // retorna o status em formato de texto legível para salvar ou exibir no kanban
    std::string getStatusTexto() const {
        switch (status) {
            case StatusPedido::EM_ABERTO:   return "Em Aberto";
            case StatusPedido::APROVADO:    return "Aprovado";
            case StatusPedido::EM_PRODUCAO: return "Em Produção";
            case StatusPedido::CONCLUIDO:   return "Concluído";
            case StatusPedido::CANCELADO:  return "Cancelado";
            default:                        return "Desconhecido";
        }
    }

    // setters
    void setStatus(StatusPedido novoStatus) { status = novoStatus; }
    void setDataCriacao(const std::string& novaData) { dataCriacao = novaData; }
    void setOrcamento(const Orcamento& novoOrcamento) { orcamento = novoOrcamento; }
};

#endif