const CHAVE_LOGADO = 'tear_usuario'  
const CHAVE_LISTA = 'tear_usuarios'   

function ler(chave, padrao) {
  try {
    const texto = localStorage.getItem(chave)
    return texto ? JSON.parse(texto) : padrao
  } catch {
    return padrao
  }
}

function guardar(chave, valor) {
  try {
    localStorage.setItem(chave, JSON.stringify(valor))
  } catch {
    
  }
}


export function salvarCadastro(nome, email) {
  const usuario = { nome: nome.trim(), email: email.trim().toLowerCase() }
  const lista = ler(CHAVE_LISTA, []).filter((u) => u.email !== usuario.email)
  lista.push(usuario)
  guardar(CHAVE_LISTA, lista)
  guardar(CHAVE_LOGADO, usuario)
  return usuario
}

export function entrarComEmail(email) {
  const emailLimpo = email.trim().toLowerCase()
  const encontrado = ler(CHAVE_LISTA, []).find((u) => u.email === emailLimpo)
 
  const usuario = encontrado || { nome: emailLimpo.split('@')[0], email: emailLimpo }
  guardar(CHAVE_LOGADO, usuario)
  return usuario
}

export function usuarioLogado() {
  return ler(CHAVE_LOGADO, null)
}

export function sair() {
  try {
    localStorage.removeItem(CHAVE_LOGADO)
  } catch {

  }
}


export function primeiroNome(nome) {
  return nome ? nome.trim().split(' ')[0] : ''
}

export function iniciais(nome) {
  if (!nome) return '?'
  const partes = nome.trim().split(' ').filter(Boolean)
  const primeira = partes[0][0]
  const ultima = partes.length > 1 ? partes[partes.length - 1][0] : ''
  return (primeira + ultima).toUpperCase()
}