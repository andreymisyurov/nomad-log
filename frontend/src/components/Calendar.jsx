import { useMemo, useState, useEffect } from 'react'
import './Calendar.css'

const LS_KEY = 'nomadLog.labels.v1'

function startOfDay(d) {
  const x = new Date(d)
  x.setHours(0, 0, 0, 0)
  return x
}
function addDays(d, n) {
  const x = new Date(d)
  x.setDate(x.getDate() + n)
  return x
}
function fmtISO(d) {
  return d.toISOString().split('T')[0]
}
function sameDay(a, b) {
  return a && b && a.getFullYear() === b.getFullYear() && a.getMonth() === b.getMonth() && a.getDate() === b.getDate()
}
function monthGrid(date) {
  const y = date.getFullYear()
  const m = date.getMonth()
  const first = new Date(y, m, 1)
  const shift = (first.getDay() + 6) % 7
  const start = addDays(first, -shift)
  return Array.from({ length: 42 }, (_, i) => addDays(start, i))
}

export default function Calendar() {
  const [viewDate, setViewDate] = useState(startOfDay(new Date()))
  const [currentCode, setCurrentCode] = useState('')
  const [labels, setLabels] = useState({})

  useEffect(() => {
    fetch('http://localhost:8080/api/labels')
      .then(res => res.json())
      .then(data => setLabels(data))
      .catch(err => console.error('Failed to load labels:', err))
  }, [])
  
  useEffect(() => {
    if (Object.keys(labels).length === 0) return;
    
    fetch('http://localhost:8080/api/labels', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(labels)
    }).catch(err => console.error('Failed to save labels:', err))
  }, [labels])

  const grid = useMemo(() => monthGrid(viewDate), [viewDate])
  const today = startOfDay(new Date())
  const monthLabel = new Intl.DateTimeFormat('ru-RU', { month: 'long', year: 'numeric' }).format(viewDate)
  const weekdays = ['Пн', 'Вт', 'Ср', 'Чт', 'Пт', 'Сб', 'Вс']

  const goPrev = () => setViewDate(new Date(viewDate.getFullYear(), viewDate.getMonth() - 1, 1))
  const goNext = () => setViewDate(new Date(viewDate.getFullYear(), viewDate.getMonth() + 1, 1))
  const goToday = () => setViewDate(startOfDay(new Date()))

  function handleDayClick(d) {
    if (currentCode !== '') {
      const key = fmtISO(d)
      setLabels(prev => ({ ...prev, [key]: String(Number(currentCode)) }))
    }
  }

  return (
    <div className="calendar">
      <div className="calendar__header">
        <button className="calendar__nav" onClick={goPrev}>{'<'}</button>
        <div className="calendar__title">{monthLabel.charAt(0).toUpperCase() + monthLabel.slice(1)}</div>
        <button className="calendar__nav" onClick={goNext}>{'>'}</button>
      </div>

      <div className="calendar__toolbar">
        <button className="calendar__today" onClick={goToday}>Сегодня</button>
        <div className="calendar__code">
          <label>
            Активная метка:
            <input
              type="number"
              min={0}
              max={999}
              value={currentCode}
              onChange={e => setCurrentCode(e.target.value)}
              placeholder="61"
            />
          </label>
        </div>
      </div>

      <div className="calendar__weekdays">
        {weekdays.map(w => <div key={w} className="calendar__weekday">{w}</div>)}
      </div>

      <div className="calendar__grid">
        {grid.map(d => {
          const outside = d.getMonth() !== viewDate.getMonth()
          const isToday = sameDay(d, today)
          const code = labels[fmtISO(d)]
          const cls = [
            'calendar__day',
            outside && 'calendar__day--outside',
            isToday && 'calendar__day--today',
            code && 'calendar__day--labeled'
          ].filter(Boolean).join(' ')

          return (
            <button
              key={d.toISOString()}
              className={cls}
              onClick={() => handleDayClick(d)}
              title={code ? `Метка: ${code}` : ''}
            >
              <div className="calendar__num">{d.getDate()}</div>
              {code && <div className="calendar__day-label">{code}</div>}
            </button>
          )
        })}
      </div>
    </div>
  )
}
