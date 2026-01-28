import React, { useEffect, useState } from 'react';

const defaultSettings = {
  adBlock: true,
  antiFingerprint: true,
  federatedLearning: false,
};

type Settings = typeof defaultSettings;

const Options: React.FC = () => {
  const [settings, setSettings] = useState<Settings>(defaultSettings);
  const [status, setStatus] = useState('');
  const [lastSync, setLastSync] = useState<string | null>(null);

  useEffect(() => {
    chrome.storage.sync.get({ ...defaultSettings, lastSync: null }, (result) => {
      setSettings({
        adBlock: result.adBlock,
        antiFingerprint: result.antiFingerprint,
        federatedLearning: result.federatedLearning,
      });
      setLastSync(result.lastSync);
    });
  }, []);

  const handleChange = (key: keyof Settings) => (e: React.ChangeEvent<HTMLInputElement>) => {
    setSettings({ ...settings, [key]: e.target.checked });
  };

  const handleSave = () => {
    chrome.storage.sync.set(settings, () => {
      setStatus('Settings saved!');
      setTimeout(() => setStatus(''), 1500);
    });
  };

  const handleSync = () => {
    const now = new Date().toISOString();
    chrome.storage.sync.set({ lastSync: now }, () => {
      setLastSync(now);
      setStatus('Federated learning sync complete!');
      setTimeout(() => setStatus(''), 1500);
    });
  };

  return (
    <div style={{ maxWidth: 400, margin: '0 auto', padding: 24, fontFamily: 'sans-serif' }}>
      <h2>Wazira Extension Settings</h2>
      <label style={{ display: 'block', margin: '12px 0' }}>
        <input type="checkbox" checked={settings.adBlock} onChange={handleChange('adBlock')} /> Enable Ad-Blocking
      </label>
      <label style={{ display: 'block', margin: '12px 0' }}>
        <input type="checkbox" checked={settings.antiFingerprint} onChange={handleChange('antiFingerprint')} /> Enable Anti-Fingerprinting
      </label>
      <label style={{ display: 'block', margin: '12px 0' }}>
        <input type="checkbox" checked={settings.federatedLearning} onChange={handleChange('federatedLearning')} /> Opt-in to Federated Learning
      </label>
      {settings.federatedLearning && (
        <div style={{ margin: '12px 0', paddingLeft: 8 }}>
          <div>Last sync: {lastSync ? new Date(lastSync).toLocaleString() : 'Never'}</div>
          <button onClick={handleSync} style={{ marginTop: 8, padding: '6px 12px', fontSize: 15 }}>Sync Now</button>
        </div>
      )}
      <button onClick={handleSave} style={{ marginTop: 16, padding: '8px 16px', fontSize: 16 }}>Save</button>
      {status && <div style={{ color: 'green', marginTop: 12 }}>{status}</div>}
    </div>
  );
};

export default Options; 