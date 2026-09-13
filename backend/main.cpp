#include <iostream>
#include <limits>
#include "GerenciadorOrcamentos.h"

using namespace std;

const string NOME_ARQUIVO = "orcamentos.txt";

void limparBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    GerenciadorOrcamentos gerenciador;
    
    // Leitura automatica do arquivo ao iniciar
    gerenciador.carregarDeArquivo(NOME_ARQUIVO);

    int opcao = -1;

    while (opcao != 0) {
        cout << "\n======== SISTEMA DE GESTAO DE ORCAMENTOS ========\n";
        cout << "1. Cadastrar novo orçamento\n";
        cout << "2. Listar todos os orçamentos\n";
        cout << "3. Buscar orçamento por ID\n";
        cout << "4. Remover orçamento\n";
        cout << "0. Salvar e sair\n";
        cout << "Escolha uma opcão: ";
        cin >> opcao;

        if (cin.fail()) {
            limparBuffer();
            cout << "Opcao invalida! Digite apenas numeros.\n";
            continue;
        }

        switch (opcao) {
            case 1: {
                string nome, contato;
                double mat, mao, adic, margem, desc;

                limparBuffer();
                cout << "\n--- CADASTRO DE ORÇAMENTO ---\n";
                cout << "Nome do Cliente: ";
                getline(cin, nome);
                cout << "Contato (Telefone/Email): ";
                getline(cin, contato);

                cout << "Custo de Materiais (R$): ";
                cin >> mat;
                cout << "Custo de Mao de Obra (R$): ";
                cin >> mao;
                cout << "Custos Adicionais (R$): ";
                cin >> adic;
                cout << "Margem de Lucro Desejada (ex: 0.20 para 20%): ";
                cin >> margem;
                cout << "Desconto (R$): ";
                cin >> desc;

                Cliente cliente(0, nome, contato);
                gerenciador.adicionarOrcamento(cliente, mat, mao, adic, margem, desc);
                break;
            }
            case 2:
                gerenciador.listarOrcamentos();
                break;

            case 3: {
                int id;
                cout << "\nDigite o ID do Orçamento: ";
                cin >> id;

                Orcamento* o = gerenciador.buscarPorId(id);
                if (o != nullptr) {
                    cout << "\n--- ORÇAMENTO ENCONTRADO ---\n";
                    cout << "ID: " << o->getId() << "\n";
                    cout << "Cliente: " << o->getCliente().getNome() << "\n";
                    cout << "Contato: " << o->getCliente().getContato() << "\n";
                    cout << "Custo Total: R$ " << o->calcularCustoTotal() << "\n";
                    cout << "Preco Bruto: R$ " << o->calcularPrecoBruto() << "\n";
                    cout << "Preco Final: R$ " << o->calcularPrecoFinal() << "\n";
                    cout << "Margem Efetiva: " << o->calcularMargemPercentual() << "%\n";
                } else {
                    cout << "\nOrçamento nao encontrado!\n";
                }
                break;
            }
            case 4: {
                int id;
                cout << "\nDigite o ID do Orçamento a remover: ";
                cin >> id;

                if (gerenciador.removerOrcamento(id)) {
                    cout << "\nOrçamento #" << id << " removido com sucesso!\n";
                } else {
                    cout << "\nOrçamento nao encontrado!\n";
                }
                break;
            }
            case 0:
                gerenciador.salvarEmArquivo(NOME_ARQUIVO);
                cout << "\nDados salvos em '" << NOME_ARQUIVO << "'. Encerrando programa...\n";
                break;

            default:
                cout << "\nOpcao invalida!\n";
                break;
        }
    }

    return 0;
}