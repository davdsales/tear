// Heloisa
#ifndef _CONTEXTO_USUARIO_H_
#define _CONTEXTO_USUARIO_H_

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "httplib.h"
#include "BancoDados.h"
#include "Estoque.h"
#include "ListaCompras.h"
#include "GerenciadorOrcamentos.h"
#include "GerenciamentoFinanceiro.h"
#include "Pedido.h"

// tudo o que pertence a uma conta: cada usuario comeca vazio e so ve o que ele mesmo cadastrou
class DadosUsuario {
public:
    int usuarioId;
    Estoque estoque;
    ListaCompras compras;
    GerenciadorOrcamentos orcamentos;
    std::vector<Pedido> pedidos;
    int proximoIdPedido = 1;
    GerenciamentoFinanceiro financeiro;

    DadosUsuario(BancoDados& banco, int id) : usuarioId(id) {
        estoque.carregarDoBanco(banco, id);
        compras.carregarDoBanco(banco, id);
        orcamentos.carregarDoBanco(banco, id);
        carregarPedidos(banco);
        financeiro.conectar(banco, id);
    }

    void salvarEstoque(BancoDados& banco) const {
        estoque.salvarNoBanco(banco, usuarioId);
        compras.salvarNoBanco(banco, usuarioId);
    }

    void salvarPedidos(BancoDados& banco) const {
        orcamentos.salvarNoBanco(banco, usuarioId);
        banco.executar("BEGIN");
        Consulta apagar(banco, "DELETE FROM pedidos WHERE usuario_id = ?");
        apagar.ligar(1, usuarioId);
        apagar.executar();
        for (const Pedido& p : pedidos) {
            Consulta insert(banco, "INSERT INTO pedidos (usuario_id, id, id_orcamento, status, data_criacao) VALUES (?, ?, ?, ?, ?)");
            insert.ligar(1, usuarioId);
            insert.ligar(2, p.getId());
            insert.ligar(3, p.getOrcamento().getId());
            insert.ligar(4, static_cast<int>(p.getStatus()));
            insert.ligar(5, p.getDataCriacao());
            insert.executar();
        }
        banco.executar("COMMIT");
    }

private:
    void carregarPedidos(BancoDados& banco) {
        Consulta consulta(banco, "SELECT id, id_orcamento, status, data_criacao FROM pedidos WHERE usuario_id = ? ORDER BY id");
        consulta.ligar(1, usuarioId);
        while (consulta.proximaLinha()) {
            Orcamento* orcamento = orcamentos.buscarPorId(consulta.inteiro(1));
            if (orcamento == nullptr) continue;
            int id = consulta.inteiro(0);
            pedidos.push_back(Pedido(id, *orcamento, static_cast<StatusPedido>(consulta.inteiro(2)), consulta.texto(3)));
            if (id >= proximoIdPedido) proximoIdPedido = id + 1;
        }
    }
};

// descobre quem fez a requisicao (cabecalho X-Usuario-Id que o front manda)
// e guarda em memoria os dados de quem ja entrou, para nao ler o banco toda hora
class SessaoUsuarios {
private:
    BancoDados& banco;
    std::map<int, std::unique_ptr<DadosUsuario>> usuarios;
    std::mutex trava;

    // tabelas da versao antiga (sem usuario_id) sao apagadas e recriadas no formato novo
    void apagarTabelaAntiga(const std::string& tabela) {
        bool antiga = false;
        {
            // a consulta precisa terminar antes do DROP, senao o SQLite diz que a tabela esta travada
            Consulta colunas(banco, "SELECT COUNT(*), SUM(name = 'usuario_id') FROM pragma_table_info(?)");
            colunas.ligar(1, tabela);
            antiga = colunas.proximaLinha() && colunas.inteiro(0) > 0 && colunas.inteiro(1) == 0;
        }
        if (antiga) banco.executar("DROP TABLE " + tabela);
    }

public:
    explicit SessaoUsuarios(BancoDados& b) : banco(b) {
        for (const char* tabela : {"materiais", "movimentacoes", "compras", "itens_compra", "orcamentos", "pedidos", "transacoes"}) {
            apagarTabelaAntiga(tabela);
        }

        banco.executar(
            "CREATE TABLE IF NOT EXISTS usuarios ("
            " id INTEGER PRIMARY KEY AUTOINCREMENT,"
            " nome TEXT NOT NULL,"
            " email TEXT NOT NULL UNIQUE,"
            " senha TEXT NOT NULL)");
        Estoque::criarTabelas(banco);
        ListaCompras::criarTabelas(banco);
        GerenciadorOrcamentos::criarTabela(banco);
        GerenciamentoFinanceiro::criarTabela(banco);
        banco.executar(
            "CREATE TABLE IF NOT EXISTS pedidos ("
            " usuario_id INTEGER NOT NULL REFERENCES usuarios(id),"
            " id INTEGER NOT NULL,"
            " id_orcamento INTEGER NOT NULL,"
            " status INTEGER NOT NULL,"               // 0 em aberto, 1 aprovado, 2 em producao, 3 concluido, 4 cancelado
            " data_criacao TEXT NOT NULL,"
            " PRIMARY KEY (usuario_id, id))");
    }

    BancoDados& getBanco() { return banco; }

    // devolve nullptr se ninguem estiver logado ou se o usuario nao existir no banco
    DadosUsuario* daRequisicao(const httplib::Request& req) {
        int id = 0;
        try { id = std::stoi(req.get_header_value("X-Usuario-Id")); } catch (...) { return nullptr; }

        std::lock_guard<std::mutex> bloqueio(trava);
        auto encontrado = usuarios.find(id);
        if (encontrado != usuarios.end()) return encontrado->second.get();

        Consulta existe(banco, "SELECT id FROM usuarios WHERE id = ?");
        existe.ligar(1, id);
        if (!existe.proximaLinha()) return nullptr;

        usuarios[id] = std::unique_ptr<DadosUsuario>(new DadosUsuario(banco, id));
        return usuarios[id].get();
    }
};

#endif