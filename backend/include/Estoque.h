// Maria Gabriela
#ifndef _ESTOQUE_H_
#define _ESTOQUE_H_

#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include "Material.h"
#include "MovimentacaoEstoque.h"
#include "BancoDados.h"

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

    // --- persistencia no banco SQLite (tabelas materiais e movimentacoes) ---
    // cada usuario tem o seu estoque: todas as linhas guardam o usuario_id

    static void criarTabelas(BancoDados& banco) {
        banco.executar(
            "CREATE TABLE IF NOT EXISTS materiais ("
            " usuario_id INTEGER NOT NULL REFERENCES usuarios(id),"
            " id INTEGER NOT NULL,"
            " tipo TEXT NOT NULL CHECK (tipo IN ('FIO', 'TECIDO', 'AVIAMENTO')),"
            " nome TEXT NOT NULL,"
            " unidade TEXT NOT NULL,"
            " custo_unitario REAL NOT NULL,"
            " estoque_minimo REAL NOT NULL,"
            " marca TEXT, cor TEXT, metragem REAL,"   // fio
            " composicao TEXT, largura REAL,"         // tecido
            " detalhe TEXT,"                          // aviamento
            " PRIMARY KEY (usuario_id, id))");
        banco.executar(
            "CREATE TABLE IF NOT EXISTS movimentacoes ("
            " usuario_id INTEGER NOT NULL REFERENCES usuarios(id),"
            " id INTEGER NOT NULL,"
            " id_material INTEGER NOT NULL,"
            " tipo INTEGER NOT NULL,"                 // 0 entrada, 1 consumo, 2 ajuste
            " quantidade REAL NOT NULL,"
            " data TEXT NOT NULL,"
            " observacao TEXT,"
            " PRIMARY KEY (usuario_id, id))");
    }

    // grava o estado atual inteiro do usuario (mesma ideia do antigo salvarEmArquivo)
    void salvarNoBanco(BancoDados& banco, int usuarioId) const {
        banco.executar("BEGIN");
        Consulta apagarMovs(banco, "DELETE FROM movimentacoes WHERE usuario_id = ?");
        apagarMovs.ligar(1, usuarioId);
        apagarMovs.executar();
        Consulta apagarMats(banco, "DELETE FROM materiais WHERE usuario_id = ?");
        apagarMats.ligar(1, usuarioId);
        apagarMats.executar();

        for (const Material* m : materiais) {
            Consulta insert(banco,
                "INSERT INTO materiais (usuario_id, id, tipo, nome, unidade, custo_unitario, estoque_minimo,"
                " marca, cor, metragem, composicao, largura, detalhe)"
                " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
            insert.ligar(1, usuarioId);
            insert.ligar(2, m->getId());
            insert.ligar(3, m->getTipo());
            insert.ligar(4, m->getNome());
            insert.ligar(5, m->getUnidade());
            insert.ligar(6, m->getCustoUnitario());
            insert.ligar(7, m->getEstoqueMinimo());
            if (const Fio* f = dynamic_cast<const Fio*>(m)) {
                insert.ligar(8, f->getMarca());
                insert.ligar(9, f->getCor());
                insert.ligar(10, f->getMetragem());
            } else if (const Tecido* t = dynamic_cast<const Tecido*>(m)) {
                insert.ligar(11, t->getComposicao());
                insert.ligar(12, t->getLargura());
            } else if (const Aviamento* a = dynamic_cast<const Aviamento*>(m)) {
                insert.ligar(13, a->getDetalhe());
            }
            insert.executar();
        }

        for (const auto& mov : movimentacoes) {
            Consulta insert(banco,
                "INSERT INTO movimentacoes (usuario_id, id, id_material, tipo, quantidade, data, observacao)"
                " VALUES (?, ?, ?, ?, ?, ?, ?)");
            insert.ligar(1, usuarioId);
            insert.ligar(2, mov.getId());
            insert.ligar(3, mov.getIdMaterial());
            insert.ligar(4, static_cast<int>(mov.getTipo()));
            insert.ligar(5, mov.getQuantidade());
            insert.ligar(6, mov.getData());
            insert.ligar(7, mov.getObservacao());
            insert.executar();
        }
        banco.executar("COMMIT");
    }

    void carregarDoBanco(BancoDados& banco, int usuarioId) {
        limparMateriais();
        movimentacoes.clear();
        int maxIdMaterial = 0;
        int maxIdMov = 0;

        Consulta mats(banco,
            "SELECT id, tipo, nome, unidade, custo_unitario, estoque_minimo,"
            " marca, cor, metragem, composicao, largura, detalhe FROM materiais"
            " WHERE usuario_id = ? ORDER BY id");
        mats.ligar(1, usuarioId);
        while (mats.proximaLinha()) {
            int id = mats.inteiro(0);
            std::string tipo = mats.texto(1);
            Material* m = nullptr;

            if (tipo == "FIO") {
                m = new Fio(id, mats.texto(2), mats.texto(3), mats.numero(4), mats.numero(5),
                            mats.texto(6), mats.texto(7), mats.numero(8));
            } else if (tipo == "TECIDO") {
                m = new Tecido(id, mats.texto(2), mats.texto(3), mats.numero(4), mats.numero(5),
                               mats.texto(9), mats.numero(10));
            } else if (tipo == "AVIAMENTO") {
                m = new Aviamento(id, mats.texto(2), mats.texto(3), mats.numero(4), mats.numero(5),
                                  mats.texto(11));
            }
            if (m != nullptr) {
                materiais.push_back(m);
                if (id > maxIdMaterial) maxIdMaterial = id;
            }
        }

        Consulta movs(banco,
            "SELECT id, id_material, tipo, quantidade, data, observacao FROM movimentacoes"
            " WHERE usuario_id = ? ORDER BY id");
        movs.ligar(1, usuarioId);
        while (movs.proximaLinha()) {
            int id = movs.inteiro(0);
            int tipo = movs.inteiro(2);
            if (tipo < 0 || tipo > 2) continue;
            movimentacoes.push_back(MovimentacaoEstoque(id, movs.inteiro(1),
                                    static_cast<TipoMovimentacao>(tipo), movs.numero(3),
                                    movs.texto(4), movs.texto(5)));
            if (id > maxIdMov) maxIdMov = id;
        }

        proximoIdMaterial = maxIdMaterial + 1;
        proximoIdMovimentacao = maxIdMov + 1;
    }
};

#endif