// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

'use strict';

const fs = require('node:fs');
const path = require('node:path');

const SCREENSHOT_DIR = path.join(__dirname, '..', 'test-results', 'screenshots');

async function captureScreenshot(page, name) {
  fs.mkdirSync(SCREENSHOT_DIR, { recursive: true });
  await page.waitForLoadState('networkidle');
  const timestamp = Date.now();
  await page.screenshot({
    path: path.join(SCREENSHOT_DIR, `${name}-${timestamp}.png`),
  });
}

module.exports = { captureScreenshot };
