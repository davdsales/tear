// Heloisa
#ifndef _ROTAS_FINANCEIRO_H_
#define _ROTAS_FINANCEIRO_H_

#include <string>
#include <vector>
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "ContextoUsuario.h"
#include "RotasEstoque.h"  // reaproveita responder, lerNumero, lerTexto e dataDeHoje

namespace rotas_financeiro {

using json = nlohmann::json;
using rotas_estoque::responder;
using rotas_estoque::responderErro;
using rotas_estoque::lerNumero;
using rotas_estoque::lerTexto;
using rotas_estoque::dataDeHoje;
using rotas_estoque::semLogin;

const char* const ORIGENS[] = {"Encomenda", "Venda", "Feira", "Outros"};
const char* const MESES[] = {"Jan", "Fev", "Mar", "Abr", "Mai", "Jun", "Jul", "Ago", "Set", "Out", "Nov", "Dez"};

// "2026-10" voltando 3 meses -> "2026-07"
inline std::string mesAnterior(const std::string& mes, int meses) {
    int ano = std::stoi(mes.substr(0, 4));
    int m = std::stoi(mes.substr(5, 2)) - meses;
    while (m <= 0) { m += 12; ano--; }
    return std::to_string(ano) + (m < 10 ? "-0" : "-") + std::to_string(m);
}

inline json transacaoParaJson(const transacao* t) {
    json j = {
        {"id", t->getId()},
        {"tipo", t->getTipo()},
        {"descricao", t->getDescricao()},
        {"valor", t->getValor()},
        {"data", t->getData()}
    };
    if (const Receita* r = dynamic_cast<const Receita*>(t)) j["origem"] = r->getOrigem();
    else if (const Despesa* d = dynamic_cast<const Despesa*>(t)) j["categoria"] = d->getCategoria();
    return j;
}

}  // namespace rotas_financeiro

inline void registrarRotasFinanceiro(httplib::Server& svr, SessaoUsuarios& sessoes) {
    using namespace rotas_financeiro;

    svr.Get("/api/financeiro/transacoes", [&sessoes](const httplib::Request& req, httplib::Response& res) {
        DadosUsuario* d = sessoes.daRequisicao(req);
        if (!d) return semLogin(res);
        GerenciamentoFinanceiro& financeiro = d->financeiro;
        json lista = json::array();
        for (const transacao* t : financeiro.listarTransacoes()) lista.push_back(transacaoParaJson(t));
        responder(res, 200, lista);
    });

    // body: { tipo: "Receita" | "Despesa", descricao, valor, data, origem | categoria }
    svr.Post("/api/financeiro/transacoes", [&sessoes](const httplib::Request& req, httplib::Response& res) {
        DadosUsuario* d = sessoes.daRequisicao(req);
        if (!d) return semLogin(res);
        GerenciamentoFinanceiro& financeiro = d->financeiro;
        try {
            json body = json::parse(req.body);
            std::string tipo = lerTexto(body, "tipo");
            std::string descricao = lerTexto(body, "descricao");
            double valor = lerNumero(body, "valor");
            std::string data = lerTexto(body, "data", dataDeHoje());

            int id = 0;
            if (tipo == "Receita") id = financeiro.adicionarReceita(descricao, valor, data, lerTexto(body, "origem", "Outros"));
            else if (tipo == "Despesa") id = financeiro.adicionarDespesa(descricao, valor, data, lerTexto(body, "categoria", "Outros"));
            else return responderErro(res, 400, "Tipo inválido. Use Receita ou Despesa.");

            if (id == 0) return responderErro(res, 400, financeiro.getUltimoErro());
            responder(res, 201, {{"status", "sucesso"}, {"id", id}});
        } catch (const std::exception&) {
            responderErro(res, 400, "Dados inválidos no corpo da requisição.");
        }
    });

    // body: { descricao, valor, data, origem | categoria } (o tipo nao muda)
    svr.Put(R"(/api/financeiro/transacoes/(\d+))", [&sessoes](const httplib::Request& req, httplib::Response& res) {
        DadosUsuario* d = sessoes.daRequisicao(req);
        if (!d) return semLogin(res);
        GerenciamentoFinanceiro& financeiro = d->financeiro;
        try {
            int id = std::stoi(req.matches[1].str());
            json body = json::parse(req.body);
            bool ok = financeiro.editarTransacao(id, lerTexto(body, "descricao"), lerNumero(body, "valor"),
                                                 lerTexto(body, "data", dataDeHoje()),
                                                 lerTexto(body, "origem", "Outros"), lerTexto(body, "categoria", "Outros"));
            if (!ok) return responderErro(res, 400, financeiro.getUltimoErro());
            responder(res, 200, {{"status", "sucesso"}});
        } catch (const std::exception&) {
            responderErro(res, 400, "Dados inválidos no corpo da requisição.");
        }
    });

    svr.Delete(R"(/api/financeiro/transacoes/(\d+))", [&sessoes](const httplib::Request& req, httplib::Response& res) {
        DadosUsuario* d = sessoes.daRequisicao(req);
        if (!d) return semLogin(res);
        GerenciamentoFinanceiro& financeiro = d->financeiro;
        int id = std::stoi(req.matches[1].str());
        if (!financeiro.removerTransacao(id)) return responderErro(res, 404, financeiro.getUltimoErro());
        responder(res, 200, {{"status", "sucesso"}});
    });

    // os 4 cards da tela inicial
    svr.Get("/api/dashboard", [&sessoes](const httplib::Request& req, httplib::Response& res) {
        DadosUsuario* d = sessoes.daRequisicao(req);
        if (!d) return semLogin(res);
        GerenciamentoFinanceiro& financeiro = d->financeiro;
        double aReceber = 0.0;
        int emAndamento = 0;
        for (const Pedido& p : d->pedidos) {
            if (p.getStatus() == StatusPedido::CONCLUIDO || p.getStatus() == StatusPedido::CANCELADO) continue;
            aReceber += p.getOrcamento().calcularPrecoFinal();
            emAndamento++;
        }

        // materiais abaixo do minimo, para o aviso
        json baixos = json::array();
        for (const Material* m : d->estoque.getMateriais()) {
            if (!d->estoque.estaAbaixoDoMinimo(*m)) continue;
            baixos.push_back({
                {"nome", m->getNome()},
                {"unidade", m->getUnidade()},
                {"saldo", d->estoque.calcularSaldo(m->getId())},
                {"estoqueMinimo", m->getEstoqueMinimo()}
            });
        }

        responder(res, 200, {
            {"receitaMes", financeiro.receitaMes(dataDeHoje().substr(0, 7))},
            {"aReceber", aReceber},
            {"pedidosEmAndamento", emAndamento},
            {"estoqueBaixo", d->estoque.contarAbaixoDoMinimo()},
            {"itensAbaixoDoMinimo", baixos}
        });
    });

    // secao financeiro da tela inicial: cards do mes, grafico e origens
    svr.Get("/api/financeiro/resumo", [&sessoes](const httplib::Request& req, httplib::Response& res) {
        DadosUsuario* d = sessoes.daRequisicao(req);
        if (!d) return semLogin(res);
        GerenciamentoFinanceiro& financeiro = d->financeiro;
        std::string mes = dataDeHoje().substr(0, 7);

        json meses = json::array();
        for (int i = 5; i >= 0; i--) {
            std::string m = mesAnterior(mes, i);
            meses.push_back({
                {"rotulo", MESES[std::stoi(m.substr(5, 2)) - 1]},
                {"receitas", financeiro.receitaMes(m)},
                {"despesas", financeiro.despesaMes(m)}
            });
        }

        double total = financeiro.totalReceitas();
        json origens = json::array();
        for (const char* origem : ORIGENS) {
            double valor = financeiro.receitasPorOrigem(origem);
            origens.push_back({{"origem", origem}, {"percentual", total > 0 ? valor / total * 100 : 0}});
        }

        responder(res, 200, {
            {"receitas", financeiro.receitaMes(mes)},
            {"despesas", financeiro.despesaMes(mes)},
            {"lucro", financeiro.receitaMes(mes) - financeiro.despesaMes(mes)},
            {"ultimosMeses", meses},
            {"porOrigem", origens}
        });
    });
}

#endif