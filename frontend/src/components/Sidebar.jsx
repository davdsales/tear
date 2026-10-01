import './Sidebar.css'
import { Link } from 'react-router-dom'

// componente da barra lateral de navegacao do sistema
function Sidebar() {
  return (
    <aside>
      <nav>
        <Link to="/">Início</Link>
        <Link to="/estoque">Estoque</Link>
        <Link to="/compras">Compras</Link>
        <Link to="/projetos">Projetos</Link>
        <Link to="/producao">Produção</Link>
        <Link to="/pedidos">Pedidos</Link>
        <Link to="/financeiro">Financeiro</Link>
      </nav>
    </aside>
  )
}

export default Sidebar