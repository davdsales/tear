import './Sidebar.css'
import { NavLink } from 'react-router-dom'
import logo from '../assets/logo.png'
import { usuarioLogado, iniciais } from '../usuario.js'

// componente da barra lateral de navegacao do sistema
function Sidebar() {
  const usuario = usuarioLogado()   

  return (
    <aside className="sidebar">
      <div className="sidebar-marca">
        <img src={logo} alt="Davigurumi" className="sidebar-logo" />
      </div>


      <nav>
        <NavLink to="/inicio" end>Início</NavLink>
        <NavLink to="/estoque">Estoque</NavLink>
        <NavLink to="/compras">Orçamento</NavLink>
        <NavLink to="/pedidos">Pedidos</NavLink>

      </nav>


      <div className="sidebar-usuario">
        <div className="sidebar-avatar">{iniciais(usuario?.nome)}</div>
        <div className="sidebar-usuario-info">
          <strong>{usuario ? usuario.nome : 'Visitante'}</strong>
          <small>{usuario ? usuario.email : 'Faça login'}</small>
        </div>
        <span className="sidebar-seta" />
      </div>
    </aside>
  )
}

export default Sidebar