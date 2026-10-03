// Heloisa
#ifndef _GERENCIAMENTOFINANCEIRO_H_
#define _GERENCIAMENTOFINANCEIRO_H_

#include <string>
#include <vector>
#include <algorithm>
#include "Transacao.h"
#include "Receita.h"
#include "Despesa.h"
#include "BancoDados.h"

using namespace std;

// guarda as receitas e despesas em memoria (para os calculos)
// e grava cada alteracao na tabela "transacoes" do banco SQLite
class GerenciamentoFinanceiro{

    private:
        vector<Receita*> receitas;
        vector<Despesa*> despesas;
        BancoDados* banco = nullptr;
        int usuarioId = 0;
        string ultimoErro;

        bool validar(const string& descricao, double valor){
            ultimoErro.clear();
            if (!banco) ultimoErro = "Banco de dados não conectado.";
            else if (descricao.empty()) ultimoErro = "Preencha a descrição.";
            else if (valor <= 0) ultimoErro = "O valor precisa ser maior que zero.";
            return ultimoErro.empty();
        }

        void limpar(){
            for (Receita* receita : receitas) delete receita;
            for (Despesa* despesa : despesas) delete despesa;
            receitas.clear();
            despesas.clear();
        }

        transacao* buscarPorId(int id){
            for (Receita* receita : receitas) if (receita->getId() == id) return receita;
            for (Despesa* despesa : despesas) if (despesa->getId() == id) return despesa;
            return nullptr;
        }

        // le todas as linhas da tabela e recria os objetos
        void carregarDoBanco(){
            limpar();
            Consulta consulta(*banco, "SELECT id, tipo, descricao, valor, data, origem, categoria FROM transacoes WHERE usuario_id = ?");
            consulta.ligar(1, usuarioId);
            while (consulta.proximaLinha()){
                int id = consulta.inteiro(0);
                string tipo = consulta.texto(1);
                if (tipo == "Receita"){
                    adicionarTransacao(new Receita(id, consulta.texto(2), consulta.numero(3), consulta.texto(4), consulta.texto(5)));
                } else {
                    adicionarTransacao(new Despesa(id, consulta.texto(2), consulta.numero(3), consulta.texto(4), consulta.texto(6)));
                }
            }
        }

    public:

        GerenciamentoFinanceiro() {}
        ~GerenciamentoFinanceiro(){ limpar(); }

        // a classe e dona dos ponteiros, entao nao pode ser copiada
        GerenciamentoFinanceiro(const GerenciamentoFinanceiro&) = delete;
        GerenciamentoFinanceiro& operator=(const GerenciamentoFinanceiro&) = delete;

        static void criarTabela(BancoDados& b){
            b.executar(
                "CREATE TABLE IF NOT EXISTS transacoes ("
                " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                " usuario_id INTEGER NOT NULL REFERENCES usuarios(id),"
                " tipo TEXT NOT NULL CHECK (tipo IN ('Receita', 'Despesa')),"
                " descricao TEXT NOT NULL,"
                " valor REAL NOT NULL CHECK (valor > 0),"
                " data TEXT NOT NULL,"
                " origem TEXT,"
                " categoria TEXT)");
        }

        // liga ao banco e carrega so as transacoes deste usuario
        void conectar(BancoDados& b, int idUsuario){
            banco = &b;
            usuarioId = idUsuario;
            carregarDoBanco();
        }

        string getUltimoErro() const { return ultimoErro; }

        void adicionarTransacao(Receita* receita){
            receitas.push_back(receita);
        }

        void adicionarTransacao(Despesa* despesa){
            despesas.push_back(despesa);
        }

// CRUD

        // create: grava no banco e usa o id gerado por ele; devolve 0 se der errado
        int adicionarReceita(const string& descricao, double valor, const string& data, const string& origem){
            if (!validar(descricao, valor)) return 0;

            Consulta insert(*banco, "INSERT INTO transacoes (usuario_id, tipo, descricao, valor, data, origem) VALUES (?, 'Receita', ?, ?, ?, ?)");
            insert.ligar(1, usuarioId);
            insert.ligar(2, descricao);
            insert.ligar(3, valor);
            insert.ligar(4, data);
            insert.ligar(5, origem);
            if (!insert.executar()){ ultimoErro = "Não consegui salvar no banco."; return 0; }

            int id = banco->ultimoId();
            adicionarTransacao(new Receita(id, descricao, valor, data, origem));
            return id;
        }

        int adicionarDespesa(const string& descricao, double valor, const string& data, const string& categoria){
            if (!validar(descricao, valor)) return 0;

            Consulta insert(*banco, "INSERT INTO transacoes (usuario_id, tipo, descricao, valor, data, categoria) VALUES (?, 'Despesa', ?, ?, ?, ?)");
            insert.ligar(1, usuarioId);
            insert.ligar(2, descricao);
            insert.ligar(3, valor);
            insert.ligar(4, data);
            insert.ligar(5, categoria);
            if (!insert.executar()){ ultimoErro = "Não consegui salvar no banco."; return 0; }

            int id = banco->ultimoId();
            adicionarTransacao(new Despesa(id, descricao, valor, data, categoria));
            return id;
        }

        // read: receitas e despesas juntas, da mais recente para a mais antiga
        vector<const transacao*> listarTransacoes() const{
            vector<const transacao*> todas;
            for (Receita* receita : receitas) todas.push_back(receita);
            for (Despesa* despesa : despesas) todas.push_back(despesa);

            sort(todas.begin(), todas.end(), [](const transacao* a, const transacao* b){
                if (a->getData() != b->getData()) return a->getData() > b->getData();
                return a->getId() > b->getId();
            });
            return todas;
        }

        // update: o tipo nao muda; receita usa a origem e despesa usa a categoria
        bool editarTransacao(int id, const string& descricao, double valor, const string& data,
                             const string& origem, const string& categoria){
            if (!validar(descricao, valor)) return false;

            transacao* t = buscarPorId(id);
            if (!t){ ultimoErro = "Transação não encontrada."; return false; }

            Receita* receita = dynamic_cast<Receita*>(t);
            string coluna = receita ? "origem" : "categoria";
            string extra = receita ? origem : categoria;

            Consulta update(*banco, "UPDATE transacoes SET descricao = ?, valor = ?, data = ?, " + coluna + " = ? WHERE id = ? AND usuario_id = ?");
            update.ligar(1, descricao);
            update.ligar(2, valor);
            update.ligar(3, data);
            update.ligar(4, extra);
            update.ligar(5, id);
            update.ligar(6, usuarioId);
            if (!update.executar()){ ultimoErro = "Não consegui salvar no banco."; return false; }

            t->setDescricao(descricao);
            t->setValor(valor);
            t->setData(data);
            if (receita) receita->setOrigem(extra);
            else dynamic_cast<Despesa*>(t)->setCategoria(extra);
            return true;
        }

        // delete
        bool removerTransacao(int id){
            ultimoErro.clear();
            if (!buscarPorId(id)){ ultimoErro = "Transação não encontrada."; return false; }

            Consulta del(*banco, "DELETE FROM transacoes WHERE id = ? AND usuario_id = ?");
            del.ligar(1, id);
            del.ligar(2, usuarioId);
            if (!del.executar()){ ultimoErro = "Não consegui apagar no banco."; return false; }

            for (auto it = receitas.begin(); it != receitas.end(); ++it){
                if ((*it)->getId() == id){ delete *it; receitas.erase(it); return true; }
            }
            for (auto it = despesas.begin(); it != despesas.end(); ++it){
                if ((*it)->getId() == id){ delete *it; despesas.erase(it); return true; }
            }
            return true;
        }

// Calculos

        double totalReceitas(){

            double total = 0;

            for (Receita* receita : receitas){
                total = total + receita->getValor();
            }

            return total;
        }

        double totalDespesas(){

            double total = 0;

            for (Despesa* despesa : despesas){
                total = total + despesa->getValor();
            }

            return total;
        }

        double calculoLucro(){

            return totalReceitas() - totalDespesas();
        }

//Por mês

        double receitaMes(string mes){

            double total = 0;

            for (Receita* receita : receitas){

                if (receita->getData().substr(0, 7) == mes){
                    total += receita->getValor();
                }
            }

            return total;
        }

        double despesaMes(string mes){

            double total = 0;

            for (Despesa* despesa : despesas){

                if (despesa->getData().substr(0, 7) == mes){
                    total += despesa->getValor();
                }
            }

            return total;
        }

// Receita por origem (ex: encomenda, feiras, vendas)

        double receitasPorOrigem(string origem){

            double total = 0;

            for (Receita* receita : receitas){

                if (receita->getOrigem() == origem){
                    total += receita->getValor();
                }
            }

            return total;
        }

// Despesa por categoria

        double despesasPorCategoria(string categoria){

            double total = 0;

            for (Despesa* despesa : despesas){

                if (despesa->getCategoria() == categoria){
                    total += despesa->getValor();
                }
            }

            return total;
        }
};

#endif