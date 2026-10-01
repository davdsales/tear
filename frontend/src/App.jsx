import { BrowserRouter, Routes, Route } from 'react-router-dom'
import Inicio from './pages/Inicio.jsx'
import Projetos from './pages/Projetos.jsx'
import EmConstrucao from './components/EmConstrucao.jsx'
import PedidosKanban from './pages/PedidosKanban.jsx'

// componente principal que controla o roteamento da aplicacao
function App() {
  return (
    <BrowserRouter>
      <Routes>
        <Route path="/" element={<Inicio />} />
        <Route path="/projetos" element={<Projetos />} />
        <Route
          path="/projetos/:id"
          element={<EmConstrucao titulo="Detalhe do Projeto" />}
        />
        <Route
          path="/estoque"
          element={<EmConstrucao titulo="Estoque" />}
        />
        <Route
          path="/compras"
          element={<EmConstrucao titulo="Compras" />}
        />
        <Route
          path="/pedidos"
          element={<PedidosKanban />}
        />
        <Route
          path="/financeiro"
          element={<EmConstrucao titulo="Financeiro" />}
        />
      </Routes>
    </BrowserRouter>
  )
}

export default App