// Heloisa
#ifndef _GERENCIAMENTOFINANCEIRO_H_
#define _GERENCIAMENTOFINANCEIRO_H_

#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "Transacao.h"
#include "Receita.h"
#include "Despesa.h"

using namespace std;

class GerenciamentoFinanceiro{

    private:
        vector<Receita*> receitas;
        vector<Despesa*> despesas;
        int proximoId = 1;
        string ultimoErro;

        static string limparCampo(string texto){
            texto.erase(remove(texto.begin(), texto.end(), ';'), texto.end());
            texto.erase(remove(texto.begin(), texto.end(), '\n'), texto.end());
            texto.erase(remove(texto.begin(), texto.end(), '\r'), texto.end());
            return texto;
        }

        bool validar(const string& descricao, double valor){
            ultimoErro.clear();
            if (descricao.empty()) ultimoErro = "Preencha a descrição.";
            else if (valor <= 0) ultimoErro = "O valor precisa ser maior que zero.";
            return ultimoErro.empty();
        }

        void limpar(){
            for (Receita* receita : receitas) delete receita;
            for (Despesa* despesa : despesas) delete despesa;
            receitas.clear();
            despesas.clear();
        }

    public:

        GerenciamentoFinanceiro() {}
        ~GerenciamentoFinanceiro(){ limpar(); }

        // a classe e dona dos ponteiros, entao nao pode ser copiada
        GerenciamentoFinanceiro(const GerenciamentoFinanceiro&) = delete;
        GerenciamentoFinanceiro& operator=(const GerenciamentoFinanceiro&) = delete;

        string getUltimoErro() const { return ultimoErro; }

        void adicionarTransacao(Receita* receita){
            receitas.push_back(receita);
            proximoId = max(proximoId, receita->getId() + 1);
        }

        void adicionarTransacao(Despesa* despesa){
            despesas.push_back(despesa);
            proximoId = max(proximoId, despesa->getId() + 1);
        }

        // cria a receita com id novo; devolve 0 se os dados forem invalidos
        int adicionarReceita(const string& descricao, double valor, const string& data, const string& origem){
            if (!validar(limparCampo(descricao), valor)) return 0;
            int id = proximoId;
            adicionarTransacao(new Receita(id, limparCampo(descricao), valor, data, limparCampo(origem)));
            return id;
        }

        int adicionarDespesa(const string& descricao, double valor, const string& data, const string& categoria){
            if (!validar(limparCampo(descricao), valor)) return 0;
            int id = proximoId;
            adicionarTransacao(new Despesa(id, limparCampo(descricao), valor, data, limparCampo(categoria)));
            return id;
        }

        // receitas e despesas juntas, da mais recente para a mais antiga
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

        bool vazio() const { return receitas.empty() && despesas.empty(); }

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

// Arquivo: tipo;id;data;valor;descricao;origem ou categoria

        void salvarEmArquivo(const string& nomeArquivo) const{
            ofstream arquivo(nomeArquivo);
            if (!arquivo.is_open()){
                cerr << "Erro ao abrir " << nomeArquivo << " para escrita!\n";
                return;
            }

            arquivo << fixed << setprecision(2);
            for (Receita* r : receitas){
                arquivo << "R;" << r->getId() << ";" << r->getData() << ";" << r->getValor() << ";"
                        << r->getDescricao() << ";" << r->getOrigem() << "\n";
            }
            for (Despesa* d : despesas){
                arquivo << "D;" << d->getId() << ";" << d->getData() << ";" << d->getValor() << ";"
                        << d->getDescricao() << ";" << d->getCategoria() << "\n";
            }
        }

        void carregarDeArquivo(const string& nomeArquivo){
            ifstream arquivo(nomeArquivo);
            if (!arquivo.is_open()) return;

            limpar();
            proximoId = 1;
            string linha;

            while (getline(arquivo, linha)){
                if (linha.empty()) continue;

                stringstream ss(linha);
                string tipo, id, data, valor, descricao, extra;
                getline(ss, tipo, ';');
                getline(ss, id, ';');
                getline(ss, data, ';');
                getline(ss, valor, ';');
                getline(ss, descricao, ';');
                getline(ss, extra);

                try {
                    if (tipo == "R") adicionarTransacao(new Receita(stoi(id), descricao, stod(valor), data, extra));
                    else if (tipo == "D") adicionarTransacao(new Despesa(stoi(id), descricao, stod(valor), data, extra));
                } catch (const exception&) {
                    cerr << "Aviso: linha inválida ignorada em " << nomeArquivo << "\n";
                }
            }
        }
};

#endif