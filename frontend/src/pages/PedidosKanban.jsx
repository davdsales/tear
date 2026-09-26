import React, { useEffect, useState } from 'react';

// componente do quadro kanban com suporte a arrastar e soltar cartoes entre colunas
function PedidosKanban() {
  const [pedidos, setPedidos] = useState([]);
  const [carregando, setCarregando] = useState(true);

  // estados dos campos do formulario
  const [cliente, setCliente] = useState('');
  const [contato, setContato] = useState('');
  const [materiais, setMateriais] = useState('');
  const [maoDeObra, setMaoDeObra] = useState('');

  // colunas fixas do fluxo de trabalho
  const colunas = ['Em Aberto', 'Em Produção', 'Concluído'];

  // busca os pedidos cadastrados no backend c++
  const buscarPedidos = () => {
    fetch('http://localhost:8080/api/pedidos')
      .then((res) => res.json())
      .then((data) => {
        setPedidos(data);
        setCarregando(false);
      })
      .catch((err) => console.error("Erro ao buscar pedidos:", err));
  };

  useEffect(() => {
    buscarPedidos();
  }, []);

  // cria um novo pedido e recarrega a lista
  const handleSubmit = (e) => {
    e.preventDefault();

    const novoPedido = {
      cliente,
      contato,
      materiais: parseFloat(materiais) || 0,
      maoDeObra: parseFloat(maoDeObra) || 0,
      adicionais: 0,
      margem: 0.20,
      desconto: 0,
      status: 'Em Aberto'
    };

    fetch('http://localhost:8080/api/pedidos', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(novoPedido)
    })
      .then((res) => {
        if (!res.ok) throw new Error("Erro na resposta do C++");
        return res.json();
      })
      .then(() => {
        setCliente('');
        setContato('');
        setMateriais('');
        setMaoDeObra('');
        buscarPedidos();
      })
      .catch(() => alert("Certifique-se de que o backend C++ (sistema.exe) está rodando!"));
  };

  // guarda o id do pedido transferido ao iniciar o arrasto
  const handleDragStart = (e, id) => {
    e.dataTransfer.setData('text/plain', String(id));
    e.dataTransfer.effectAllowed = 'move';
  };

  // autoriza a zona de soltura
  const handleDragOver = (e) => {
    e.preventDefault();
    e.dataTransfer.dropEffect = 'move';
  };

  // executa a movimentacao do card para a nova coluna
  const handleDrop = (e, novoStatus) => {
    e.preventDefault();
    const idStr = e.dataTransfer.getData('text/plain');
    if (!idStr) return;

    const id = parseInt(idStr, 10);

    // atualizacao visual imediata no estado local
    setPedidos((prev) =>
      prev.map((p) => (p.id === id ? { ...p, status: novoStatus } : p))
    );

    // persiste a alteracao de status no backend c++
    fetch(`http://localhost:8080/api/pedidos/${id}`, {
      method: 'PUT',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ status: novoStatus })
    }).catch((err) => {
      console.error("Erro ao sincronizar status com C++:", err);
    });
  };

  return (
    <div style={{ fontFamily: '"DM Sans", sans-serif' }}>
      <h3 style={{ fontSize: '24px', fontWeight: 'bold', marginBottom: '20px', color: '#000000', fontFamily: '"DM Sans", sans-serif' }}>
        Quadro de Pedidos (Kanban)
      </h3>

      {/* formulario de criacao do pedido */}
      <form onSubmit={handleSubmit} style={{ background: '#FFF', padding: '16px 20px', borderRadius: '16px', border: '1px solid #F3EFEA', marginBottom: '30px', display: 'flex', gap: '12px', flexWrap: 'wrap', alignItems: 'center' }}>
        <input 
          type="text" 
          placeholder="Nome do Cliente" 
          value={cliente} 
          onChange={(e) => setCliente(e.target.value)} 
          required 
          style={{ padding: '10px 14px', border: '1px solid #EFEAE4', borderRadius: '10px', fontSize: '14px', flex: 1, minWidth: '150px', outline: 'none', background: '#FAF8F5', fontFamily: '"DM Sans", sans-serif' }}
        />
        <input 
          type="text" 
          placeholder="Contato / Tel" 
          value={contato} 
          onChange={(e) => setContato(e.target.value)} 
          style={{ padding: '10px 14px', border: '1px solid #EFEAE4', borderRadius: '10px', fontSize: '14px', flex: 1, minWidth: '130px', outline: 'none', background: '#FAF8F5', fontFamily: '"DM Sans", sans-serif' }}
        />
        <input 
          type="number" 
          placeholder="Materiais (R$)" 
          value={materiais} 
          onChange={(e) => setMateriais(e.target.value)} 
          style={{ padding: '10px 14px', border: '1px solid #EFEAE4', borderRadius: '10px', fontSize: '14px', width: '120px', outline: 'none', background: '#FAF8F5', fontFamily: '"DM Sans", sans-serif' }}
        />
        <input 
          type="number" 
          placeholder="Mão de Obra (R$)" 
          value={maoDeObra} 
          onChange={(e) => setMaoDeObra(e.target.value)} 
          style={{ padding: '10px 14px', border: '1px solid #EFEAE4', borderRadius: '10px', fontSize: '14px', width: '130px', outline: 'none', background: '#FAF8F5', fontFamily: '"DM Sans", sans-serif' }}
        />
        <button 
          type="submit" 
          style={{ padding: '10px 20px', background: '#D96B27', color: '#FFF', border: 'none', borderRadius: '10px', fontSize: '14px', fontWeight: 'bold', cursor: 'pointer', fontFamily: '"DM Sans", sans-serif' }}
        >
          Criar Pedido
        </button>
      </form>

      {/* colunas kanban */}
      {carregando ? (
        <p style={{ color: '#786F6A' }}>Carregando dados do servidor C++...</p>
      ) : (
        <div style={{ display: 'flex', gap: '20px', alignItems: 'flex-start', overflowX: 'auto', paddingBottom: '10px' }}>
          {colunas.map((colunaStatus) => {
            const pedidosDaColuna = pedidos.filter(
              (p) => (p.status || 'Em Aberto') === colunaStatus
            );

            return (
              <div 
                key={colunaStatus}
                onDragOver={handleDragOver}
                onDrop={(e) => handleDrop(e, colunaStatus)}
                style={{
                  background: '#F7F4EF',
                  borderRadius: '16px',
                  padding: '16px',
                  width: '300px',
                  minHeight: '480px',
                  flexShrink: 0,
                  border: '2px dashed #E5DEC9'
                }}
              >
                {/* cabeçalho da coluna */}
                <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '16px' }}>
                  <h4 style={{ margin: 0, fontSize: '16px', fontWeight: 'bold', color: '#2B231F' }}>
                    {colunaStatus}
                  </h4>
                  <span style={{ background: '#EAE5DF', padding: '2px 8px', borderRadius: '12px', fontSize: '12px', fontWeight: 'bold', color: '#524B46' }}>
                    {pedidosDaColuna.length}
                  </span>
                </div>

                {/* conteiner interno de cartoes */}
                <div style={{ display: 'flex', flexDirection: 'column', gap: '12px', minHeight: '400px' }}>
                  {pedidosDaColuna.map((p) => (
                    <div 
                      key={p.id}
                      draggable={true}
                      onDragStart={(e) => handleDragStart(e, p.id)}
                      style={{ 
                        background: '#FFF', 
                        padding: '16px', 
                        borderRadius: '12px', 
                        border: '1px solid #EFEAE4', 
                        boxShadow: '0 2px 4px rgba(0,0,0,0.04)',
                        cursor: 'grab',
                        userSelect: 'none'
                      }}
                    >
                      <div style={{ pointerEvents: 'none' }}>
                        <h3 style={{ margin: '0 0 8px 0', fontSize: '16px', fontWeight: 'bold', color: '#000000' }}>
                          {p.cliente}
                        </h3>
                        
                        {p.descricao && (
                          <p style={{ margin: '4px 0', fontSize: '13px', color: '#786F6A' }}>
                            <strong>Descrição:</strong> {p.descricao}
                          </p>
                        )}
                        
                        {p.contato && (
                          <p style={{ margin: '4px 0', fontSize: '13px', color: '#786F6A' }}>
                            <strong>Contato:</strong> {p.contato}
                          </p>
                        )}

                        <div style={{ marginTop: '12px', display: 'flex', justifyContent: 'space-between', alignItems: 'center', paddingTop: '8px', borderTop: '1px solid #F8F6F3' }}>
                          <span style={{ fontSize: '11px', color: '#A89F98' }}>ID #{p.id}</span>
                          <span style={{ fontSize: '14px', fontWeight: 'bold', color: '#D96B27' }}>
                            R$ {Number(p.valor || 0).toFixed(2)}
                          </span>
                        </div>
                      </div>
                    </div>
                  ))}
                </div>
              </div>
            );
          })}
        </div>
      )}
    </div>
  );
}

export default PedidosKanban;