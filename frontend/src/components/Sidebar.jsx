import './Sidebar.css'
import { Link } from 'react-router-dom'

function Sidebar() {
  return (
    <aside className="sidebar">
      <h2>Tear</h2>

      <nav>
        <Link to="/">Início</Link>
        <Link to="/estoque">Estoque</Link>
        <Link to="/compras">Compras</Link>
        <Link to="/projetos">Projetos</Link>
        <Link to="/projetos">Produção</Link>
        <Link to="/pedidos">Pedidos</Link>
        <Link to="/financeiro">Financeiro</Link>
      </nav>
    </aside>
  )
}

export default Sidebar