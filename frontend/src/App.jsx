import React, { useState, useEffect } from 'react'
import Inicio from './pages/Inicio.jsx'
import PedidosKanban from './pages/PedidosKanban.jsx'
import Sidebar from './components/Sidebar.jsx'

// componente principal que controla o roteamento por hash da aplicacao
function App() {
  const [rotaAtual, setRotaAtual] = useState(window.location.hash || '#Inicio');

  // escuta as mudancas de hash na url para atualizar a tela
  useEffect(() => {
    const aoMudarRota = () => {
      setRotaAtual(window.location.hash || '#Inicio');
    };

    window.addEventListener('hashchange', aoMudarRota);
    return () => window.removeEventListener('hashchange', aoMudarRota);
  }, []);

  // renderiza o kanban com a barra lateral se a rota for #pedidos
  if (rotaAtual === '#Pedidos') {
    return (
      <div style={{ display: 'flex', minHeight: '100vh', backgroundColor: '#FDFBF7' }}>
        <Sidebar />
        <main style={{ flex: 1, padding: '30px' }}>
          <PedidosKanban />
        </main>
      </div>
    );
  }

  // renderiza a pagina inicial por padrao
  return <Inicio />;
}

export default App;