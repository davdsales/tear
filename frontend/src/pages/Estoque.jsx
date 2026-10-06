import React, { useState, useEffect } from 'react'
import Compras from './Compras.jsx'
import '../styles/estoqueCompras.css'

// endereço do servidor em C++
const API = 'http://localhost:8080'

// formulário em branco de material novo
const MATERIAL_VAZIO = {
  tipo: 'FIO', nome: '', unidade: 'g', custoUnitario: '', estoqueMinimo: '', estoqueInicial: '',
  marca: '', cor: '', metragem: '', composicao: '', largura: '', detalhe: ''
}

// unidade que já vem escolhida para cada tipo de material
const UNIDADE_PADRAO = { FIO: 'g', TECIDO: 'm', AVIAMENTO: 'un' }

// mostra o número como dinheiro tipo R$ 12,50
function dinheiro(valor) {
  return 'R$ ' + Number(valor).toFixed(2).replace('.', ',')
}

// consumo aparece com menos e entrada com mais e o ajuste fica como foi digitado
function comSinal(mov) {
  const q = Number(mov.quantidade)
  if (mov.tipo === 'Consumo') return '-' + Math.abs(q)
  return q > 0 ? '+' + q : String(q)
}

// aba de materiais com a lista e os formulários
function Materiais() {
  // guardam as informações da tela
  const [materiais, setMateriais] = useState([])
  const [resumo, setResumo] = useState({ totalMateriais: 0, abaixoDoMinimo: 0 })
  const [mensagem, setMensagem] = useState(null)
  const [mostrarNovo, setMostrarNovo] = useState(false)
  const [novo, setNovo] = useState(MATERIAL_VAZIO)
  const [mov, setMov] = useState(null)
  const [edicao, setEdicao] = useState(null)
  const [historico, setHistorico] = useState(null)

  // busca os dados assim que a tela abre
  useEffect(() => { carregar() }, [])

  // pede ao servidor a lista de materiais e o resumo ao mesmo tempo
  async function carregar() {
    try {
      const [resMat, resResumo] = await Promise.all([
        fetch(API + '/api/materiais'),
        fetch(API + '/api/estoque')
      ])
      setMateriais(await resMat.json())
      setResumo(await resResumo.json())
    } catch {
      setMensagem({ erro: true, texto: 'Não consegui falar com o servidor C++. Ele está rodando?' })
    }
  }

  // manda o pedido para o servidor e mostra a mensagem de certo ou de erro
  async function enviar(url, metodo, corpo, textoOk) {
    try {
      const res = await fetch(API + url, {
        method: metodo,
        headers: { 'Content-Type': 'application/json' },
        body: corpo ? JSON.stringify(corpo) : undefined
      })
      const dados = await res.json()
      if (!res.ok) {
        setMensagem({ erro: true, texto: dados.mensagem })
        return false
      }
      setMensagem({ erro: false, texto: textoOk })
      carregar()
      return true
    } catch {
      setMensagem({ erro: true, texto: 'Não consegui falar com o servidor C++. Ele está rodando?' })
      return false
    }
  }

  // muda um campo do material novo e se trocar o tipo troca a unidade também
  function mudarNovo(campo, valor) {
    const atualizado = { ...novo, [campo]: valor }
    if (campo === 'tipo') atualizado.unidade = UNIDADE_PADRAO[valor]
    setNovo(atualizado)
  }

  // salva o material novo e se der certo limpa o formulário
  async function salvarNovo(e) {
    e.preventDefault()
    if (await enviar('/api/materiais', 'POST', novo, 'Material cadastrado.')) {
      setNovo(MATERIAL_VAZIO)
      setMostrarNovo(false)
    }
  }

  // registra uma entrada consumo ou ajuste no estoque
  async function salvarMovimentacao(e) {
    e.preventDefault()
    const corpo = { idMaterial: mov.material.id, tipo: mov.tipo, quantidade: mov.quantidade, observacao: mov.observacao }
    if (await enviar('/api/estoque/movimentacoes', 'POST', corpo, 'Movimentação registrada.')) {
      setMov(null)
    }
  }

  // salva o que foi mudado no material
  async function salvarEdicao(e) {
    e.preventDefault()
    const corpo = {
      nome: edicao.nome,
      unidade: edicao.unidade,
      custoUnitario: edicao.custoUnitario,
      estoqueMinimo: edicao.estoqueMinimo
    }
    if (await enviar('/api/materiais/' + edicao.id, 'PUT', corpo, 'Material atualizado.')) {
      setEdicao(null)
    }
  }

  // busca todas as movimentações de um material
  async function verHistorico(m) {
    try {
      const res = await fetch(API + '/api/estoque/movimentacoes?idMaterial=' + m.id)
      setHistorico({ material: m, lista: await res.json() })
    } catch {
      setMensagem({ erro: true, texto: 'Não consegui carregar o histórico.' })
    }
  }

  // pergunta antes de apagar o material
  function excluir(m) {
    if (window.confirm('Excluir "' + m.nome + '"?')) {
      enviar('/api/materiais/' + m.id, 'DELETE', null, 'Material excluído.')
    }
  }

  // o que aparece na tela
  return (
    <div className="ec-pagina">
      {/* botão que abre e fecha o formulário de material novo */}
      <div className="ec-topo ec-topo-direita">
        <button className="ec-botao" onClick={() => setMostrarNovo(!mostrarNovo)}>
          {mostrarNovo ? 'Fechar' : '+ Novo material'}
        </button>
      </div>

      {/* aviso de sucesso ou de erro */}
      {mensagem && <div className={'ec-aviso' + (mensagem.erro ? '' : ' ok')}>{mensagem.texto}</div>}

      {/* cards com o total de materiais e quantos estão com estoque baixo */}
      <div className="ec-resumo">
        <div className="ec-card"><span>Materiais cadastrados</span><strong>{resumo.totalMateriais}</strong></div>
        <div className="ec-card"><span>Itens com estoque baixo</span><strong>{resumo.abaixoDoMinimo}</strong></div>
      </div>

      {/* formulário de material novo */}
      {mostrarNovo && (
        <form className="ec-form" onSubmit={salvarNovo}>
          <h2>Novo material</h2>
          <div className="ec-grade">
            <label className="ec-campo">Tipo
              <select value={novo.tipo} onChange={e => mudarNovo('tipo', e.target.value)}>
                <option value="FIO">Fio</option>
                <option value="TECIDO">Tecido</option>
                <option value="AVIAMENTO">Aviamento</option>
              </select>
            </label>
            <label className="ec-campo">Nome
              <input value={novo.nome} onChange={e => mudarNovo('nome', e.target.value)} required />
            </label>
            <label className="ec-campo">Unidade
              <input value={novo.unidade} onChange={e => mudarNovo('unidade', e.target.value)} />
            </label>
            <label className="ec-campo">Custo por unidade (R$)
              <input type="number" step="0.001" min="0" value={novo.custoUnitario} onChange={e => mudarNovo('custoUnitario', e.target.value)} />
            </label>
            <label className="ec-campo">Estoque mínimo
              <input type="number" step="any" min="0" value={novo.estoqueMinimo} onChange={e => mudarNovo('estoqueMinimo', e.target.value)} />
            </label>
            <label className="ec-campo">Estoque inicial
              <input type="number" step="any" min="0" value={novo.estoqueInicial} onChange={e => mudarNovo('estoqueInicial', e.target.value)} />
            </label>

            {/* os campos de baixo mudam conforme o tipo escolhido */}
            {novo.tipo === 'FIO' && (
              <>
                <label className="ec-campo">Marca
                  <input value={novo.marca} onChange={e => mudarNovo('marca', e.target.value)} />
                </label>
                <label className="ec-campo">Cor
                  <input value={novo.cor} onChange={e => mudarNovo('cor', e.target.value)} />
                </label>
                <label className="ec-campo">Metragem do novelo (m)
                  <input type="number" step="any" min="0" value={novo.metragem} onChange={e => mudarNovo('metragem', e.target.value)} />
                </label>
              </>
            )}
            {novo.tipo === 'TECIDO' && (
              <>
                <label className="ec-campo">Composição
                  <input value={novo.composicao} onChange={e => mudarNovo('composicao', e.target.value)} />
                </label>
                <label className="ec-campo">Largura (cm)
                  <input type="number" step="any" min="0" value={novo.largura} onChange={e => mudarNovo('largura', e.target.value)} />
                </label>
              </>
            )}
            {novo.tipo === 'AVIAMENTO' && (
              <label className="ec-campo">Detalhe
                <input value={novo.detalhe} onChange={e => mudarNovo('detalhe', e.target.value)} />
              </label>
            )}
          </div>
          <div className="ec-botoes">
            <button className="ec-botao" type="submit">Salvar</button>
            <button className="ec-botao secundario" type="button" onClick={() => setMostrarNovo(false)}>Cancelar</button>
          </div>
        </form>
      )}

      {/* formulário para movimentar o estoque de um material */}
      {mov && (
        <form className="ec-form" onSubmit={salvarMovimentacao}>
          <h2>Movimentar: {mov.material.descricao}</h2>
          <div className="ec-grade">
            <label className="ec-campo">Tipo
              <select value={mov.tipo} onChange={e => setMov({ ...mov, tipo: e.target.value })}>
                <option value="ENTRADA">Entrada</option>
                <option value="CONSUMO">Consumo</option>
                <option value="AJUSTE">Ajuste (use negativo para reduzir)</option>
              </select>
            </label>
            <label className="ec-campo">Quantidade ({mov.material.unidade})
              <input type="number" step="any" value={mov.quantidade} onChange={e => setMov({ ...mov, quantidade: e.target.value })} required />
            </label>
            <label className="ec-campo">Observação
              <input value={mov.observacao} onChange={e => setMov({ ...mov, observacao: e.target.value })} />
            </label>
          </div>
          <div className="ec-botoes">
            <button className="ec-botao" type="submit">Registrar</button>
            <button className="ec-botao secundario" type="button" onClick={() => setMov(null)}>Cancelar</button>
          </div>
        </form>
      )}

      {/* formulário para editar um material */}
      {edicao && (
        <form className="ec-form" onSubmit={salvarEdicao}>
          <h2>Editar: {edicao.descricao}</h2>
          <div className="ec-grade">
            <label className="ec-campo">Nome
              <input value={edicao.nome} onChange={e => setEdicao({ ...edicao, nome: e.target.value })} required />
            </label>
            <label className="ec-campo">Unidade
              <input value={edicao.unidade} onChange={e => setEdicao({ ...edicao, unidade: e.target.value })} />
            </label>
            <label className="ec-campo">Custo por unidade (R$)
              <input type="number" step="0.001" min="0" value={edicao.custoUnitario} onChange={e => setEdicao({ ...edicao, custoUnitario: e.target.value })} />
            </label>
            <label className="ec-campo">Estoque mínimo
              <input type="number" step="any" min="0" value={edicao.estoqueMinimo} onChange={e => setEdicao({ ...edicao, estoqueMinimo: e.target.value })} />
            </label>
          </div>
          <div className="ec-botoes">
            <button className="ec-botao" type="submit">Salvar</button>
            <button className="ec-botao secundario" type="button" onClick={() => setEdicao(null)}>Cancelar</button>
          </div>
        </form>
      )}

      {/* histórico de movimentações de um material */}
      {historico && (
        <div className="ec-form">
          <h2>Histórico: {historico.material.descricao}</h2>
          <table className="ec-tabela">
            <thead>
              <tr><th>Data</th><th>Tipo</th><th>Quantidade</th><th>Observação</th></tr>
            </thead>
            <tbody>
              {historico.lista.length === 0 && (
                <tr><td colSpan="4" className="ec-vazio">Nenhuma movimentação ainda.</td></tr>
              )}
              {/* mostra da mais recente para a mais antiga */}
              {[...historico.lista].sort((a, b) => b.data.localeCompare(a.data) || b.id - a.id).map(h => (
                <tr key={h.id}>
                  <td>{h.data}</td>
                  <td>{h.tipo}</td>
                  <td>{comSinal(h)} {historico.material.unidade}</td>
                  <td>{h.observacao || '—'}</td>
                </tr>
              ))}
            </tbody>
          </table>
          <div className="ec-botoes" style={{ marginTop: 14 }}>
            <button className="ec-botao secundario" type="button" onClick={() => setHistorico(null)}>Fechar</button>
          </div>
        </div>
      )}

      {/* tabela com todos os materiais */}
      <table className="ec-tabela">
        <thead>
          <tr>
            <th>Material</th><th>Tipo</th><th>Saldo</th><th>Mínimo</th><th>Custo unitário</th><th></th>
          </tr>
        </thead>
        <tbody>
          {materiais.length === 0 && (
            <tr><td colSpan="6" className="ec-vazio">Nenhum material cadastrado ainda.</td></tr>
          )}
          {/* uma linha para cada material com o saldo e os botões */}
          {materiais.map(m => (
            <tr key={m.id}>
              <td>{m.descricao}</td>
              <td>{m.tipo}</td>
              <td>
                {m.saldo} {m.unidade}{' '}
                {/* etiqueta vermelha se estiver abaixo do mínimo */}
                <span className={'ec-etiqueta ' + (m.abaixoDoMinimo ? 'baixo' : 'ok')}>
                  {m.abaixoDoMinimo ? 'Baixo' : 'OK'}
                </span>
              </td>
              <td>{m.estoqueMinimo} {m.unidade}</td>
              <td>{dinheiro(m.custoUnitario)}</td>
              <td className="acoes">
                <button className="ec-botao secundario pequeno"
                  onClick={() => setMov({ material: m, tipo: 'ENTRADA', quantidade: '', observacao: '' })}>
                  Movimentar
                </button>
                <button className="ec-botao secundario pequeno" onClick={() => verHistorico(m)}>Histórico</button>
                <button className="ec-botao secundario pequeno" onClick={() => setEdicao({ ...m })}>Editar</button>
                <button className="ec-botao secundario perigo pequeno" onClick={() => excluir(m)}>Excluir</button>
              </td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  )
}

// tela de estoque com duas abas materiais e compras
function Estoque() {
  const [aba, setAba] = useState('materiais')

  return (
    <div className="ec-pagina">
      <h1 className="ec-titulo">Estoque</h1>

      {/* botões das abas */}
      <div className="ec-abas">
        <button className={'ec-aba' + (aba === 'materiais' ? ' ativa' : '')} onClick={() => setAba('materiais')}>
          Materiais
        </button>
        <button className={'ec-aba' + (aba === 'compras' ? ' ativa' : '')} onClick={() => setAba('compras')}>
          Compras
        </button>
      </div>

      {/* mostra a aba escolhida */}
      {aba === 'materiais' ? <Materiais /> : <Compras embutida />}
    </div>
  )
}

export default Estoque