#include <iostream>
#include <limits>
#include <algorithm>
#include <clocale>
#include <windows.h>
#include <iomanip>
#include "GerenciadorOrcamentos.h"

using namespace std;

const string NOME_ARQUIVO = "orcamentos.txt";

// limpa o buffer caso o usuário digite letras onde deveria ser número
void limparBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// remove caracteres ';' das strings para não quebrar a estrutura do arquivo .txt
string removePVEntrada(string texto) {
    texto.erase(remove(texto.begin(), texto.end(), ';'), texto.end());
    return texto;
}

// função auxiliar para garantir a leitura segura de valores monetários/numéricos
double lerDoubleValido(const string& mensagem) {
    double valor;
    while (true) {
        cout << mensagem;
        if (cin >> valor && valor >= 0.0) {
            return valor;
        }
        cout << "Entrada inválida! Digite um número positivo válido.\n";
        limparBuffer();
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    GerenciadorOrcamentos gerenciador;
    
    // leitura automática dos dados
    gerenciador.carregarDeArquivo(NOME_ARQUIVO);

    int opcao = -1;

    while (opcao != 0) {
        cout << "\n======== Sistema de gestão de orçamentos ========\n";
        cout << "1. Cadastrar novo orçamento\n";
        cout << "2. Listar todos os orçamentos\n";
        cout << "3. Buscar orçamento por id\n";
        cout << "4. Remover orçamento\n";
        cout << "0. Salvar e sair\n";
        cout << "Escolha uma opção: ";
        cin >> opcao;

        if (cin.fail()) {
            limparBuffer();
            cout << "Opção inválida! Digite apenas números.\n";
            continue;
        }

        switch (opcao) {
            case 1: {
                string nome, contato;

                limparBuffer();
                cout << "\n--- Cadastro de orçamento ---\n";
                cout << "Nome do cliente: ";
                getline(cin, nome);
                cout << "Contato (telefone/e-mail): ";
                getline(cin, contato);

                // remove ";" da entrada, caso tenha
                nome = removePVEntrada(nome); // PV = ponto e vírgula
                contato = removePVEntrada(contato);

                double mat = lerDoubleValido("Custo de materiais (R$): ");
                double mao = lerDoubleValido("Custo de mão de obra (R$): ");
                double adic = lerDoubleValido("Custos adicionais (R$): ");
                double margem = lerDoubleValido("Margem de lucro desejada (ex: 0.20 para 20%): ");
                double desc = lerDoubleValido("Desconto (R$): ");

                Cliente cliente(0, nome, contato);
                gerenciador.adicionarOrcamento(cliente, mat, mao, adic, margem, desc);
                break;
            }
            case 2:
                gerenciador.listarOrcamentos();
                break;

            case 3: {
                int id;
                cout << "\nDigite o id do orçamento: ";
                cin >> id;

                if (cin.fail()) {
                    limparBuffer();
                    cout << "Id inválido!\n";
                    break;
                }

                Orcamento* o = gerenciador.buscarPorId(id);
                if (o != nullptr) {
                    cout << "\n--- Orçamento encontrado ---\n";
                    cout << "Id: " << o->getId() << "\n";
                    cout << "Cliente: " << o->getCliente().getNome() << "\n";
                    cout << "Contato: " << o->getCliente().getContato() << "\n";
                    cout << "Custo total: R$ " << o->calcularCustoTotal() << "\n";
                    cout << "Preço bruto: R$ " << o->calcularPrecoBruto() << "\n";
                    cout << "Preço final: R$ " << o->calcularPrecoFinal() << "\n";
                    cout << "Margem efetiva: " << o->calcularMargemPercentual() << "%\n";
                } else {
                    cout << "\nOrçamento não encontrado!\n";
                }
                break;
            }
            case 4: {
                int id;
                cout << "\nDigite o id do orçamento a remover: ";
                cin >> id;

                if (cin.fail()) {
                    limparBuffer();
                    cout << "Id inválido!\n";
                    break;
                }

                if (gerenciador.removerOrcamento(id)) {
                    cout << "\nOrçamento #" << id << " removido com sucesso!\n";
                } else {
                    cout << "\nOrçamento não encontrado!\n";
                }
                break;
            }
            case 0:
                gerenciador.salvarEmArquivo(NOME_ARQUIVO);
                cout << "\nDados salvos com sucesso em '" << NOME_ARQUIVO << "'. Encerrando programa...\n";
                break;

            default:
                cout << "\nOpção inválida!\n";
                break;
        }
    }

    return 0;
}