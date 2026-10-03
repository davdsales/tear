import { useState } from "react";
import { useNavigate } from "react-router-dom";

import "../styles/Cadastro.css";
import { salvarCadastro } from "../usuario.js";
import logo from "../assets/logo.png";

export default function Cadastro() {

    const [nome, setnome] = useState("");
    const [email, setemail] = useState("");
    const [senha, setsenha] = useState("");
    const [confirmarsenha, setconfirmarsenha] = useState("");

    const navigate = useNavigate();

    function envio(event) {

        event.preventDefault();

        if (senha != confirmarsenha) {
            alert("As senhas não são iguais!");
            return;
        }

        console.log({
            nome,
            email,
            senha
        });
        
        salvarCadastro(nome, email);
        alert("Cadastro finalizado com sucesso!");

        navigate("/inicio");
    }

    return (
        <div className="cadastro-container">

            <div className="cadastro-card">

                <div className="logo">
                    <img src={logo} alt="Logo" />
                </div>

                <div className="cadastro">
                    <h1>Cadastro</h1>
                </div>

                <form onSubmit={envio}>

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

                <p>
                    Já possui uma conta?{" "}
                    <a href="/entrar">Entrar</a>
                </p>

            </div>

        </div>
    );
}