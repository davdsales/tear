#ifndef _GERENCIADOR_ORCAMENTOS_H_
#define _GERENCIADOR_ORCAMENTOS_H_
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Orcamento.h"
using namespace std;

class GerenciadorOrcamentos {
private:
    vector<Orcamento> orcamentos;
    int proximoId;

public:
    GerenciadorOrcamentos() : proximoId(1) {}
    // adiciona um novo orçamento gerando um id automaticamente
    void adicionarOrcamento(const Cliente& cliente, double custoMateriais, double custoMaoDeObra, double custoAdicionais, double margemLucro, double desconto) {
        Orcamento novoOrcamento(proximoId++, cliente, custoMateriais, custoMaoDeObra, custoAdicionais, margemLucro, desconto);
        orcamentos.push_back(novoOrcamento);
        cout << "Orcamento #" << novoOrcamento.getId() << " cadastrado com sucesso!\n";
    }

    // buscar por id
    Orcamento* buscarPorId(int id) {
        for (auto& orcamento : orcamentos) {
            if (orcamento.getId() == id) {
                return &orcamento;
            }
        }
        return nullptr; // não encontrado
    }

    // remover um orçamento pelo id
    bool removerOrcamento(int id) {
        for (auto i = orcamentos.begin(); i != orcamentos.end(); ++i) {
            if (i->getId() == id) {
                orcamentos.erase(i);
                return true;
            }
        }
        return false;
    }

    // lista todos os orçamentos cadastrados
    void listarOrcamentos() const {
        if (orcamentos.empty()) {
            cout << "Nenhum orcamento cadastrado.\n";
            return;
        }

        cout << "\n================ LISTA DE ORCAMENTOS ================\n";
        for (const auto& o : orcamentos) {
            cout << "ID: " << o.getId() 
                      << " | Cliente: " << o.getCliente().getNome()
                      << " | Custo Total: R$ " << o.calcularCustoTotal()
                      << " | Preco Final: R$ " << o.calcularPrecoFinal()
                      << " | Margem: " << o.calcularMargemPercentual() << "%\n";
        }
        cout << "=====================================================\n";
    }

    // retorna todos os orçamentos (caso precise salvar em arquivo depois)
    const vector<Orcamento>& getTodosOrcamentos() const {
        return orcamentos;
    }
    void salvarEmArquivo(const string& nomeArquivo) const {
        ofstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) return;
        for (const auto& o : orcamentos) {
            arquivo << o.getId() << ";"
                    << o.getCliente().getNome() << ";"
                    << o.getCliente().getContato() << ";"
                    << o.getCustoMateriais() << ";"
                    << o.getCustoMaoDeObra() << ";"
                    << o.getCustoAdicionais() << ";"
                    << o.getMargemLucroDesejada() << ";"
                    << o.getDesconto() << "\n";
        }
        arquivo.close();
    }
    void carregarDeArquivo(const string& nomeArquivo) {
        ifstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) return;
        orcamentos.clear();
        string linha;
        int maxId = 0;
        while (getline(arquivo, linha)) {
            if (linha.empty()) continue;
            stringstream ss(linha);
            string temp;

            int id;
            string nomeCliente, contatoCliente;
            double mat, mao, adic, margem, desc;

            getline(ss, temp, ';'); id = stoi(temp);
            getline(ss, nomeCliente, ';');
            getline(ss, contatoCliente, ';');
            getline(ss, temp, ';'); mat = stod(temp);
            getline(ss, temp, ';'); mao = stod(temp);
            getline(ss, temp, ';'); adic = stod(temp);
            getline(ss, temp, ';'); margem = stod(temp);
            getline(ss, temp, ';'); desc = stod(temp);

            Cliente c(0, nomeCliente, contatoCliente);
            Orcamento o(id, c, mat, mao, adic, margem, desc);
            orcamentos.push_back(o);

            if (id > maxId) maxId = id;
        }
        proximoId = maxId + 1;
        arquivo.close();
    }
};

#endif