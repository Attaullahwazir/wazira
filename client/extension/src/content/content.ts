// content.ts - Content script for Wazira extension
// Implements advanced anti-fingerprinting logic, respecting user settings

// Utility to spoof a property
function spoofProperty(obj: any, prop: string, value: any) {
  try {
    Object.defineProperty(obj, prop, {
      get: () => value,
      configurable: true
    });
  } catch {}
}

function applyAdvancedAntiFingerprinting(settings: any) {
  // Spoof navigator.userAgent
  if (settings.spoofUserAgent) {
    spoofProperty(navigator, 'userAgent', 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36 Wazira');
  }
  // Block access to navigator.plugins
  if (settings.spoofPlugins) {
    spoofProperty(navigator, 'plugins', []);
  }
  // Spoof WebGL vendor/renderer
  if (settings.spoofWebGL) {
    const spoofWebGL = () => {
      const getParameter = WebGLRenderingContext.prototype.getParameter;
      WebGLRenderingContext.prototype.getParameter = function(parameter) {
        if (parameter === 37445) return 'Wazira';
        if (parameter === 37446) return 'Wazira Renderer';
        return getParameter.apply(this, arguments);
      };
    };
    try { spoofWebGL(); } catch (e) {}
  }
  // Spoof Canvas fingerprinting
  if (settings.spoofCanvas) {
    const origToDataURL = HTMLCanvasElement.prototype.toDataURL;
    HTMLCanvasElement.prototype.toDataURL = function() {
      // Return a randomized string to break fingerprinting
      return 'data:image/png;base64,' + btoa(Math.random().toString());
    };
    const origGetImageData = CanvasRenderingContext2D.prototype.getImageData;
    CanvasRenderingContext2D.prototype.getImageData = function() {
      // Return random data
      const imgData = origGetImageData.apply(this, arguments);
      for (let i = 0; i < imgData.data.length; i++) {
        imgData.data[i] = (imgData.data[i] + Math.floor(Math.random() * 10)) % 256;
      }
      return imgData;
    };
  }
  // Spoof AudioContext fingerprinting
  if (settings.spoofAudio) {
    const origGetChannelData = AudioBuffer.prototype.getChannelData;
    AudioBuffer.prototype.getChannelData = function() {
      const data = origGetChannelData.apply(this, arguments);
      for (let i = 0; i < data.length; i++) {
        data[i] = data[i] + (Math.random() - 0.5) * 0.01;
      }
      return data;
    };
  }
  // Spoof timezone
  if (settings.spoofTimezone) {
    spoofProperty(Intl.DateTimeFormat.prototype, 'resolvedOptions', () => ({ timeZone: 'UTC' }));
  }
  // Spoof language
  if (settings.spoofLanguage) {
    spoofProperty(navigator, 'language', 'en-US');
    spoofProperty(navigator, 'languages', ['en-US', 'en']);
  }
  // Spoof screen properties
  if (settings.spoofScreen) {
    spoofProperty(window.screen, 'width', 1920);
    spoofProperty(window.screen, 'height', 1080);
    spoofProperty(window.screen, 'colorDepth', 24);
  }
  console.log('Wazira advanced anti-fingerprinting content script loaded.');
}

// Default settings for all protections enabled
const defaultSettings = {
  spoofUserAgent: true,
  spoofPlugins: true,
  spoofWebGL: true,
  spoofCanvas: true,
  spoofAudio: true,
  spoofTimezone: true,
  spoofLanguage: true,
  spoofScreen: true
};

// Read settings and apply protections
chrome.storage.sync.get(defaultSettings, (settings) => {
  applyAdvancedAntiFingerprinting(settings);
}); 