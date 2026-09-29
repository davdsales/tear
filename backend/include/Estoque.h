// Maria Gabriela
#ifndef _ESTOQUE_H_
#define _ESTOQUE_H_

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include "Material.h"
#include "MovimentacaoEstoque.h"

// divide uma linha do .txt em campos, mantendo os campos vazios
inline std::vector<std::string> dividirCampos(const std::string& linha) {
    std::vector<std::string> campos;
    std::string atual;
    for (char c : linha) {
        if (c == ';') {
            campos.push_back(atual);
            atual.clear();
        } else {
            atual += c;
        }
    }
    campos.push_back(atual);
    return campos;
}

// guarda os materiais (por ponteiro, para o polimorfismo funcionar) e todas as movimentacoes.
// o saldo nunca e um numero editavel: ele e sempre a soma das movimentacoes.
class Estoque {
private:
    std::vector<Material*> materiais;
    std::vector<MovimentacaoEstoque> movimentacoes;
    int proximoIdMaterial;
    int proximoIdMovimentacao;
    std::string ultimoErro;

    static constexpr double EPS = 1e-9;

    void limparMateriais() {
        for (Material* m : materiais) delete m;
        materiais.clear();
    }

    // valida e grava uma movimentacao; se algo estiver errado, explica em ultimoErro
    bool registrar(int idMaterial, TipoMovimentacao tipo, double quantidade,
                   const std::string& data, const std::string& observacao) {
        ultimoErro.clear();
        Material* m = buscarPorId(idMaterial);
        if (m == nullptr) {
            ultimoErro = "Material #" + std::to_string(idMaterial) + " não encontrado.";
            return false;
        }

        if (tipo == TipoMovimentacao::AJUSTE) {
            if (std::fabs(quantidade) < EPS) {
                ultimoErro = "O ajuste precisa ser diferente de zero.";
                return false;
            }
        } else if (quantidade <= EPS) {
            ultimoErro = "A quantidade precisa ser maior que zero.";
            return false;
        }

        double saldo = calcularSaldo(idMaterial);
        double novoSaldo = saldo;
        if (tipo == TipoMovimentacao::CONSUMO) novoSaldo -= quantidade;
        else novoSaldo += quantidade;

        if (novoSaldo < -EPS) {
            ultimoErro = "Estoque insuficiente de " + m->getNome() + ": você tentou retirar " +
                         formatarNumero(std::fabs(quantidade)) + " " + m->getUnidade() +
                         ", mas existem apenas " + formatarNumero(saldo) + " " + m->getUnidade() + ".";
            return false;
        }

        movimentacoes.push_back(MovimentacaoEstoque(proximoIdMovimentacao++, idMaterial, tipo,
                                                    quantidade, data, observacao));
        return true;
    }

public:
    Estoque() : proximoIdMaterial(1), proximoIdMovimentacao(1) {}
    ~Estoque() { limparMateriais(); }

    // o estoque e dono dos ponteiros, entao nao pode ser copiado
    Estoque(const Estoque&) = delete;
    Estoque& operator=(const Estoque&) = delete;

    std::string getUltimoErro() const { return ultimoErro; }

    // --- materiais (create / read / update / delete) ---

    // assume a posse do ponteiro e devolve o id gerado
    int adicionarMaterial(Material* material) {
        material->setId(proximoIdMaterial++);
        materiais.push_back(material);
        return material->getId();
    }

    Material* buscarPorId(int id) const {
        for (Material* m : materiais) {
            if (m->getId() == id) return m;
        }
        return nullptr;
    }

    // material com historico nao pode ser apagado, senao o saldo e as compras perdem o sentido
    bool removerMaterial(int id) {
        ultimoErro.clear();
        for (const auto& mov : movimentacoes) {
            if (mov.getIdMaterial() == id) {
                ultimoErro = "Este material já tem movimentações de estoque e não pode ser excluído.";
                return false;
            }
        }
        for (auto it = materiais.begin(); it != materiais.end(); ++it) {
            if ((*it)->getId() == id) {
                delete *it;
                materiais.erase(it);
                return true;
            }
        }
        ultimoErro = "Material #" + std::to_string(id) + " não encontrado.";
        return false;
    }

    const std::vector<Material*>& getMateriais() const { return materiais; }

    // --- movimentacoes ---

    bool registrarEntrada(int idMaterial, double quantidade, const std::string& data,
                          const std::string& observacao = "") {
        return registrar(idMaterial, TipoMovimentacao::ENTRADA, quantidade, data, observacao);
    }

    bool registrarConsumo(int idMaterial, double quantidade, const std::string& data,
                          const std::string& observacao = "") {
        return registrar(idMaterial, TipoMovimentacao::CONSUMO, quantidade, data, observacao);
    }

    bool registrarAjuste(int idMaterial, double quantidade, const std::string& data,
                         const std::string& observacao = "") {
        return registrar(idMaterial, TipoMovimentacao::AJUSTE, quantidade, data, observacao);
    }

    const std::vector<MovimentacaoEstoque>& getMovimentacoes() const { return movimentacoes; }

    // --- consultas ---

    double calcularSaldo(int idMaterial) const {
        double saldo = 0.0;
        for (const auto& mov : movimentacoes) {
            if (mov.getIdMaterial() == idMaterial) saldo += mov.getVariacao();
        }
        return saldo;
    }

    bool estaAbaixoDoMinimo(const Material& material) const {
        return calcularSaldo(material.getId()) < material.getEstoqueMinimo() - EPS;
    }

    // usado no card "Itens com estoque baixo" da tela inicial
    int contarAbaixoDoMinimo() const {
        int total = 0;
        for (const Material* m : materiais) {
            if (estaAbaixoDoMinimo(*m)) total++;
        }
        return total;
    }

    // --- persistencia em arquivo texto (mesmo padrao do GerenciadorOrcamentos) ---

    void salvarEmArquivo(const std::string& arquivoMateriais,
                         const std::string& arquivoMovimentacoes) const {
        std::ofstream arqMat(arquivoMateriais);
        if (!arqMat.is_open()) {
            std::cerr << "Erro ao abrir " << arquivoMateriais << " para escrita!\n";
            return;
        }
        for (const Material* m : materiais) arqMat << m->serializar() << "\n";
        arqMat.close();

        std::ofstream arqMov(arquivoMovimentacoes);
        if (!arqMov.is_open()) {
            std::cerr << "Erro ao abrir " << arquivoMovimentacoes << " para escrita!\n";
            return;
        }
        for (const auto& mov : movimentacoes) {
            arqMov << mov.getId() << ";"
                   << mov.getIdMaterial() << ";"
                   << static_cast<int>(mov.getTipo()) << ";"
                   << mov.getQuantidade() << ";"
                   << mov.getData() << ";"
                   << mov.getObservacao() << "\n";
        }
        arqMov.close();
    }

    void carregarDeArquivo(const std::string& arquivoMateriais,
                           const std::string& arquivoMovimentacoes) {
        limparMateriais();
        movimentacoes.clear();
        int maxIdMaterial = 0;
        int maxIdMov = 0;

        std::ifstream arqMat(arquivoMateriais);
        std::string linha;
        while (arqMat.is_open() && std::getline(arqMat, linha)) {
            if (linha.empty()) continue;
            std::vector<std::string> c = dividirCampos(linha);
            try {
                // c: tipo;id;nome;unidade;custo;minimo;extras...
                if (c.size() < 7) continue;
                int id = std::stoi(c[1]);
                double custo = std::stod(c[4]);
                double minimo = std::stod(c[5]);
                Material* m = nullptr;

                if (c[0] == "FIO" && c.size() >= 9) {
                    m = new Fio(id, c[2], c[3], custo, minimo, c[6], c[7], std::stod(c[8]));
                } else if (c[0] == "TECIDO" && c.size() >= 8) {
                    m = new Tecido(id, c[2], c[3], custo, minimo, c[6], std::stod(c[7]));
                } else if (c[0] == "AVIAMENTO") {
                    m = new Aviamento(id, c[2], c[3], custo, minimo, c[6]);
                }

                if (m != nullptr) {
                    materiais.push_back(m);
                    if (id > maxIdMaterial) maxIdMaterial = id;
                }
            } catch (const std::invalid_argument&) {
                std::cerr << "Aviso: linha de material com formato inválido ignorada.\n";
            } catch (const std::out_of_range&) {
                std::cerr << "Aviso: número fora do limite em materiais ignorado.\n";
            }
        }
        arqMat.close();

        std::ifstream arqMov(arquivoMovimentacoes);
        while (arqMov.is_open() && std::getline(arqMov, linha)) {
            if (linha.empty()) continue;
            std::vector<std::string> c = dividirCampos(linha);
            try {
                // c: id;idMaterial;tipo;quantidade;data;observacao
                if (c.size() < 6) continue;
                int id = std::stoi(c[0]);
                int idMaterial = std::stoi(c[1]);
                int tipo = std::stoi(c[2]);
                double qtd = std::stod(c[3]);
                if (tipo < 0 || tipo > 2) continue;

                movimentacoes.push_back(MovimentacaoEstoque(id, idMaterial,
                                        static_cast<TipoMovimentacao>(tipo), qtd, c[4], c[5]));
                if (id > maxIdMov) maxIdMov = id;
            } catch (const std::invalid_argument&) {
                std::cerr << "Aviso: linha de movimentação com formato inválido ignorada.\n";
            } catch (const std::out_of_range&) {
                std::cerr << "Aviso: número fora do limite em movimentações ignorado.\n";
            }
        }
        arqMov.close();

        proximoIdMaterial = maxIdMaterial + 1;
        proximoIdMovimentacao = maxIdMov + 1;
    }
};

#endif
