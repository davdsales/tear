// Maria Gabriela
import React, { useState, useEffect } from 'react'
import '../styles/estoqueCompras.css'

const API = 'http://localhost:8080'

const MATERIAL_VAZIO = {
  tipo: 'FIO', nome: '', unidade: 'g', custoUnitario: '', estoqueMinimo: '', estoqueInicial: '',
  marca: '', cor: '', metragem: '', composicao: '', largura: '', detalhe: ''
}

const UNIDADE_PADRAO = { FIO: 'g', TECIDO: 'm', AVIAMENTO: 'un' }

function dinheiro(valor) {
  return 'R$ ' + Number(valor).toFixed(2).replace('.', ',')
}

function Estoque() {
  const [materiais, setMateriais] = useState([])
  const [resumo, setResumo] = useState({ totalMateriais: 0, abaixoDoMinimo: 0 })
  const [mensagem, setMensagem] = useState(null)
  const [mostrarNovo, setMostrarNovo] = useState(false)
  const [novo, setNovo] = useState(MATERIAL_VAZIO)
  const [mov, setMov] = useState(null)

  useEffect(() => { carregar() }, [])

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

  // manda a requisicao e mostra a mensagem que o backend devolver
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

  function mudarNovo(campo, valor) {
    const atualizado = { ...novo, [campo]: valor }
    if (campo === 'tipo') atualizado.unidade = UNIDADE_PADRAO[valor]
    setNovo(atualizado)
  }

  async function salvarNovo(e) {
    e.preventDefault()
    if (await enviar('/api/materiais', 'POST', novo, 'Material cadastrado.')) {
      setNovo(MATERIAL_VAZIO)
      setMostrarNovo(false)
    }
  }

  async function salvarMovimentacao(e) {
    e.preventDefault()
    const corpo = { idMaterial: mov.material.id, tipo: mov.tipo, quantidade: mov.quantidade, observacao: mov.observacao }
    if (await enviar('/api/estoque/movimentacoes', 'POST', corpo, 'Movimentação registrada.')) {
      setMov(null)
    }
  }

  function excluir(m) {
    if (window.confirm('Excluir "' + m.nome + '"?')) {
      enviar('/api/materiais/' + m.id, 'DELETE', null, 'Material excluído.')
    }
  }

  return (
    <div className="ec-pagina">
      <div className="ec-topo">
        <h1>Estoque</h1>
        <button className="ec-botao" onClick={() => setMostrarNovo(!mostrarNovo)}>
          {mostrarNovo ? 'Fechar' : '+ Novo material'}
        </button>
      </div>

      {mensagem && <div className={'ec-aviso' + (mensagem.erro ? '' : ' ok')}>{mensagem.texto}</div>}

      <div className="ec-resumo">
        <div className="ec-card"><span>Materiais cadastrados</span><strong>{resumo.totalMateriais}</strong></div>
        <div className="ec-card"><span>Itens com estoque baixo</span><strong>{resumo.abaixoDoMinimo}</strong></div>
      </div>

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
          {materiais.map(m => (
            <tr key={m.id}>
              <td>{m.descricao}</td>
              <td>{m.tipo}</td>
              <td>
                {m.saldo} {m.unidade}{' '}
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
                <button className="ec-botao secundario perigo pequeno" onClick={() => excluir(m)}>Excluir</button>
              </td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  )
}

export default Estoque