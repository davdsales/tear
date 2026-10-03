#include "httplib.h"
#include "nlohmann/json.hpp"
#include <iostream>

#include "Cliente.h"
#include "Orcamento.h"
#include "Pedido.h"
#include "ContextoUsuario.h"
#include "RotasEstoque.h"
#include "RotasFinanceiro.h"
#include "RotasUsuarios.h"

using json = nlohmann::json;
using rotas_estoque::responder;
using rotas_estoque::semLogin;

// banco SQLite cada conta tem os seus proprios materiais, compras, orcamentos, pedidos e transacoes
BancoDados banco("database/tear.db");
SessaoUsuarios sessoes(banco);

int main() {
    httplib::Server svr;

// Trata requisições 
    svr.Options(".*", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, X-Usuario-Id");
        res.status = 200;
    });

// Adiciona o cabeçalho de CORS globalmente (uma única vez por resposta)
    svr.set_post_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        if (!res.has_header("Access-Control-Allow-Origin")) {
            res.set_header("Access-Control-Allow-Origin", "*");
        }
    });

    registrarRotasEstoque(svr, sessoes);
    registrarRotasFinanceiro(svr, sessoes);
    registrarRotasUsuarios(svr, banco);

// rota get para listar os pedidos em formato json para o kanban
    svr.Get("/api/pedidos", [](const httplib::Request& req, httplib::Response& res) {
        DadosUsuario* d = sessoes.daRequisicao(req);
        if (!d) return semLogin(res);

        json listaJson = json::array();
        for (const auto& pedido : d->pedidos) {
            const auto& orcamento = pedido.getOrcamento();
            const auto& cliente = orcamento.getCliente();

            listaJson.push_back({
                {"id", pedido.getId()},
                {"cliente", cliente.getNome()},
                {"contato", cliente.getContato()},
                {"descricao", "Orçamento #" + std::to_string(orcamento.getId())},
                {"valor", orcamento.calcularPrecoFinal()},
                {"status", pedido.getStatusTexto()},
                {"dataCriacao", pedido.getDataCriacao()}
            });
        }
        responder(res, 200, listaJson);
    });

// rota post para receber os dados do react e criar novo orcamento e pedido
    svr.Post("/api/pedidos", [](const httplib::Request& req, httplib::Response& res) {
        DadosUsuario* d = sessoes.daRequisicao(req);
        if (!d) return semLogin(res);

        try {
            auto body = json::parse(req.body);

            std::string nomeCliente = body.value("cliente", "Cliente Anônimo");
            std::string contato = body.value("contato", "");
            double mat = body.value("materiais", 0.0);
            double mao = body.value("maoDeObra", 0.0);
            double adic = body.value("adicionais", 0.0);
            double margem = body.value("margem", 0.20);
            double desc = body.value("desconto", 0.0);

// cria o cliente e o orcamento
            Cliente novoCliente(0, nomeCliente, contato);
            d->orcamentos.adicionarOrcamento(novoCliente, mat, mao, adic, margem, desc);

// pega o ultimo orcamento criado para montar o pedido
            const auto& ultimoOrcamento = d->orcamentos.getTodosOrcamentos().back();
            Pedido novoPedido(d->proximoIdPedido++, ultimoOrcamento, StatusPedido::EM_ABERTO, rotas_estoque::dataDeHoje());
            d->pedidos.push_back(novoPedido);
            d->salvarPedidos(banco);

            std::cout << "\n[C++] Novo Pedido #" << novoPedido.getId() << " cadastrado com sucesso via React!\n";
            responder(res, 201, {{"status", "sucesso"}});
        } catch (const std::exception& e) {
            responder(res, 400, {{"status", "erro"}});
        }
    });

// rota put para atualizar o status do pedido ao mover no kanban
    svr.Put(R"(/api/pedidos/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        DadosUsuario* d = sessoes.daRequisicao(req);
        if (!d) return semLogin(res);

        try {
            int id = std::stoi(req.matches[1]);
            auto body = json::parse(req.body);
            std::string novoStatus = body.value("status", "Em Aberto");

            for (auto& pedido : d->pedidos) {
                if (pedido.getId() == id) {
                    if (novoStatus == "Em Produção") {
                        pedido.setStatus(StatusPedido::EM_PRODUCAO);
                    } else if (novoStatus == "Concluído") {
                        pedido.setStatus(StatusPedido::CONCLUIDO);
                    } else {
                        pedido.setStatus(StatusPedido::EM_ABERTO);
                    }
                    break;
                }
            }
            d->salvarPedidos(banco);

            std::cout << "\n[C++] Status do Pedido #" << id << " atualizado para: " << novoStatus << "\n";
            responder(res, 200, {{"status", "sucesso"}});
        } catch (const std::exception& e) {
            responder(res, 400, {{"status", "erro"}});
        }
    });

    std::cout << "Servidor Backend em C++ rodando em http://localhost:8080" << std::endl;
    if (!svr.listen("0.0.0.0", 8080)) {
        std::cout << "Nao consegui abrir a porta 8080. Feche o outro sistema.exe e tente de novo." << std::endl;
    }

    return 0;
}