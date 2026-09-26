//David
#ifndef _GERENCIADOR_ORCAMENTOS_H_
#define _GERENCIADOR_ORCAMENTOS_H_

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include "Orcamento.h"

// classe que gerencia a lista de orcamentos e o arquivo txt
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

    // salva os orcamentos no arquivo de texto separado por ponto e virgula
    void salvarEmArquivo(const std::string& nomeArquivo) const {
        std::ofstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) {
            std::cerr << "Erro ao abrir o arquivo para escrita!\n";
            return;
        }

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

    // le o arquivo txt tratando possiveis erros de formato com try catch
    void carregarDeArquivo(const std::string& nomeArquivo) {
        std::ifstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) return;

        orcamentos.clear();
        std::string linha;
        int maxId = 0;

        while (std::getline(arquivo, linha)) {
            if (linha.empty()) continue;

            std::stringstream ss(linha);
            std::string temp;

            try {
                int id;
                std::string nomeCliente, contatoCliente;
                double mat, mao, adic, margem, desc;

                if (!std::getline(ss, temp, ';')) continue;
                id = std::stoi(temp);

                std::getline(ss, nomeCliente, ';');
                std::getline(ss, contatoCliente, ';');

                if (!std::getline(ss, temp, ';')) continue;
                mat = std::stod(temp);

                if (!std::getline(ss, temp, ';')) continue;
                mao = std::stod(temp);

                if (!std::getline(ss, temp, ';')) continue;
                adic = std::stod(temp);

                if (!std::getline(ss, temp, ';')) continue;
                margem = std::stod(temp);

                if (!std::getline(ss, temp, ';')) continue;
                desc = std::stod(temp);

                Cliente c(0, nomeCliente, contatoCliente);
                Orcamento o(id, c, mat, mao, adic, margem, desc);
                orcamentos.push_back(o);

                if (id > maxId) maxId = id;

            } catch (const std::invalid_argument& e) {
                std::cerr << "Aviso: Linha com formato inválido ignorada no arquivo.\n";
            } catch (const std::out_of_range& e) {
                std::cerr << "Aviso: Número fora do limite permitido no arquivo.\n";
            }
        }

        proximoId = maxId + 1;
        arquivo.close();
    }
};

#endif