import React from 'react';
import { View, Text, StyleSheet } from 'react-native';

// Placeholder for AR overlay (future: integrate WebXR/React Native AR)
export default function AROverlay() {
  return (
    <View style={styles.container}>
      <Text style={styles.text}>[AR Overlay Placeholder]</Text>
      {/* TODO: Integrate WebXR/React Native AR, map POIs, overlay search results */}
    </View>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1, alignItems: 'center', justifyContent: 'center', backgroundColor: '#eef' },
  text: { fontSize: 20, color: '#333' },
}); 