import { useState } from "react";
import { useNavigate } from "react-router-dom";

import "../styles/Cadastro.css";
import { guardarLogin } from "../usuario.js";
import logo from "../assets/logo.png";

// tela de cadastro de uma conta nova
export default function Cadastro() {

    // guardam o que a pessoa digita em cada campo
    const [nome, setnome] = useState("");
    const [email, setemail] = useState("");
    const [senha, setsenha] = useState("");
    const [confirmarsenha, setconfirmarsenha] = useState("");

    // serve para mudar de página
    const navigate = useNavigate();

    // roda quando a pessoa clica em cadastrar
    async function envio(event) {

        // impede a página de recarregar
        event.preventDefault();

        // confere se as duas senhas são iguais
        if (senha != confirmarsenha) {
            alert("As senhas não são iguais!");
            return;
        }

        try {
            // manda nome email e senha para o servidor em C++
            const res = await fetch("http://localhost:8080/api/usuarios", {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify({ nome, email, senha })
            });
            const resposta = await res.json();

            // se o servidor recusar mostra o motivo
            if (!res.ok) {
                alert(resposta.mensagem);
                return;
            }

            // salva quem entrou e vai para a tela inicial
            guardarLogin(resposta);
            alert("Cadastro finalizado com sucesso!");
            navigate("/inicio");
        } catch {
            // cai aqui se o servidor estiver desligado
            alert("Não consegui falar com o servidor C++. Ele está rodando?");
        }
    }

    // o que aparece na tela
    return (
        <div className="cadastro-container">

            <div className="cadastro-card">

                <div className="logo">
                    <img src={logo} alt="Logo" />
                </div>

                <div className="cadastro">
                    <h1>Cadastro</h1>
                </div>

                {/* formulário e quando enviar chama a função envio */}
                <form onSubmit={envio}>

                    {/* cada campo mostra o valor guardado e atualiza quando a pessoa digita */}
                    <div>
                        <label>Nome</label>

                        <input
                            type="text"
                            value={nome}
                            onChange={(event) => setnome(event.target.value)}
                            placeholder="Digite seu nome"
                        />
                    </div>

                    <div>
                        <label>E-mail</label>

                        <input
                            type="email"
                            value={email}
                            onChange={(event) => setemail(event.target.value)}
                            placeholder="Digite seu e-mail"
                        />
                    </div>

                    {/* password esconde o que é digitado */}
                    <div>
                        <label>Senha</label>

                        <input
                            type="password"
                            value={senha}
                            onChange={(event) => setsenha(event.target.value)}
                            placeholder="Digite sua senha"
                        />
                    </div>

                    <div>
                        <label>Confirmar senha</label>

                        <input
                            type="password"
                            value={confirmarsenha}
                            onChange={(event) => setconfirmarsenha(event.target.value)}
                            placeholder="Confirme sua senha"
                        />
                    </div>

                    <button type="submit">
                        Cadastrar
                    </button>

                </form>

                {/* link para quem já tem conta */}
                <p>
                    Já possui uma conta?{" "}
                    <a href="/entrar">Entrar</a>
                </p>

            </div>

        </div>
    );
}