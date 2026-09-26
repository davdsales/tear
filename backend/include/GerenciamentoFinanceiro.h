#ifndef _GERENCIAMENTOFINANCEIRO_H_
#define _GERENCIAMENTOFINANCEIRO_H_

#include <string>
#include <vector>
#include "Transacao.h"
#include "Receita.h"
#include "Despesa.h"

using namespace std;

class GerenciamentoFinanceiro{

    private:
        vector<Receita*> receitas;
        vector<Despesa*> despesas;

    public:

        void adicionarTransacao(Receita* receita){
            receitas.push_back(receita);
        }

        void adicionarTransacao(Despesa* despesa){
            despesas.push_back(despesa);
        }

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