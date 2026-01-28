# Wazira Mobile Client

A privacy-first, AI-powered mobile search app built with React Native.

## Features
- Search bar and results list (with AI summaries)
- Settings page (privacy, federated learning opt-in)
- Fast, modern UI
- (Planned) AR overlays and voice search

## Setup
1. Install dependencies:
   ```
   cd client/mobile
   npm install
   # or
   yarn install
   ```
2. Run on Android:
   ```
   npx react-native run-android
   ```
   Or on iOS:
   ```
   npx react-native run-ios
   ```

## Development
- Edit `App.tsx` for main UI.
- See `SearchEnginePlan.txt` for planned features.

## Future Plans
- Integrate AR overlays (WebXR/React Native AR)
- Add voice search (Whisper-tiny WASM)
- Sync settings with browser extension 

## AR & Voice Search
- `ar/AROverlay.tsx`: Placeholder for AR overlays (future: WebXR/React Native AR, POI mapping, result overlays)
- `ar/VoiceSearch.tsx`: Placeholder for voice search (future: Whisper-tiny WASM, on-device voice-to-text)

To use: import and render `<AROverlay />` or `<VoiceSearch />` in your app.

Planned libraries:
- WebXR (for web AR)
- React Native AR (for mobile AR overlays)
- Whisper-tiny WASM (for privacy-preserving voice search) 