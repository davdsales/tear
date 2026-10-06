import { useState } from "react";
import { useNavigate } from "react-router-dom";
import { guardarLogin } from "../usuario.js";
import "../styles/Entrar.css";

import logo from "../assets/logo.png";

// tela de login para quem já tem conta
export default function Entrar() {

    // guardam o que a pessoa digita
    const [email, setemail] = useState("");
    const [senha, setsenha] = useState("");

    // serve para mudar de página
    const navigate = useNavigate();

    // roda quando a pessoa clica em entrar
    async function entrar(event) {

        // impede a página de recarregar
        event.preventDefault();

        try {
            // manda email e senha para o servidor em C++ conferir
            const res = await fetch("http://localhost:8080/api/login", {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify({ email, senha })
            });
            const resposta = await res.json();

            // se o email ou a senha estiverem errados mostra o motivo
            if (!res.ok) {
                alert(resposta.mensagem);
                return;
            }

            // salva quem entrou e vai para a tela inicial
            guardarLogin(resposta);
            navigate("/inicio");
        } catch {
            // cai aqui se o servidor estiver desligado
            alert("Não consegui falar com o servidor C++. Ele está rodando?");
        }
    }

    // o que aparece na tela
    return (
        <div className="entrar-container">

            <div className="entrar-card">

                <div className="logo">
                    <img src={logo} alt="Logo" />
                </div>

                <div className="entrar">
                    <h1>Entrar</h1>
                </div>

                {/* formulário e quando enviar chama a função entrar */}
                <form onSubmit={entrar}>

                    {/* cada campo mostra o valor guardado e atualiza quando a pessoa digita */}
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

                    <button type="submit">
                        Entrar
                    </button>

                </form>

                {/* link para criar uma conta nova */}
                <p>
                    Ainda não possui uma conta?{" "}
                    <a href="/">Criar conta</a>
                </p>

            </div>

        </div>
    );
}