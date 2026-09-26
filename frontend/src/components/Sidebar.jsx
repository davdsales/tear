import React from 'react';

// componente da barra lateral de navegacao do sistema
function Sidebar() {
  // altera a hash da url para navegar entre as telas
  const navegar = (hash) => {
    window.location.hash = hash;
  };

  return (
    <aside style={{ width: '220px', background: '#FFF', padding: '20px', borderRight: '1px solid #EEE' }}>
      <h2 style={{ marginBottom: '30px' }}>Tear</h2>
      <nav style={{ display: 'flex', flexDirection: 'column', gap: '15px' }}>
        <button 
          onClick={() => navegar('#Inicio')}
          style={{ textAlign: 'left', background: 'none', border: 'none', cursor: 'pointer' }}
        >
          Início
        </button>
        <button 
          onClick={() => navegar('#Pedidos')}
          style={{ textAlign: 'left', background: 'none', border: 'none', cursor: 'pointer' }}
        >
          Pedidos
        </button>
      </nav>
    </aside>
  );
}

export default Sidebar;