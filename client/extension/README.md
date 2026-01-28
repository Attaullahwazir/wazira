# Wazira Search Extension (Client)

A privacy-first, AI-powered browser extension built with React + TypeScript.

## Features
- Popup UI for search (React)
- Background script for ad-blocking (placeholder)
- Content script for anti-fingerprinting (placeholder)
- Modern build system (Vite)

## Directory Structure
```
extension/
  manifest.json
  background.js (built)
  content.js (built)
  popup.html (built)
  icons/
  src/
    popup/
      Popup.tsx
      main.tsx
      popup.html
    background/
      background.ts
    content/
      content.ts
  package.json
  tsconfig.json
  vite.config.ts
```

## Development

1. **Install dependencies:**
   ```
   cd client/extension
   npm install
   ```
2. **Build the extension:**
   ```
   npm run build
   ```
   Built files will appear in the extension root (background.js, content.js, popup.html).

3. **Load in Chrome:**
   - Go to `chrome://extensions/`
   - Enable "Developer mode"
   - Click "Load unpacked" and select the `client/extension` directory

## Where to Add Logic
- **Ad-blocking:** `src/background/background.ts`
- **Anti-fingerprinting:** `src/content/content.ts`
- **Popup UI:** `src/popup/Popup.tsx`

## Next Steps
- Implement real ad-blocking and anti-fingerprinting logic
- Connect popup UI to backend search service
- Add advanced privacy and UX features 