import '../styles/Inicio.css'

import Sidebar from "../components/Sidebar";
import imgReceita from '../assets/img_receita.png'
import imgReceber from '../assets/img_receber.png'
import imgAndamento from '../assets/img_andamento.png'
import imgEstoque from '../assets/img_estoque.png'

function Inicio() {
  return (
    <div className="app-layout">
      <Sidebar />

      <div className="inicio-conteudo">
        <h3>Olá, David 👋</h3>
        <a>Veja como está seu trabalho hoje.</a>

        <div className="indicadores">
          <div className="box_info">
            <a>Receita este mês</a>
            <h3>R$ 1.840</h3>
            <img src={imgReceita} alt="Cálculo Receita mensal"/>
          </div>

          <div className="box_info">
            <a>A receber</a>
            <h3>R$ 620</h3>
            <img src={imgReceber} alt="A receber"/>
          </div>

          <div className="box_info">
            <a>Pedidos em andamento</a>
            <h3>8</h3>
            <img src={imgAndamento} alt="Pedidos em andamento"/>
          </div>

          <div className="box_info">
            <a>Itens com estoque baixo</a>
            <h3>3</h3>
            <img src={imgEstoque} alt="Itens com estoque baixo"/>
          </div>
        </div>
      </div>
    </div>
  )
}

export default Inicio