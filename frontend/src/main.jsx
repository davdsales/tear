import React from 'react'
import ReactDOM from 'react-dom/client'
import { usuarioLogado } from './usuario.js'
import App from './App.jsx'
import './index.css'

// sem login so da para ver as telas de cadastro e de entrar
const telaPublica = ['/', '/entrar'].includes(window.location.pathname)
if (!telaPublica && !usuarioLogado()) window.location.replace('/entrar')

ReactDOM.createRoot(document.getElementById('root')).render(
  <React.StrictMode>
    <App />
  </React.StrictMode>,
)