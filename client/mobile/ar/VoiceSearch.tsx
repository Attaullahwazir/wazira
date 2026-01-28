import React from 'react';
import { View, Text, StyleSheet } from 'react-native';

// Placeholder for voice search (future: integrate Whisper-tiny WASM)
export default function VoiceSearch() {
  return (
    <View style={styles.container}>
      <Text style={styles.text}>[Voice Search Placeholder]</Text>
      {/* TODO: Integrate Whisper-tiny WASM for on-device voice-to-text search */}
    </View>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1, alignItems: 'center', justifyContent: 'center', backgroundColor: '#efe' },
  text: { fontSize: 20, color: '#333' },
}); 