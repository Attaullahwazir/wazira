/// <reference types="chrome" />
// background.ts - Background service worker for Wazira extension
// Implements ad-blocking using declarativeNetRequest (Manifest V3), respecting user settings

const adBlockRules: chrome.declarativeNetRequest.Rule[] = [
  {
    id: 1,
    priority: 1,
    action: { type: 'block' },
    condition: {
      urlFilter: '||doubleclick.net^',
      resourceTypes: ['script', 'image', 'sub_frame']
    }
  },
  {
    id: 2,
    priority: 1,
    action: { type: 'block' },
    condition: {
      urlFilter: '||googlesyndication.com^',
      resourceTypes: ['script', 'image', 'sub_frame']
    }
  },
  {
    id: 3,
    priority: 1,
    action: { type: 'block' },
    condition: {
      urlFilter: '||adservice.google.com^',
      resourceTypes: ['script', 'image', 'sub_frame']
    }
  },
  {
    id: 4,
    priority: 1,
    action: { type: 'block' },
    condition: {
      urlFilter: '||ads.yahoo.com^',
      resourceTypes: ['script', 'image', 'sub_frame']
    }
  },
  // Add more rules as needed
];

function updateAdBlockRules(enabled: boolean) {
  if (enabled) {
    chrome.declarativeNetRequest.updateDynamicRules({
      removeRuleIds: adBlockRules.map(r => r.id),
      addRules: adBlockRules
    }, () => {
      if (chrome.runtime.lastError) {
        console.error('Failed to add ad-block rules:', chrome.runtime.lastError);
      } else {
        console.log('Wazira ad-block rules installed.');
      }
    });
  } else {
    chrome.declarativeNetRequest.updateDynamicRules({
      removeRuleIds: adBlockRules.map(r => r.id),
      addRules: []
    }, () => {
      if (chrome.runtime.lastError) {
        console.error('Failed to remove ad-block rules:', chrome.runtime.lastError);
      } else {
        console.log('Wazira ad-block rules removed.');
      }
    });
  }
}

// On install/startup, apply ad-block rules based on settings
chrome.runtime.onInstalled.addListener(() => {
  chrome.storage.sync.get({ adBlock: true }, (result) => {
    updateAdBlockRules(result.adBlock);
  });
});

// Listen for changes to adBlock setting
chrome.storage.onChanged.addListener((changes, area) => {
  if (area === 'sync' && changes.adBlock) {
    updateAdBlockRules(changes.adBlock.newValue);
  }
});

// TODO: In the future, fetch and update rules dynamically, allow user customization, etc. 