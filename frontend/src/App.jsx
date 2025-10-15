import './App.css'
import Calendar from './components/Calendar'

function App() {
  return (
    <div className="app" style={{ padding: 16 }}>
      <h1>Nomad Log</h1>
      <p>Трекер путешествий — стартовая версия</p>
      <Calendar />
    </div>
  )
}

export default App
