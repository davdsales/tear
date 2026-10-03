import '../styles/Inicio.css'
import { usuarioLogado, primeiroNome } from '../usuario.js'
import imgReceita from '../assets/img_receita.png'
import imgReceber from '../assets/img_receber.png'
import imgAndamento from '../assets/img_andamento.png'
import imgEstoque from '../assets/img_estoque.png'

function Inicio() {
  const usuario = usuarioLogado()
  return (
    <div className="inicio-conteudo">
      <h3>Olá, {primeiroNome(usuario?.nome)} 👋</h3>
      <p>Veja como está seu trabalho hoje.</p>

      <div className="indicadores">
        <div className="box_info">
          <span>Receita este mês</span>
          <h3>R$ 1.840</h3>
          <img src={imgReceita} alt="Cálculo Receita mensal"/>
        </div>

        <div className="box_info">
          <span>A receber</span>
          <h3>R$ 620</h3>
          <img src={imgReceber} alt="A receber"/>
        </div>

        <div className="box_info">
          <span>Pedidos em andamento</span>
          <h3>8</h3>
          <img src={imgAndamento} alt="Pedidos em andamento"/>
        </div>

        <div className="box_info">
          <span>Itens com estoque baixo</span>
          <h3>3</h3>
          <img src={imgEstoque} alt="Itens com estoque baixo"/>
        </div>
      </div>
    </div>
  )
}

export default Inicio