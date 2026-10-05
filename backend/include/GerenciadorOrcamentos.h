//David
#ifndef _GERENCIADOR_ORCAMENTOS_H_
#define _GERENCIADOR_ORCAMENTOS_H_

#include <vector>
#include <iostream>
#include <iomanip>
#include "Orcamento.h"
#include "BancoDados.h"

// classe que gerencia a lista de orcamentos e a tabela orcamentos do banco
class GerenciadorOrcamentos {
private:
    std::vector<Orcamento> orcamentos;
    int proximoId;

public:
    // construtor comecando os ids em 1
    GerenciadorOrcamentos() : proximoId(1) {}

    // adiciona um novo orcamento no vector
    void adicionarOrcamento(const Cliente& cliente, double custoMateriais, 
                            double custoMaoDeObra, double custoAdicionais, 
                            double margemLucro, double desconto) {
        Orcamento novoOrcamento(proximoId++, cliente, custoMateriais, 
                               custoMaoDeObra, custoAdicionais, 
                               margemLucro, desconto);
        orcamentos.push_back(novoOrcamento);
        std::cout << "\nOrçamento #" << novoOrcamento.getId() << " cadastrado com sucesso!\n";
    }

    // procura o orcamento pelo id
    Orcamento* buscarPorId(int id) {
        for (auto& orcamento : orcamentos) {
            if (orcamento.getId() == id) {
                return &orcamento;
            }
        }
        return nullptr;
    }

    // apaga o orcamento do vector
    bool removerOrcamento(int id) {
        for (auto it = orcamentos.begin(); it != orcamentos.end(); ++it) {
            if (it->getId() == id) {
                orcamentos.erase(it);
                return true;
            }
        }
        return false;
    }

    // imprime a lista de orcamentos no terminal
    void listarOrcamentos() const {
        if (orcamentos.empty()) {
            std::cout << "\nNenhum orçamento cadastrado.\n";
            return;
        }

        std::cout << "\n================ LISTA DE ORÇAMENTOS ================\n";
        std::cout << std::fixed << std::setprecision(2);
        for (const auto& o : orcamentos) {
            std::cout << "ID: " << o.getId() 
                      << " | Cliente: " << o.getCliente().getNome()
                      << " | Custo total: R$ " << o.calcularCustoTotal()
                      << " | Preco final: R$ " << o.calcularPrecoFinal()
                      << " | Margem: " << o.calcularMargemPercentual() << "%\n";
        }
        std::cout << "=====================================================\n";
    }

    // retorna a lista completa de orcamentos
    const std::vector<Orcamento>& getTodosOrcamentos() const {
        return orcamentos;
    }

    static void criarTabela(BancoDados& banco) {
        banco.executar(
            "CREATE TABLE IF NOT EXISTS orcamentos ("
            " usuario_id INTEGER NOT NULL REFERENCES usuarios(id),"
            " id INTEGER NOT NULL,"
            " cliente TEXT NOT NULL,"
            " contato TEXT,"
            " custo_materiais REAL NOT NULL,"
            " custo_mao_de_obra REAL NOT NULL,"
            " custo_adicionais REAL NOT NULL,"
            " margem_lucro REAL NOT NULL,"
            " desconto REAL NOT NULL,"
            " PRIMARY KEY (usuario_id, id))");
    }

    // grava os orcamentos do usuario na tabela (substitui o antigo salvarEmArquivo)
    void salvarNoBanco(BancoDados& banco, int usuarioId) const {
        banco.executar("BEGIN");
        Consulta apagar(banco, "DELETE FROM orcamentos WHERE usuario_id = ?");
        apagar.ligar(1, usuarioId);
        apagar.executar();
        for (const auto& o : orcamentos) {
            Consulta insert(banco,
                "INSERT INTO orcamentos (usuario_id, id, cliente, contato, custo_materiais, custo_mao_de_obra,"
                " custo_adicionais, margem_lucro, desconto) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
            insert.ligar(1, usuarioId);
            insert.ligar(2, o.getId());
            insert.ligar(3, o.getCliente().getNome());
            insert.ligar(4, o.getCliente().getContato());
            insert.ligar(5, o.getCustoMateriais());
            insert.ligar(6, o.getCustoMaoDeObra());
            insert.ligar(7, o.getCustoAdicionais());
            insert.ligar(8, o.getMargemLucroDesejada());
            insert.ligar(9, o.getDesconto());
            insert.executar();
        }
        banco.executar("COMMIT");
    }

    // le os orcamentos do usuario (substitui o antigo carregarDeArquivo)
    void carregarDoBanco(BancoDados& banco, int usuarioId) {
        orcamentos.clear();
        int maxId = 0;

        Consulta consulta(banco,
            "SELECT id, cliente, contato, custo_materiais, custo_mao_de_obra, custo_adicionais,"
            " margem_lucro, desconto FROM orcamentos WHERE usuario_id = ? ORDER BY id");
        consulta.ligar(1, usuarioId);
        while (consulta.proximaLinha()) {
            int id = consulta.inteiro(0);
            Cliente c(0, consulta.texto(1), consulta.texto(2));
            orcamentos.push_back(Orcamento(id, c, consulta.numero(3), consulta.numero(4),
                                           consulta.numero(5), consulta.numero(6), consulta.numero(7)));
            if (id > maxId) maxId = id;
        }
        proximoId = maxId + 1;
    }
};

#endif