// Maria Gabriela
#ifndef _ROTAS_ESTOQUE_H_
#define _ROTAS_ESTOQUE_H_

#include <string>
#include <memory>
#include <ctime>
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "Estoque.h"
#include "ListaCompras.h"

// rotas de estoque e compras. O main.cpp so precisa chamar registrarRotasEstoque(...)
namespace rotas_estoque {

using json = nlohmann::json;

inline const std::string ARQUIVO_MATERIAIS = "materiais.txt";
inline const std::string ARQUIVO_MOVIMENTACOES = "movimentacoes.txt";
inline const std::string ARQUIVO_COMPRAS = "compras.txt";

inline void cors(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

inline void responder(httplib::Response& res, int status, const json& corpo) {
    cors(res);
    res.status = status;
    res.set_content(corpo.dump(-1, ' ', false, json::error_handler_t::replace), "application/json");
}

inline void responderErro(httplib::Response& res, int status, const std::string& mensagem) {
    responder(res, status, {{"status", "erro"}, {"mensagem", mensagem}});
}

inline std::string dataDeHoje() {
    std::time_t agora = std::time(nullptr);
    std::tm t;
#ifdef _WIN32
    localtime_s(&t, &agora);
#else
    localtime_r(&agora, &t);
#endif
    char buffer[16];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &t);
    return buffer;
}

// os inputs do React costumam chegar como texto ("15"), entao aceita numero ou texto numerico
inline double lerNumero(const json& b, const std::string& chave, double padrao = 0.0) {
    if (!b.contains(chave) || b[chave].is_null()) return padrao;
    const json& v = b[chave];
    if (v.is_number()) return v.get<double>();
    if (v.is_string()) {
        try { return std::stod(v.get<std::string>()); } catch (...) { return padrao; }
    }
    return padrao;
}

inline std::string lerTexto(const json& b, const std::string& chave, const std::string& padrao = "") {
    if (b.contains(chave) && b[chave].is_string()) return b[chave].get<std::string>();
    return padrao;
}

inline void salvarTudo(const Estoque& estoque, const ListaCompras& listaCompras) {
    estoque.salvarEmArquivo(ARQUIVO_MATERIAIS, ARQUIVO_MOVIMENTACOES);
    listaCompras.salvarEmArquivo(ARQUIVO_COMPRAS);
}

// --- conversao para json ---

inline json materialParaJson(const Estoque& estoque, const Material& m) {
    json j = {
        {"id", m.getId()},
        {"tipo", m.getTipo()},
        {"nome", m.getNome()},
        {"descricao", m.descricao()},  // cada tipo de material descreve a si mesmo (polimorfismo)
        {"unidade", m.getUnidade()},
        {"custoUnitario", m.getCustoUnitario()},
        {"estoqueMinimo", m.getEstoqueMinimo()},
        {"saldo", estoque.calcularSaldo(m.getId())},
        {"abaixoDoMinimo", estoque.estaAbaixoDoMinimo(m)}
    };
    if (const Fio* f = dynamic_cast<const Fio*>(&m)) {
        j["marca"] = f->getMarca();
        j["cor"] = f->getCor();
        j["metragem"] = f->getMetragem();
    } else if (const Tecido* t = dynamic_cast<const Tecido*>(&m)) {
        j["composicao"] = t->getComposicao();
        j["largura"] = t->getLargura();
    } else if (const Aviamento* a = dynamic_cast<const Aviamento*>(&m)) {
        j["detalhe"] = a->getDetalhe();
    }
    return j;
}

inline json movimentacaoParaJson(const Estoque& estoque, const MovimentacaoEstoque& mov) {
    const Material* m = estoque.buscarPorId(mov.getIdMaterial());
    return {
        {"id", mov.getId()},
        {"idMaterial", mov.getIdMaterial()},
        {"material", m ? m->getNome() : "(removido)"},
        {"tipo", mov.getTipoTexto()},
        {"quantidade", mov.getQuantidade()},
        {"data", mov.getData()},
        {"observacao", mov.getObservacao()}
    };
}

inline json compraParaJson(const Estoque& estoque, const Compra& c) {
    json itens = json::array();
    for (const auto& item : c.getItens()) {
        const Material* m = estoque.buscarPorId(item.getIdMaterial());
        itens.push_back({
            {"idMaterial", item.getIdMaterial()},
            {"material", m ? m->getNome() : "(removido)"},
            {"quantidade", item.getQuantidade()},
            {"precoUnitario", item.getPrecoUnitario()},
            {"subtotal", item.getSubtotal()}
        });
    }
    return {
        {"id", c.getId()},
        {"fornecedor", c.getFornecedor()},
        {"data", c.getData()},
        {"confirmada", c.isConfirmada()},
        {"total", c.calcularTotal()},
        {"itens", itens}
    };
}

// --- criar e editar materiais ---

// devolve nullptr se o tipo for desconhecido
inline Material* criarMaterial(const json& b) {
    std::string tipo = lerTexto(b, "tipo");
    std::string nome = lerTexto(b, "nome");
    double custo = lerNumero(b, "custoUnitario");
    double minimo = lerNumero(b, "estoqueMinimo");

    if (tipo == "FIO") {
        return new Fio(0, nome, lerTexto(b, "unidade", "g"), custo, minimo,
                       lerTexto(b, "marca"), lerTexto(b, "cor"), lerNumero(b, "metragem"));
    }
    if (tipo == "TECIDO") {
        return new Tecido(0, nome, lerTexto(b, "unidade", "m"), custo, minimo,
                          lerTexto(b, "composicao"), lerNumero(b, "largura"));
    }
    if (tipo == "AVIAMENTO") {
        return new Aviamento(0, nome, lerTexto(b, "unidade", "un"), custo, minimo,
                             lerTexto(b, "detalhe"));
    }
    return nullptr;
}

// so muda os campos que vieram no json
inline void aplicarEdicao(Material& m, const json& b) {
    if (b.contains("nome")) m.setNome(lerTexto(b, "nome"));
    if (b.contains("unidade")) m.setUnidade(lerTexto(b, "unidade"));
    if (b.contains("custoUnitario")) m.setCustoUnitario(lerNumero(b, "custoUnitario"));
    if (b.contains("estoqueMinimo")) m.setEstoqueMinimo(lerNumero(b, "estoqueMinimo"));

    if (Fio* f = dynamic_cast<Fio*>(&m)) {
        if (b.contains("marca")) f->setMarca(lerTexto(b, "marca"));
        if (b.contains("cor")) f->setCor(lerTexto(b, "cor"));
        if (b.contains("metragem")) f->setMetragem(lerNumero(b, "metragem"));
    } else if (Tecido* t = dynamic_cast<Tecido*>(&m)) {
        if (b.contains("composicao")) t->setComposicao(lerTexto(b, "composicao"));
        if (b.contains("largura")) t->setLargura(lerNumero(b, "largura"));
    } else if (Aviamento* a = dynamic_cast<Aviamento*>(&m)) {
        if (b.contains("detalhe")) a->setDetalhe(lerTexto(b, "detalhe"));
    }
}

}  // namespace rotas_estoque

// carrega os arquivos e registra todas as rotas de estoque e compras
inline void registrarRotasEstoque(httplib::Server& svr, Estoque& estoque, ListaCompras& listaCompras) {
    using namespace rotas_estoque;

    estoque.carregarDeArquivo(ARQUIVO_MATERIAIS, ARQUIVO_MOVIMENTACOES);
    listaCompras.carregarDeArquivo(ARQUIVO_COMPRAS);

    // ---------- materiais ----------

    svr.Get("/api/materiais", [&estoque](const httplib::Request&, httplib::Response& res) {
        json lista = json::array();
        for (const Material* m : estoque.getMateriais()) lista.push_back(materialParaJson(estoque, *m));
        responder(res, 200, lista);
    });

    svr.Post("/api/materiais", [&estoque, &listaCompras](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);
            std::unique_ptr<Material> novo(criarMaterial(body));
            if (!novo) return responderErro(res, 400, "Tipo de material inválido. Use FIO, TECIDO ou AVIAMENTO.");
            if (novo->getNome().empty()) return responderErro(res, 400, "Informe o nome do material.");

            int id = estoque.adicionarMaterial(novo.release());

            // estoque inicial opcional: vira uma movimentacao de entrada, como qualquer outra
            double inicial = lerNumero(body, "estoqueInicial");
            if (inicial > 0.0 && !estoque.registrarEntrada(id, inicial, lerTexto(body, "data", dataDeHoje()), "Estoque inicial")) {
                return responderErro(res, 400, estoque.getUltimoErro());
            }

            salvarTudo(estoque, listaCompras);
            responder(res, 201, {{"status", "sucesso"}, {"id", id}});
        } catch (const std::exception&) {
            responderErro(res, 400, "Dados inválidos no corpo da requisição.");
        }
    });

    svr.Put(R"(/api/materiais/(\d+))", [&estoque, &listaCompras](const httplib::Request& req, httplib::Response& res) {
        try {
            Material* m = estoque.buscarPorId(std::stoi(req.matches[1].str()));
            if (m == nullptr) return responderErro(res, 404, "Material não encontrado.");
            aplicarEdicao(*m, json::parse(req.body));
            salvarTudo(estoque, listaCompras);
            responder(res, 200, {{"status", "sucesso"}});
        } catch (const std::exception&) {
            responderErro(res, 400, "Dados inválidos no corpo da requisição.");
        }
    });

    svr.Delete(R"(/api/materiais/(\d+))", [&estoque, &listaCompras](const httplib::Request& req, httplib::Response& res) {
        int id = std::stoi(req.matches[1].str());
        if (estoque.buscarPorId(id) == nullptr) return responderErro(res, 404, "Material não encontrado.");
        if (!estoque.removerMaterial(id)) return responderErro(res, 409, estoque.getUltimoErro());
        salvarTudo(estoque, listaCompras);
        responder(res, 200, {{"status", "sucesso"}});
    });

    // ---------- estoque ----------

    // resumo para a tela inicial (card "Itens com estoque baixo")
    svr.Get("/api/estoque", [&estoque](const httplib::Request&, httplib::Response& res) {
        json baixos = json::array();
        for (const Material* m : estoque.getMateriais()) {
            if (estoque.estaAbaixoDoMinimo(*m)) baixos.push_back(materialParaJson(estoque, *m));
        }
        responder(res, 200, {
            {"totalMateriais", estoque.getMateriais().size()},
            {"abaixoDoMinimo", estoque.contarAbaixoDoMinimo()},
            {"itensAbaixoDoMinimo", baixos}
        });
    });

    svr.Get("/api/estoque/movimentacoes", [&estoque](const httplib::Request& req, httplib::Response& res) {
        // filtro opcional: /api/estoque/movimentacoes?idMaterial=3
        int filtro = 0;
        if (req.has_param("idMaterial")) {
            try { filtro = std::stoi(req.get_param_value("idMaterial")); } catch (...) { filtro = 0; }
        }
        json lista = json::array();
        for (const auto& mov : estoque.getMovimentacoes()) {
            if (filtro == 0 || mov.getIdMaterial() == filtro) lista.push_back(movimentacaoParaJson(estoque, mov));
        }
        responder(res, 200, lista);
    });

    svr.Post("/api/estoque/movimentacoes", [&estoque, &listaCompras](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);
            int idMaterial = static_cast<int>(lerNumero(body, "idMaterial"));
            double quantidade = lerNumero(body, "quantidade");
            std::string tipo = lerTexto(body, "tipo");
            std::string data = lerTexto(body, "data", dataDeHoje());
            std::string obs = lerTexto(body, "observacao");

            bool ok = false;
            if (tipo == "ENTRADA") ok = estoque.registrarEntrada(idMaterial, quantidade, data, obs);
            else if (tipo == "CONSUMO") ok = estoque.registrarConsumo(idMaterial, quantidade, data, obs);
            else if (tipo == "AJUSTE") ok = estoque.registrarAjuste(idMaterial, quantidade, data, obs);
            else return responderErro(res, 400, "Tipo de movimentação inválido. Use ENTRADA, CONSUMO ou AJUSTE.");

            if (!ok) return responderErro(res, 400, estoque.getUltimoErro());
            salvarTudo(estoque, listaCompras);
            responder(res, 201, {{"status", "sucesso"}, {"saldo", estoque.calcularSaldo(idMaterial)}});
        } catch (const std::exception&) {
            responderErro(res, 400, "Dados inválidos no corpo da requisição.");
        }
    });

    // ---------- compras ----------

    svr.Get("/api/compras", [&estoque, &listaCompras](const httplib::Request&, httplib::Response& res) {
        json lista = json::array();
        for (const auto& c : listaCompras.getCompras()) lista.push_back(compraParaJson(estoque, c));
        responder(res, 200, lista);
    });

    // cria a compra com os itens; se "confirmar" for true, ja da entrada no estoque
    svr.Post("/api/compras", [&estoque, &listaCompras](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);
            if (!body.contains("itens") || !body["itens"].is_array() || body["itens"].empty()) {
                return responderErro(res, 400, "Adicione pelo menos um item à compra.");
            }

            int id = listaCompras.criarCompra(lerTexto(body, "fornecedor"), lerTexto(body, "data", dataDeHoje()));
            for (const json& item : body["itens"]) {
                bool ok = listaCompras.adicionarItem(id, static_cast<int>(lerNumero(item, "idMaterial")),
                                                     lerNumero(item, "quantidade"),
                                                     lerNumero(item, "precoUnitario"), estoque);
                if (!ok) {
                    std::string erro = listaCompras.getUltimoErro();
                    listaCompras.removerCompra(id);  // desfaz, nao deixa compra pela metade
                    return responderErro(res, 400, erro);
                }
            }

            if (body.value("confirmar", false) && !listaCompras.confirmarCompra(id, estoque)) {
                std::string erro = listaCompras.getUltimoErro();
                listaCompras.removerCompra(id);
                return responderErro(res, 400, erro);
            }

            salvarTudo(estoque, listaCompras);
            responder(res, 201, {{"status", "sucesso"}, {"id", id}});
        } catch (const std::exception&) {
            responderErro(res, 400, "Dados inválidos no corpo da requisição.");
        }
    });

    svr.Post(R"(/api/compras/(\d+)/confirmar)", [&estoque, &listaCompras](const httplib::Request& req, httplib::Response& res) {
        int id = std::stoi(req.matches[1].str());
        if (listaCompras.buscarPorId(id) == nullptr) return responderErro(res, 404, "Compra não encontrada.");
        if (!listaCompras.confirmarCompra(id, estoque)) return responderErro(res, 400, listaCompras.getUltimoErro());
        salvarTudo(estoque, listaCompras);
        responder(res, 200, {{"status", "sucesso"}});
    });

    svr.Delete(R"(/api/compras/(\d+))", [&estoque, &listaCompras](const httplib::Request& req, httplib::Response& res) {
        int id = std::stoi(req.matches[1].str());
        if (listaCompras.buscarPorId(id) == nullptr) return responderErro(res, 404, "Compra não encontrada.");
        if (!listaCompras.removerCompra(id)) return responderErro(res, 409, listaCompras.getUltimoErro());
        salvarTudo(estoque, listaCompras);
        responder(res, 200, {{"status", "sucesso"}});
    });
}

#endif
