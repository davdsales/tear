#include "httplib.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include <vector>

#include "Cliente.h"
#include "Orcamento.h"
#include "GerenciadorOrcamentos.h"
#include "Pedido.h"

using json = nlohmann::json;

// instancias globais para gerenciar os orcamentos e a lista de pedidos
GerenciadorOrcamentos gerenciadorOrcamentos;
std::vector<Pedido> listaPedidos;
int proximoIdPedido = 1;

// funcao que carrega os dados do arquivo txt ou cria os orcamentos padrao
void carregarDadosIniciais() {
    gerenciadorOrcamentos.carregarDeArquivo("orcamentos.txt");

    if (gerenciadorOrcamentos.getTodosOrcamentos().empty()) {
        Cliente c1(1, "Maria Clara", "81999998888");
        Cliente c2(2, "João Pedro", "81988887777");

        gerenciadorOrcamentos.adicionarOrcamento(c1, 50.0, 40.0, 10.0, 0.20, 0.0);
        gerenciadorOrcamentos.adicionarOrcamento(c2, 100.0, 80.0, 20.0, 0.25, 0.0);

        gerenciadorOrcamentos.salvarEmArquivo("orcamentos.txt");
    }

    const auto& orcamentos = gerenciadorOrcamentos.getTodosOrcamentos();
    if (!orcamentos.empty() && listaPedidos.empty()) {
        listaPedidos.push_back(Pedido(proximoIdPedido++, orcamentos[0], StatusPedido::EM_ABERTO, "2026-09-26"));
        if (orcamentos.size() > 1) {
            listaPedidos.push_back(Pedido(proximoIdPedido++, orcamentos[1], StatusPedido::EM_PRODUCAO, "2026-09-26"));
        }
    }
}

int main() {
    carregarDadosIniciais();

    httplib::Server svr;

    // rota get para listar os pedidos em formato json para o kanban
    svr.Get("/api/pedidos", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");

        json listaJson = json::array();

        for (const auto& pedido : listaPedidos) {
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

        res.set_content(listaJson.dump(), "application/json");
    });

    // rota post para receber os dados do react e criar novo orcamento e pedido
    svr.Post("/api/pedidos", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");

        try {
            auto body = json::parse(req.body);

            std::string nomeCliente = body.value("cliente", "Cliente Anônimo");
            std::string contato = body.value("contato", "");
            double mat = body.value("materiais", 0.0);
            double mao = body.value("maoDeObra", 0.0);
            double adic = body.value("adicionais", 0.0);
            double margem = body.value("margem", 0.20);
            double desc = body.value("desconto", 0.0);

            // cria o cliente e salva o orcamento
            Cliente novoCliente(0, nomeCliente, contato);
            gerenciadorOrcamentos.adicionarOrcamento(novoCliente, mat, mao, adic, margem, desc);
            gerenciadorOrcamentos.salvarEmArquivo("orcamentos.txt");

            // pega o ultimo orcamento criado para montar o pedido
            const auto& todos = gerenciadorOrcamentos.getTodosOrcamentos();
            const auto& ultimoOrcamento = todos.back();

            Pedido novoPedido(proximoIdPedido++, ultimoOrcamento, StatusPedido::EM_ABERTO, "2026-09-26");
            listaPedidos.push_back(novoPedido);

            std::cout << "\n[C++] Novo Pedido #" << novoPedido.getId() << " cadastrado com sucesso via React!\n";

            res.set_content(R"({"status": "sucesso"})", "application/json");
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(R"({"status": "erro"})", "application/json");
        }
    });

    // rota options para liberar requisicoes cors do navegador
    svr.Options(R"(/api/.*)", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.status = 200;
    });

    std::cout << "Servidor Backend em C++ rodando em http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}