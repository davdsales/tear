import { useState } from "react";
import { useNavigate } from "react-router-dom";
import { entrarComEmail } from "../usuario.js";
import "../styles/Entrar.css";

import logo from "../assets/logo.png";

export default function Entrar() {

    const [email, setemail] = useState("");
    const [senha, setsenha] = useState("");

    const navigate = useNavigate();

    function entrar(event) {

        event.preventDefault();

        console.log({
            email,
            senha
        });
        entrarComEmail(email);
        alert("Login realizado com sucesso!");

        navigate("/inicio");
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