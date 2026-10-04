import { BrowserRouter, Routes, Route } from 'react-router-dom'

import Sidebar from './components/Sidebar.jsx'

import Cadastro from './pages/Cadastro.jsx'
import Entrar from './pages/Entrar.jsx'
import Inicio from './pages/Inicio.jsx'

import Projetos from './pages/Projetos.jsx'
import Estoque from './pages/Estoque.jsx'
import Compras from './pages/Compras.jsx'
import PedidosKanban from './pages/PedidosKanban.jsx'

import EmConstrucao from './components/EmConstrucao.jsx'

import NovoOrcamento from './pages/NovoOrcamento.jsx'

function App() {

  return (

    <BrowserRouter>

      <Routes>

        {/* Tela de cadastro */}
        <Route path="/" element={<Cadastro />} />

        {/* Tela de login */}
        <Route path="/entrar" element={<Entrar />} />

        {/* Sistema */}
        <Route
          path="/*"
          element={
            <div style={{
              display: 'flex',
              minHeight: '100vh',
              backgroundColor: '#FDFBF7'
            }}>

              <Sidebar />

              <main style={{
                flex: 1,
                padding: '32px 40px'
              }}>

                <Routes>

                  <Route path="/inicio" element={<Inicio />} />

                  <Route path="/projetos" element={<Projetos />} />

                  <Route
                    path="/projetos/:id"
                    element={
                      <EmConstrucao titulo="Detalhe do Projeto" />
                    }
                  />

                  <Route path="/estoque" element={<Estoque />} />

                  <Route path="/compras" element={<Compras />} />

                  <Route path="/pedidos" element={<PedidosKanban />} />

                  <Route path="/orcamento" element={<NovoOrcamento />} />

                  <Route
                    path="/financeiro"
                    element={
                      <EmConstrucao titulo="Financeiro" />
                    }
                  />

                </Routes>

              </main>

            </div>
          }
        />

      </Routes>

    </BrowserRouter>
  )
}

export default App