import { useState, useEffect } from 'react'
import '../styles/Inicio.css'
import { usuarioLogado, primeiroNome } from '../usuario.js'
import imgReceita from '../assets/img_receita.png'
import imgReceber from '../assets/img_receber.png'
import imgAndamento from '../assets/img_andamento.png'
import imgEstoque from '../assets/img_estoque.png'

// endereço do servidor em C++
const API = 'http://localhost:8080'

// data de hoje no formato do banco tipo 2026-10-06
function hoje() {
  return new Date().toLocaleDateString('en-CA')
}

// formulário em branco de transação nova
const TRANSACAO_VAZIA = { tipo: 'Receita', descricao: '', valor: '', data: hoje(), origem: 'Encomenda', categoria: 'Materiais' }

// mostra o número como dinheiro tipo R$ 12,50
function dinheiro(valor) {
  return Number(valor).toLocaleString('pt-BR', { style: 'currency', currency: 'BRL' })
}

// troca 2026-10-06 por 06/10/2026
function dataBR(data) {
  return data.split('-').reverse().join('/')
}

// tela inicial com os cards o financeiro e as últimas transações
function Inicio() {
  // guardam as informações da tela
  const usuario = usuarioLogado()
  const [dados, setDados] = useState(null)
  const [resumo, setResumo] = useState(null)
  const [transacoes, setTransacoes] = useState([])
  const [mensagem, setMensagem] = useState(null)
  const [mostrarForm, setMostrarForm] = useState(false)
  const [nova, setNova] = useState(TRANSACAO_VAZIA)
  const [editandoId, setEditandoId] = useState(null)

  // busca os dados assim que a tela abre
  useEffect(() => { carregar() }, [])

  // pede ao servidor os cards o resumo e as transações ao mesmo tempo
  async function carregar() {
    try {
      const [resDados, resResumo, resTransacoes] = await Promise.all([
        fetch(API + '/api/dashboard'),
        fetch(API + '/api/financeiro/resumo'),
        fetch(API + '/api/financeiro/transacoes')
      ])
      setDados(await resDados.json())
      setResumo(await resResumo.json())
      setTransacoes(await resTransacoes.json())
    } catch {
      setMensagem({ erro: true, texto: 'Não consegui falar com o servidor C++. Ele está rodando?' })
    }
  }

  // salva a transação nova ou a editada
  async function salvarTransacao(e) {
    e.preventDefault()
    const corpo = {
      tipo: nova.tipo,
      descricao: nova.descricao,
      // troca vírgula por ponto para o servidor entender o número
      valor: String(nova.valor).replace(',', '.'),
      data: nova.data
    }
    // receita leva a origem e despesa leva a categoria
    if (nova.tipo === 'Receita') corpo.origem = nova.origem
    else corpo.categoria = nova.categoria
    // se estiver editando manda o id junto e usa PUT senão usa POST
    const url = API + '/api/financeiro/transacoes' + (editandoId ? '/' + editandoId : '')
    try {
      const res = await fetch(url, {
        method: editandoId ? 'PUT' : 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(corpo)
      })
      const resposta = await res.json()
      if (!res.ok) return setMensagem({ erro: true, texto: resposta.mensagem })
      setMensagem({ erro: false, texto: editandoId ? 'Transação atualizada.' : 'Transação registrada.' })
      fecharForm()
      carregar()
    } catch {
      setMensagem({ erro: true, texto: 'Não consegui falar com o servidor C++. Ele está rodando?' })
    }
  }

  // abre o formulário vazio
  function abrirNova() {
    setNova(TRANSACAO_VAZIA)
    setEditandoId(null)
    setMostrarForm(true)
  }

  // abre o formulário já preenchido com a transação escolhida
  function editar(t) {
    setNova({
      tipo: t.tipo,
      descricao: t.descricao,
      valor: String(t.valor).replace('.', ','),
      data: t.data,
      origem: t.origem || 'Encomenda',
      categoria: t.categoria || 'Materiais'
    })
    setEditandoId(t.id)
    setMostrarForm(true)
  }

  // fecha e limpa o formulário
  function fecharForm() {
    setNova(TRANSACAO_VAZIA)
    setEditandoId(null)
    setMostrarForm(false)
  }

  // pergunta antes de apagar a transação
  async function excluir(t) {
    if (!window.confirm(`Excluir "${t.descricao}"?`)) return
    try {
      const res = await fetch(API + '/api/financeiro/transacoes/' + t.id, { method: 'DELETE' })
      const resposta = await res.json()
      if (!res.ok) return setMensagem({ erro: true, texto: resposta.mensagem })
      setMensagem({ erro: false, texto: 'Transação excluída.' })
      if (editandoId === t.id) fecharForm()
      carregar()
    } catch {
      setMensagem({ erro: true, texto: 'Não consegui falar com o servidor C++. Ele está rodando?' })
    }
  }

  // maior valor dos 6 meses para o tamanho das barras do gráfico
  const maiorMes = resumo ? Math.max(1, ...resumo.ultimosMeses.flatMap(m => [m.receitas, m.despesas])) : 1

  // o que aparece na tela
  return (
    <div className="inicio-conteudo">
      {/* mostra só o primeiro nome de quem entrou */}
      <h1>Olá, {primeiroNome(usuario?.nome)} 👋</h1>
      <p>Veja como está seu trabalho hoje.</p>

      {/* aviso de sucesso ou de erro */}
      {mensagem && <div className={'in-faixa ' + (mensagem.erro ? 'erro' : 'ok')}>{mensagem.texto}</div>}

      {/* os 4 cards do topo */}
      <div className="indicadores">
        <div className="box_info">
          <span>Receita este mês</span>
          <h3>{dados ? dinheiro(dados.receitaMes) : '—'}</h3>
          <img src={imgReceita} alt="Cálculo Receita mensal"/>
        </div>

        <div className="box_info">
          <span>A receber</span>
          <h3>{dados ? dinheiro(dados.aReceber) : '—'}</h3>
          <img src={imgReceber} alt="A receber"/>
        </div>

        <div className="box_info">
          <span>Pedidos em andamento</span>
          <h3>{dados ? dados.pedidosEmAndamento : '—'}</h3>
          <img src={imgAndamento} alt="Pedidos em andamento"/>
        </div>

        <div className="box_info">
          <span>Itens com estoque baixo</span>
          <h3>{dados ? dados.estoqueBaixo : '—'}</h3>
          <img src={imgEstoque} alt="Itens com estoque baixo"/>
        </div>
      </div>

      {/* aviso dos materiais com estoque baixo */}
      {dados && dados.itensAbaixoDoMinimo.length > 0 && (
        <div className="in-faixa erro">
          {dados.itensAbaixoDoMinimo.map(m => (
            <p key={m.nome}>
              ⚠ {m.nome} está abaixo do mínimo: restam {m.saldo} {m.unidade} (mínimo {m.estoqueMinimo}).
            </p>
          ))}
        </div>
      )}

      <div className="in-topo">
        <h4>Financeiro</h4>
        <button className="in-botao" onClick={abrirNova}>+ Nova transação</button>
      </div>

      {/* formulário de transação que só aparece quando está aberto */}
      {mostrarForm && (
        <form className="in-card in-form" onSubmit={salvarTransacao}>
          <h5>{editandoId ? 'Editar transação' : 'Nova transação'}</h5>
          <div className="in-grade">
            <label className="in-campo">Tipo
              {/* na edição o tipo fica travado */}
              <select value={nova.tipo} disabled={editandoId !== null} onChange={e => setNova({ ...nova, tipo: e.target.value })}>
                <option>Receita</option>
                <option>Despesa</option>
              </select>
            </label>
            <label className="in-campo">Descrição
              <input value={nova.descricao} onChange={e => setNova({ ...nova, descricao: e.target.value })} />
            </label>
            <label className="in-campo">Valor (R$)
              <input value={nova.valor} onChange={e => setNova({ ...nova, valor: e.target.value })} placeholder="0,00" />
            </label>
            <label className="in-campo">Data
              <input type="date" value={nova.data} onChange={e => setNova({ ...nova, data: e.target.value })} />
            </label>
            {/* receita mostra origem e despesa mostra categoria */}
            {nova.tipo === 'Receita' ? (
              <label className="in-campo">Origem
                <select value={nova.origem} onChange={e => setNova({ ...nova, origem: e.target.value })}>
                  <option>Encomenda</option>
                  <option>Venda</option>
                  <option>Feira</option>
                  <option>Outros</option>
                </select>
              </label>
            ) : (
              <label className="in-campo">Categoria
                <select value={nova.categoria} onChange={e => setNova({ ...nova, categoria: e.target.value })}>
                  <option>Materiais</option>
                  <option>Embalagem</option>
                  <option>Transporte</option>
                  <option>Outros</option>
                </select>
              </label>
            )}
          </div>
          <div className="in-botoes">
            <button className="in-botao" type="submit">Salvar</button>
            <button className="in-botao secundario" type="button" onClick={fecharForm}>Cancelar</button>
          </div>
        </form>
      )}

      {/* cards de receitas despesas e lucro do mês */}
      <div className="in-mini">
        <div className="in-card">
          <span>Receitas do mês</span>
          <strong>{resumo ? dinheiro(resumo.receitas) : '—'}</strong>
        </div>
        <div className="in-card">
          <span>Despesas do mês</span>
          <strong>{resumo ? dinheiro(resumo.despesas) : '—'}</strong>
        </div>
        <div className="in-card">
          <span>Lucro do mês</span>
          {/* lucro fica vermelho se for negativo */}
          <strong className={resumo && resumo.lucro < 0 ? 'negativo' : 'positivo'}>
            {resumo ? dinheiro(resumo.lucro) : '—'}
          </strong>
        </div>
      </div>

      <div className="in-duas-colunas">
        <div className="in-card">
          {/* gráfico de barras dos últimos 6 meses */}
          <h5>Receitas x Despesas · últimos 6 meses</h5>
          <div className="in-grafico">
            {resumo && resumo.ultimosMeses.map(m => (
              <div key={m.rotulo} className="in-grafico-mes">
                <div className="in-grafico-barras">
                  {/* a altura da barra é proporcional ao maior valor */}
                  <div className="in-barra receita" style={{ height: (m.receitas / maiorMes) * 100 + '%' }} />
                  <div className="in-barra despesa" style={{ height: (m.despesas / maiorMes) * 100 + '%' }} />
                </div>
                <span>{m.rotulo}</span>
              </div>
            ))}
          </div>
          <div className="in-legenda">
            <span><i className="receita" /> Receita</span>
            <span><i className="despesa" /> Despesa</span>
          </div>
        </div>

        <div className="in-card">
          {/* porcentagem de cada origem da receita */}
          <h5>De onde vem sua receita?</h5>
          {resumo && resumo.porOrigem.map(o => (
            <div key={o.origem} className="in-origem">
              <div className="in-origem-linha">
                <span>{o.origem}</span>
                <span>{Math.round(o.percentual)}%</span>
              </div>
              <div className="in-trilho"><div style={{ width: o.percentual + '%' }} /></div>
            </div>
          ))}
        </div>
      </div>

      <div className="in-card">
        {/* tabela com as 8 transações mais recentes */}
        <h5>Últimas transações</h5>
        <table className="in-tabela">
          <thead>
            <tr><th>Data</th><th>Descrição</th><th>Tipo</th><th>Origem/Categoria</th><th>Valor</th><th></th></tr>
          </thead>
          <tbody>
            {transacoes.length === 0 && (
              <tr><td colSpan="6" className="in-vazio">Nenhuma transação registrada ainda.</td></tr>
            )}
            {transacoes.slice(0, 8).map(t => (
              <tr key={t.id}>
                <td>{dataBR(t.data)}</td>
                <td>{t.descricao}</td>
                <td><span className={'in-etiqueta ' + (t.tipo === 'Receita' ? 'ok' : 'baixo')}>{t.tipo}</span></td>
                <td>{t.origem || t.categoria}</td>
                <td className={t.tipo === 'Despesa' ? 'negativo' : ''}>
                  {t.tipo === 'Despesa' ? '− ' : ''}{dinheiro(t.valor)}
                </td>
                <td className="in-acoes">
                  <button className="in-botao secundario pequeno" onClick={() => editar(t)}>Editar</button>
                  <button className="in-botao secundario pequeno perigo" onClick={() => excluir(t)}>Excluir</button>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  )
}

export default Inicio