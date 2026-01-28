import React, { useState, useEffect } from 'react';

const BACKEND_URL = 'http://localhost:8000/query';
const SUMMARY_URL = 'http://localhost:8000/summarize';

const isUrl = (str: string) => /^https?:\/\//.test(str);

// Add DP noise to a value (Laplace mechanism, dummy for now)
function addDPNoise(value: number, epsilon = 1.0) {
  // Dummy: add uniform noise in [-1, 1]
  return value + (Math.random() * 2 - 1) / epsilon;
}

function generateSessionId() {
  return Math.random().toString(36).substring(2, 10);
}

const Popup: React.FC = () => {
  const [query, setQuery] = useState('');
  const [results, setResults] = useState<string[]>([]);
  const [summaries, setSummaries] = useState<{ [url: string]: string | null }>({});
  const [loadingSummaries, setLoadingSummaries] = useState<{ [url: string]: boolean }>({});
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [suggestions, setSuggestions] = useState<string[]>([]);
  const [history, setHistory] = useState<string[]>([]);
  const [showHistory, setShowHistory] = useState(false);
  const [sessionId] = useState(() => {
    // Persist session ID for collaborative search
    const sid = window.localStorage.getItem('wazira_session_id') || generateSessionId();
    window.localStorage.setItem('wazira_session_id', sid);
    return sid;
  });

  // Track last click for dwell time
  useEffect(() => {
    chrome.storage.local.get(['lastClick'], (data) => {
      if (data.lastClick) {
        const { url, timestamp } = data.lastClick;
        const dwell = Date.now() - timestamp;
        // Store click/dwell event
        chrome.storage.local.get(['clickEvents'], (d) => {
          const events = d.clickEvents || [];
          events.push({ url, dwell });
          chrome.storage.local.set({ clickEvents: events });
        });
        // Clear lastClick
        chrome.storage.local.remove('lastClick');
      }
    });
  }, []);

  // Generate a dummy model update with DP noise
  const generateModelUpdate = async () => {
    return new Promise((resolve) => {
      chrome.storage.local.get(['clickEvents'], (data) => {
        const events = data.clickEvents || [];
        // For demo: sum dwell times, add DP noise
        const totalDwell = events.reduce((sum: number, e: any) => sum + e.dwell, 0);
        const noisy = addDPNoise(totalDwell);
        resolve({ noisyDwell: noisy, count: events.length });
      });
    });
  };

  // Upload model update to server (placeholder)
  const uploadModelUpdate = async () => {
    const update = await generateModelUpdate();
    // TODO: POST to federated learning endpoint with token auth
    alert('Model update (dummy): ' + JSON.stringify(update));
  };

  // Open the extension options page
  const openSettings = () => {
    if (chrome && chrome.runtime && chrome.runtime.openOptionsPage) {
      chrome.runtime.openOptionsPage();
    } else {
      window.open('options.html');
    }
  };

  // Load query history from storage
  useEffect(() => {
    chrome.storage.local.get(['queryHistory'], (data) => {
      setHistory(data.queryHistory || []);
    });
  }, []);

  // Update suggestions as user types
  useEffect(() => {
    if (!query) {
      setSuggestions([]);
      return;
    }
    setSuggestions(history.filter(q => q.toLowerCase().includes(query.toLowerCase()) && q !== query).slice(0, 5));
  }, [query, history]);

  // Save query to history
  const saveQueryToHistory = (q: string) => {
    chrome.storage.local.get(['queryHistory'], (data) => {
      let hist = data.queryHistory || [];
      if (!hist.includes(q)) {
        hist = [q, ...hist].slice(0, 20); // Keep last 20
        chrome.storage.local.set({ queryHistory: hist });
        setHistory(hist);
      }
    });
  };

  const handleSearch = async (e: React.FormEvent) => {
    e.preventDefault();
    setLoading(true);
    setError(null);
    setResults([]);
    setSummaries({});
    saveQueryToHistory(query);
    try {
      const resp = await fetch(BACKEND_URL, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ query, ciphertext: '', answer: false })
      });
      const data = await resp.json();
      setResults(data.results || []);
    } catch (err: any) {
      setError(err.message || 'Search failed');
    } finally {
      setLoading(false);
    }
  };

  // Handle click on a result link
  const handleResultClick = (url: string) => {
    chrome.storage.local.set({ lastClick: { url, timestamp: Date.now() } });
  };

  // Fetch summary for a result
  const fetchSummary = async (url: string) => {
    setLoadingSummaries(ls => ({ ...ls, [url]: true }));
    try {
      const resp = await fetch(SUMMARY_URL, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ url })
      });
      const data = await resp.json();
      setSummaries(s => ({ ...s, [url]: data.summary }));
    } catch {
      setSummaries(s => ({ ...s, [url]: '[Summary unavailable]' }));
    } finally {
      setLoadingSummaries(ls => ({ ...ls, [url]: false }));
    }
  };

  // Fetch summaries for all results when results change
  useEffect(() => {
    if (results.length > 0) {
      results.forEach(url => {
        if (isUrl(url) && !summaries[url]) fetchSummary(url);
      });
    }
    // eslint-disable-next-line
  }, [results]);

  // Action: Share (copy link)
  const handleShare = (url: string) => {
    navigator.clipboard.writeText(url);
    alert('Link copied!');
  };

  return (
    <div style={{ width: 340, padding: 16, fontFamily: 'sans-serif', position: 'relative' }}>
      {/* Collaborative session ID */}
      <div style={{ position: 'absolute', top: 8, left: 16, fontSize: 11, color: '#888' }}>
        Session: {sessionId}
      </div>
      {/* Settings button */}
      <button
        onClick={openSettings}
        style={{ position: 'absolute', top: 12, right: 12, background: 'none', border: 'none', cursor: 'pointer' }}
        title="Settings"
        aria-label="Settings"
      >
        <span role="img" aria-label="settings" style={{ fontSize: 22 }}>⚙️</span>
      </button>
      <h2 style={{ margin: 0, paddingRight: 32 }}>Wazira Search</h2>
      <form onSubmit={handleSearch} autoComplete="off">
        <input
          type="text"
          value={query}
          onChange={e => setQuery(e.target.value)}
          placeholder="Type your search..."
          style={{ width: '100%', margin: '12px 0', padding: 8, fontSize: 16 }}
          autoComplete="off"
        />
        {/* Suggestions dropdown */}
        {suggestions.length > 0 && (
          <ul style={{ background: '#fff', border: '1px solid #eee', borderRadius: 4, margin: 0, padding: 4, position: 'absolute', zIndex: 10, width: 'calc(100% - 32px)' }}>
            {suggestions.map((s, i) => (
              <li key={i} style={{ cursor: 'pointer', padding: 4 }} onClick={() => setQuery(s)}>{s}</li>
            ))}
          </ul>
        )}
        <button type="submit" style={{ width: '100%', padding: 8, fontSize: 16 }} disabled={loading || !query}>
          {loading ? 'Searching...' : 'Search'}
        </button>
      </form>
      <button onClick={() => setShowHistory(h => !h)} style={{ margin: '8px 0', width: '100%', fontSize: 13 }}>
        {showHistory ? 'Hide History' : 'Show History'}
      </button>
      {showHistory && (
        <ul style={{ background: '#f9f9f9', border: '1px solid #eee', borderRadius: 4, margin: 0, padding: 8, maxHeight: 120, overflowY: 'auto' }}>
          {history.map((h, i) => (
            <li key={i} style={{ cursor: 'pointer', padding: 4 }} onClick={() => setQuery(h)}>{h}</li>
          ))}
        </ul>
      )}
      {error && <div style={{ color: 'red', marginTop: 8 }}>{error}</div>}
      <div style={{ color: '#888', fontSize: 14, marginTop: 12 }}>
        {loading ? (
          <div style={{ color: '#333', fontWeight: 500 }}>Loading results...</div>
        ) : results.length > 0 ? (
          <ul style={{ paddingLeft: 16 }}>
            {results.map((r, i) => (
              <li key={i} style={{ marginBottom: 12 }}>
                {isUrl(r) ? (
                  <div>
                    <a
                      href={r}
                      target="_blank"
                      rel="noopener noreferrer"
                      style={{ color: '#1976d2', textDecoration: 'underline', wordBreak: 'break-all' }}
                      onClick={() => handleResultClick(r)}
                    >
                      {r}
                    </a>
                    {/* Action cards */}
                    <div style={{ marginTop: 4, display: 'flex', gap: 8 }}>
                      <button onClick={() => window.open(r, '_blank')} title="Open" style={{ fontSize: 13 }}>Open</button>
                      <button onClick={() => fetchSummary(r)} title="Summarize" style={{ fontSize: 13 }} disabled={loadingSummaries[r]}>Summarize</button>
                      <button onClick={() => handleShare(r)} title="Share" style={{ fontSize: 13 }}>Share</button>
                    </div>
                    {/* Summary */}
                    <div style={{ marginTop: 4, color: '#444', fontSize: 13 }}>
                      {loadingSummaries[r] ? <em>Loading summary...</em> : summaries[r] || <em>No summary yet.</em>}
                    </div>
                  </div>
                ) : (
                  <span>{r}</span>
                )}
              </li>
            ))}
          </ul>
        ) : !loading && <em>AI-powered results will appear here.</em>}
      </div>
      {/* Federated learning upload button for demo */}
      <button onClick={uploadModelUpdate} style={{ marginTop: 16, width: '100%', padding: 8, fontSize: 15 }}>
        Upload Model Update (Demo)
      </button>
    </div>
  );
};

export default Popup; 