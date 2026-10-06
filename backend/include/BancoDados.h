#ifndef _BANCODADOS_H_
#define _BANCODADOS_H_

#include <string>
#include <iostream>
#include "../sqlite/sqlite3.h"

// conexao com o arquivo do banco SQLite (Abre no construtor e fecha no destrutor)
class BancoDados {
    private:
        sqlite3* db = nullptr;
        friend class Consulta;

    public:
        explicit BancoDados(const std::string& caminho){
            if (sqlite3_open(caminho.c_str(), &db) != SQLITE_OK){
                std::cerr << "Nao consegui abrir o banco " << caminho << ": " << sqlite3_errmsg(db) << "\n";
            }
        }
        ~BancoDados(){ sqlite3_close(db); }

        // a conexao e unica, então o objeto nao pode ser copiado
        BancoDados(const BancoDados&) = delete;
        BancoDados& operator=(const BancoDados&) = delete;


        bool executar(const std::string& sql){
            char* erro = nullptr;
            if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &erro) != SQLITE_OK){
                std::cerr << "Erro no SQL: " << (erro ? erro : "") << "\n";
                sqlite3_free(erro);
                return false;
            }
            return true;
        }

        // id gerado pelo ultimo INSERT (AUTOINCREMENT)
        int ultimoId(){ return static_cast<int>(sqlite3_last_insert_rowid(db)); }
};

// uma consulta preparada (INSERT, SELECT, UPDATE ou DELETE)
// os valores entram com "?" no SQL e sao ligados com ligar(), o que evita fraudes
// o destrutor libera a consulta sozinho
class Consulta {
    private:
        sqlite3_stmt* stmt = nullptr;

    public:
        Consulta(BancoDados& banco, const std::string& sql){
            if (sqlite3_prepare_v2(banco.db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK){
                std::cerr << "Erro no SQL: " << sqlite3_errmsg(banco.db) << "\n";
                stmt = nullptr;
            }
        }
        ~Consulta(){ sqlite3_finalize(stmt); }

        Consulta(const Consulta&) = delete;
        Consulta& operator=(const Consulta&) = delete;

        // as posicoes comecam em 1, na ordem dos "?" do SQL
        void ligar(int posicao, const std::string& valor){
            sqlite3_bind_text(stmt, posicao, valor.c_str(), -1, SQLITE_TRANSIENT);
        }
        void ligar(int posicao, double valor){ sqlite3_bind_double(stmt, posicao, valor); }
        void ligar(int posicao, int valor){ sqlite3_bind_int(stmt, posicao, valor); }

        // SELECT: avanca para a proxima linha; devolve false quando acabam as linhas
        bool proximaLinha(){ return stmt && sqlite3_step(stmt) == SQLITE_ROW; }

        // INSERT, UPDATE e DELETE: executa e diz se deu certo
        bool executar(){ return stmt && sqlite3_step(stmt) == SQLITE_DONE; }

        // as colunas comecam em 0, na ordem do SELECT
        int inteiro(int coluna){ return sqlite3_column_int(stmt, coluna); }
        double numero(int coluna){ return sqlite3_column_double(stmt, coluna); }
        std::string texto(int coluna){
            const unsigned char* t = sqlite3_column_text(stmt, coluna);
            return t ? reinterpret_cast<const char*>(t) : "";
        }
};

#endif