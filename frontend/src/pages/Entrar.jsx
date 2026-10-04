import { useState } from "react";
import { useNavigate } from "react-router-dom";
import { guardarLogin } from "../usuario.js";
import "../styles/Entrar.css";

import logo from "../assets/logo.png";

export default function Entrar() {

    const [email, setemail] = useState("");
    const [senha, setsenha] = useState("");

    const navigate = useNavigate();

    async function entrar(event) {

        event.preventDefault();

        try {
            const res = await fetch("http://localhost:8080/api/login", {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify({ email, senha })
            });
            const resposta = await res.json();
            if (!res.ok) {
                alert(resposta.mensagem);
                return;
            }
            guardarLogin(resposta);
            navigate("/inicio");
        } catch {
            alert("Não consegui falar com o servidor C++. Ele está rodando?");
        }
    }

    return (
        <div className="entrar-container">

            <div className="entrar-card">

                <div className="logo">
                    <img src={logo} alt="Logo" />
                </div>

                <div className="entrar">
                    <h1>Entrar</h1>
                </div>

                <form onSubmit={entrar}>

                    <div>
                        <label>E-mail</label>

                        <input
                            type="email"
                            value={email}
                            onChange={(event) => setemail(event.target.value)}
                            placeholder="Digite seu e-mail"
                        />
                    </div>

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

                <p>
                    Ainda não possui uma conta?{" "}
                    <a href="/">Criar conta</a>
                </p>

            </div>

        </div>
    );
}