import { BrowserRouter, Routes, Route } from 'react-router-dom'

import Sidebar from './components/Sidebar.jsx'
import Inicio from './pages/Inicio.jsx'
import Projetos from './pages/Projetos.jsx'
import Estoque from './pages/Estoque.jsx'
import Compras from './pages/Compras.jsx'
import PedidosKanban from './pages/PedidosKanban.jsx'
import EmConstrucao from './components/EmConstrucao.jsx'

function App() {
  return (
    <BrowserRouter>
      <div style={{ display: 'flex', minHeight: '100vh', backgroundColor: '#FDFBF7' }}>
        
        {/* Barra lateral fixa global */}
        <Sidebar />

        {/* Conteúdo principal renderizado dinamicamente */}
        <main style={{ flex: 1, padding: '32px 40px' }}>
          <Routes>
            <Route path="/" element={<Inicio />} />
            <Route path="/projetos" element={<Projetos />} />
            <Route
              path="/projetos/:id"
              element={<EmConstrucao titulo="Detalhe do Projeto" />}
            />
            <Route path="/estoque" element={<Estoque />} />
            <Route path="/compras" element={<Compras />} />
            <Route path="/pedidos" element={<PedidosKanban />} />
            <Route
              path="/financeiro"
              element={<EmConstrucao titulo="Financeiro" />}
            />
          </Routes>
        </main>

      </div>
    </BrowserRouter>
  )
}

export default App