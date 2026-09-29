// Maria Gabriela
#ifndef _LISTA_COMPRAS_H_
#define _LISTA_COMPRAS_H_

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
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

    // --- persistencia ---
    // linha de compra:  C;id;fornecedor;data;confirmada
    // linha de item:    I;idCompra;idMaterial;quantidade;preco

    void salvarEmArquivo(const std::string& nomeArquivo) const {
        std::ofstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) {
            std::cerr << "Erro ao abrir " << nomeArquivo << " para escrita!\n";
            return;
        }
        for (const auto& c : compras) {
            arquivo << "C;" << c.getId() << ";" << c.getFornecedor() << ";" << c.getData() << ";"
                    << (c.isConfirmada() ? 1 : 0) << "\n";
            for (const auto& item : c.getItens()) {
                arquivo << "I;" << c.getId() << ";" << item.getIdMaterial() << ";"
                        << item.getQuantidade() << ";" << item.getPrecoUnitario() << "\n";
            }
        }
        arquivo.close();
    }

    void carregarDeArquivo(const std::string& nomeArquivo) {
        std::ifstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) return;

        compras.clear();
        int maxId = 0;
        std::string linha;

        while (std::getline(arquivo, linha)) {
            if (linha.empty()) continue;
            std::vector<std::string> c = dividirCampos(linha);
            try {
                if (c[0] == "C" && c.size() >= 5) {
                    int id = std::stoi(c[1]);
                    compras.push_back(Compra(id, c[2], c[3], c[4] == "1"));
                    if (id > maxId) maxId = id;
                } else if (c[0] == "I" && c.size() >= 5) {
                    Compra* compra = buscarPorId(std::stoi(c[1]));
                    if (compra != nullptr) {
                        compra->adicionarItem(ItemCompra(std::stoi(c[2]), std::stod(c[3]),
                                                         std::stod(c[4])));
                    }
                }
            } catch (const std::invalid_argument&) {
                std::cerr << "Aviso: linha de compra com formato inválido ignorada.\n";
            } catch (const std::out_of_range&) {
                std::cerr << "Aviso: número fora do limite em compras ignorado.\n";
            }
        }

        proximoId = maxId + 1;
        arquivo.close();
    }
};

#endif
