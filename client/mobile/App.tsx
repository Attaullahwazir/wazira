import React, { useState } from 'react';
import { SafeAreaView, View, Text, TextInput, Button, FlatList, Switch, StyleSheet, TouchableOpacity } from 'react-native';

const BACKEND_URL = 'http://localhost:8000/query';
const SUMMARY_URL = 'http://localhost:8000/summarize';

export default function App() {
  const [query, setQuery] = useState('');
  const [results, setResults] = useState<string[]>([]);
  const [summaries, setSummaries] = useState<{ [url: string]: string | null }>({});
  const [loading, setLoading] = useState(false);
  const [fedLearning, setFedLearning] = useState(false);

  const handleSearch = async () => {
    setLoading(true);
    setResults([]);
    setSummaries({});
    try {
      const resp = await fetch(BACKEND_URL, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ query, ciphertext: '', answer: false })
      });
      const data = await resp.json();
      setResults(data.results || []);
      // Fetch summaries
      data.results.forEach(async (url: string) => {
        if (/^https?:\/\//.test(url)) {
          const sresp = await fetch(SUMMARY_URL, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ url })
          });
          const sdata = await sresp.json();
          setSummaries(s => ({ ...s, [url]: sdata.summary }));
        }
      });
    } catch {}
    setLoading(false);
  };

  return (
    <SafeAreaView style={styles.container}>
      <Text style={styles.title}>Wazira Mobile Search</Text>
      <View style={styles.searchRow}>
        <TextInput
          style={styles.input}
          value={query}
          onChangeText={setQuery}
          placeholder="Type your search..."
        />
        <Button title="Search" onPress={handleSearch} disabled={loading || !query} />
      </View>
      <View style={styles.settingsRow}>
        <Text>Federated Learning</Text>
        <Switch value={fedLearning} onValueChange={setFedLearning} />
      </View>
      <FlatList
        data={results}
        keyExtractor={item => item}
        renderItem={({ item }) => (
          <View style={styles.resultCard}>
            <TouchableOpacity onPress={() => {}}>
              <Text style={styles.resultUrl}>{item}</Text>
            </TouchableOpacity>
            <Text style={styles.summary}>{summaries[item] || 'Loading summary...'}</Text>
          </View>
        )}
        ListEmptyComponent={!loading ? <Text style={styles.empty}>No results yet.</Text> : null}
      />
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1, padding: 16, backgroundColor: '#fff' },
  title: { fontSize: 24, fontWeight: 'bold', marginBottom: 16 },
  searchRow: { flexDirection: 'row', alignItems: 'center', marginBottom: 12 },
  input: { flex: 1, borderWidth: 1, borderColor: '#ccc', borderRadius: 6, padding: 8, marginRight: 8 },
  settingsRow: { flexDirection: 'row', alignItems: 'center', marginBottom: 16, justifyContent: 'space-between' },
  resultCard: { padding: 12, borderWidth: 1, borderColor: '#eee', borderRadius: 8, marginBottom: 10, backgroundColor: '#fafbfc' },
  resultUrl: { color: '#1976d2', fontWeight: 'bold', marginBottom: 4 },
  summary: { color: '#444', fontSize: 13 },
  empty: { color: '#888', textAlign: 'center', marginTop: 32 },
}); 