// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

'use strict';

const fs = require('node:fs');
const path = require('node:path');

const SCREENSHOT_DIR = path.join(__dirname, '..', 'test-results', 'screenshots');

// Use Electron's capturePage() instead of Playwright's page.screenshot() to avoid
// being clipped by the physical screen boundary on HiDPI / multi-monitor setups.
async function captureScreenshot(page, name, app) {
  fs.mkdirSync(SCREENSHOT_DIR, { recursive: true });
  await page.waitForLoadState('networkidle');
  await page.evaluate(() => document.fonts.ready);

  const timestamp = Date.now();
  const filePath = path.join(SCREENSHOT_DIR, `${name}-${timestamp}.png`);

  const dataUrl = await app.evaluate(({ BrowserWindow }) => {
    return new Promise(resolve => {
      BrowserWindow.getAllWindows()[0].webContents.capturePage().then(img => {
        resolve(img.toDataURL());
      });
    });
  });

  const base64 = dataUrl.replace(/^data:image\/png;base64,/, '');
  fs.writeFileSync(filePath, Buffer.from(base64, 'base64'));
}

module.exports = { captureScreenshot };
