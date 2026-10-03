// Maria Gabriela
#ifndef _LISTA_COMPRAS_H_
#define _LISTA_COMPRAS_H_

#include <vector>
#include <string>
#include "Estoque.h"

// um item dentro de uma compra (quantidade e preco na unidade do estoque do material)
class ItemCompra {
private:
    int idMaterial;
    double quantidade;
    double precoUnitario;

public:
    ItemCompra(int idMaterial = 0, double quantidade = 0.0, double precoUnitario = 0.0)
        : idMaterial(idMaterial), quantidade(quantidade), precoUnitario(precoUnitario) {}

    int getIdMaterial() const { return idMaterial; }
    double getQuantidade() const { return quantidade; }
    double getPrecoUnitario() const { return precoUnitario; }
    double getSubtotal() const { return quantidade * precoUnitario; }
};

class Compra {
private:
    int id;
    std::string fornecedor;
    std::string data;
    std::vector<ItemCompra> itens;
    bool confirmada;

public:
    Compra(int id = 0, const std::string& fornecedor = "", const std::string& data = "",
           bool confirmada = false)
        : id(id), fornecedor(limparCampo(fornecedor)), data(limparCampo(data)),
          confirmada(confirmada) {}

    int getId() const { return id; }
    std::string getFornecedor() const { return fornecedor; }
    std::string getData() const { return data; }
    bool isConfirmada() const { return confirmada; }
    const std::vector<ItemCompra>& getItens() const { return itens; }

    void setFornecedor(const std::string& f) { fornecedor = limparCampo(f); }
    void setData(const std::string& d) { data = limparCampo(d); }
    void setConfirmada(bool c) { confirmada = c; }
    void adicionarItem(const ItemCompra& item) { itens.push_back(item); }

    double calcularTotal() const {
        double total = 0.0;
        for (const auto& item : itens) total += item.getSubtotal();
        return total;
    }
};

// gerencia as compras. Confirmar uma compra gera uma ENTRADA no estoque para cada item.
class ListaCompras {
private:
    std::vector<Compra> compras;
    int proximoId;
    std::string ultimoErro;

public:
    ListaCompras() : proximoId(1) {}

    std::string getUltimoErro() const { return ultimoErro; }

    int criarCompra(const std::string& fornecedor, const std::string& data) {
        Compra nova(proximoId++, fornecedor, data);
        compras.push_back(nova);
        return nova.getId();
    }

    Compra* buscarPorId(int id) {
        for (auto& c : compras) {
            if (c.getId() == id) return &c;
        }
        return nullptr;
    }

    const std::vector<Compra>& getCompras() const { return compras; }

    bool adicionarItem(int idCompra, int idMaterial, double quantidade, double precoUnitario,
                       const Estoque& estoque) {
        ultimoErro.clear();
        Compra* compra = buscarPorId(idCompra);
        if (compra == nullptr) {
            ultimoErro = "Compra #" + std::to_string(idCompra) + " não encontrada.";
            return false;
        }
        if (compra->isConfirmada()) {
            ultimoErro = "Compra já confirmada não pode ser alterada.";
            return false;
        }
        if (estoque.buscarPorId(idMaterial) == nullptr) {
            ultimoErro = "Material #" + std::to_string(idMaterial) + " não encontrado.";
            return false;
        }
        if (quantidade <= 0.0) {
            ultimoErro = "A quantidade precisa ser maior que zero.";
            return false;
        }
        if (precoUnitario < 0.0) {
            ultimoErro = "O preço não pode ser negativo.";
            return false;
        }
        compra->adicionarItem(ItemCompra(idMaterial, quantidade, precoUnitario));
        return true;
    }

    // compra confirmada e historico, entao so as pendentes podem ser apagadas
    bool removerCompra(int id) {
        ultimoErro.clear();
        for (auto it = compras.begin(); it != compras.end(); ++it) {
            if (it->getId() == id) {
                if (it->isConfirmada()) {
                    ultimoErro = "Compra já confirmada não pode ser excluída.";
                    return false;
                }
                compras.erase(it);
                return true;
            }
        }
        ultimoErro = "Compra #" + std::to_string(id) + " não encontrada.";
        return false;
    }

    // compra -> entrada no estoque. Passa o Estoque por referencia para nao copiar nada.
    // primeiro valida tudo, depois aplica, para nao deixar a compra pela metade.
    bool confirmarCompra(int id, Estoque& estoque) {
        ultimoErro.clear();
        Compra* compra = buscarPorId(id);
        if (compra == nullptr) {
            ultimoErro = "Compra #" + std::to_string(id) + " não encontrada.";
            return false;
        }
        if (compra->isConfirmada()) {
            ultimoErro = "Esta compra já foi confirmada.";
            return false;
        }
        if (compra->getItens().empty()) {
            ultimoErro = "Adicione pelo menos um item antes de confirmar a compra.";
            return false;
        }
        for (const auto& item : compra->getItens()) {
            if (estoque.buscarPorId(item.getIdMaterial()) == nullptr) {
                ultimoErro = "O material #" + std::to_string(item.getIdMaterial()) +
                             " desta compra não existe mais.";
                return false;
            }
        }

        std::string obs = "Compra #" + std::to_string(compra->getId()) +
                          (compra->getFornecedor().empty() ? "" : " - " + compra->getFornecedor());
        for (const auto& item : compra->getItens()) {
            estoque.registrarEntrada(item.getIdMaterial(), item.getQuantidade(),
                                     compra->getData(), obs);
            // o custo do material passa a ser o preco da compra mais recente
            estoque.buscarPorId(item.getIdMaterial())->setCustoUnitario(item.getPrecoUnitario());
        }
        compra->setConfirmada(true);
        return true;
    }

    // --- persistencia no banco SQLite (tabelas compras e itens_compra), separado por usuario ---

    static void criarTabelas(BancoDados& banco) {
        banco.executar(
            "CREATE TABLE IF NOT EXISTS compras ("
            " usuario_id INTEGER NOT NULL REFERENCES usuarios(id),"
            " id INTEGER NOT NULL,"
            " fornecedor TEXT,"
            " data TEXT NOT NULL,"
            " confirmada INTEGER NOT NULL DEFAULT 0,"
            " PRIMARY KEY (usuario_id, id))");
        banco.executar(
            "CREATE TABLE IF NOT EXISTS itens_compra ("
            " usuario_id INTEGER NOT NULL REFERENCES usuarios(id),"
            " id_compra INTEGER NOT NULL,"
            " id_material INTEGER NOT NULL,"
            " quantidade REAL NOT NULL,"
            " preco_unitario REAL NOT NULL)");
    }

    void salvarNoBanco(BancoDados& banco, int usuarioId) const {
        banco.executar("BEGIN");
        Consulta apagarItens(banco, "DELETE FROM itens_compra WHERE usuario_id = ?");
        apagarItens.ligar(1, usuarioId);
        apagarItens.executar();
        Consulta apagarCompras(banco, "DELETE FROM compras WHERE usuario_id = ?");
        apagarCompras.ligar(1, usuarioId);
        apagarCompras.executar();

        for (const auto& c : compras) {
            Consulta insert(banco, "INSERT INTO compras (usuario_id, id, fornecedor, data, confirmada) VALUES (?, ?, ?, ?, ?)");
            insert.ligar(1, usuarioId);
            insert.ligar(2, c.getId());
            insert.ligar(3, c.getFornecedor());
            insert.ligar(4, c.getData());
            insert.ligar(5, c.isConfirmada() ? 1 : 0);
            insert.executar();

            for (const auto& item : c.getItens()) {
                Consulta insertItem(banco,
                    "INSERT INTO itens_compra (usuario_id, id_compra, id_material, quantidade, preco_unitario) VALUES (?, ?, ?, ?, ?)");
                insertItem.ligar(1, usuarioId);
                insertItem.ligar(2, c.getId());
                insertItem.ligar(3, item.getIdMaterial());
                insertItem.ligar(4, item.getQuantidade());
                insertItem.ligar(5, item.getPrecoUnitario());
                insertItem.executar();
            }
        }
        banco.executar("COMMIT");
    }

    void carregarDoBanco(BancoDados& banco, int usuarioId) {
        compras.clear();
        int maxId = 0;

        Consulta cs(banco, "SELECT id, fornecedor, data, confirmada FROM compras WHERE usuario_id = ? ORDER BY id");
        cs.ligar(1, usuarioId);
        while (cs.proximaLinha()) {
            int id = cs.inteiro(0);
            compras.push_back(Compra(id, cs.texto(1), cs.texto(2), cs.inteiro(3) == 1));
            if (id > maxId) maxId = id;
        }

        Consulta itens(banco, "SELECT id_compra, id_material, quantidade, preco_unitario FROM itens_compra WHERE usuario_id = ?");
        itens.ligar(1, usuarioId);
        while (itens.proximaLinha()) {
            Compra* compra = buscarPorId(itens.inteiro(0));
            if (compra != nullptr) {
                compra->adicionarItem(ItemCompra(itens.inteiro(1), itens.numero(2), itens.numero(3)));
            }
        }

        proximoId = maxId + 1;
    }
};

#endif