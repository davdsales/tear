// Kailani
import Sidebar from "./Sidebar";

// placeholder pras telas que ainda não foram implementadas por ninguém do time.
// assim que alguém terminar a página de verdade, é só trocar o <EmConstrucao />
// pela rota correspondente no App.jsx — nada mais precisa mudar.
function EmConstrucao({ titulo }) {
  return (
    <div style={{ fontFamily: "var(--fonte-texto, sans-serif)" }}>
      <h3 style={{ fontFamily: "var(--fonte-titulo, sans-serif)", color: "#3D3229" }}>{titulo}</h3>
      <p style={{ color: "#7A6F63", marginTop: "10px" }}>Essa tela ainda está em construção.</p>
    </div>
  );
}

export default EmConstrucao;

