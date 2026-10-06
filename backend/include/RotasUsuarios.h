#ifndef _ROTAS_USUARIOS_H_
#define _ROTAS_USUARIOS_H_

#include <string>
#include <algorithm>
#include <cctype>
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "BancoDados.h"
#include "RotasEstoque.h"  // reaproveita responder, responderErro e lerTexto

// cadastro e login usando a tabela usuarios do banco
namespace rotas_usuarios {

using json = nlohmann::json;
using rotas_estoque::responder;
using rotas_estoque::responderErro;
using rotas_estoque::lerTexto;

// e-mail sem espacos nas pontas e em minusculas, para "Ana@x.com" e "ana@x.com" serem o mesmo
inline std::string normalizarEmail(std::string email) {
    email.erase(0, email.find_first_not_of(' '));
    email.erase(email.find_last_not_of(' ') + 1);
    std::transform(email.begin(), email.end(), email.begin(),[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return email;
}

}  // namespace rotas_usuarios

inline void registrarRotasUsuarios(httplib::Server& svr, BancoDados& banco) {
    using namespace rotas_usuarios;

    // a tabela usuarios e criada pelo SessaoUsuarios (ContextoUsuario.h)

    // cadastro (nome, email, senha)
    svr.Post("/api/usuarios", [&banco](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);
            std::string nome = lerTexto(body, "nome");
            std::string email = normalizarEmail(lerTexto(body, "email"));
            std::string senha = lerTexto(body, "senha");

            if (nome.empty() || email.empty() || senha.empty()) return responderErro(res, 400, "Preencha nome, e-mail e senha.");
            if (email.find('@') == std::string::npos) return responderErro(res, 400, "E-mail inválido.");

            Consulta existe(banco, "SELECT id FROM usuarios WHERE email = ?");
            existe.ligar(1, email);
            if (existe.proximaLinha()) return responderErro(res, 409, "Já existe uma conta com esse e-mail.");

            Consulta insert(banco, "INSERT INTO usuarios (nome, email, senha) VALUES (?, ?, ?)");
            insert.ligar(1, nome);
            insert.ligar(2, email);
            insert.ligar(3, senha);
            if (!insert.executar()) return responderErro(res, 500, "Não consegui salvar o cadastro.");

            responder(res, 201, {{"status", "sucesso"}, {"id", banco.ultimoId()}, {"nome", nome}, {"email", email}});
        } catch (const std::exception&) {
            responderErro(res, 400, "Dados inválidos no corpo da requisição.");
        }
    });

    // login (email, senha)
    svr.Post("/api/login", [&banco](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);
            Consulta consulta(banco, "SELECT id, nome, email FROM usuarios WHERE email = ? AND senha = ?");
            consulta.ligar(1, normalizarEmail(lerTexto(body, "email")));
            consulta.ligar(2, lerTexto(body, "senha"));

            if (!consulta.proximaLinha()) return responderErro(res, 401, "E-mail ou senha incorretos.");
            responder(res, 200, {{"status", "sucesso"}, {"id", consulta.inteiro(0)}, {"nome", consulta.texto(1)}, {"email", consulta.texto(2)}});
        } catch (const std::exception&) {
            responderErro(res, 400, "Dados inválidos no corpo da requisição.");
        }
    });
}

#endif