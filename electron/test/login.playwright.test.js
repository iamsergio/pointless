// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

'use strict';

const { test, expect, _electron: electron } = require('@playwright/test');
const path = require('node:path');

test('login with debug credentials succeeds', async () => {
  const email    = process.env.POINTLESS_DEBUG_USERNAME;
  const password = process.env.POINTLESS_DEBUG_PASSWORD;

  if (!email)    throw new Error('POINTLESS_DEBUG_USERNAME env var must be set');
  if (!password) throw new Error('POINTLESS_DEBUG_PASSWORD env var must be set');

  const app  = await electron.launch({ args: [path.join(__dirname, '..')] });
  const page = await app.firstWindow();

  try {
    const loginSucceeded = page.waitForEvent('console', {
      predicate: msg => msg.text().includes('Login successful'),
      timeout:   10_000,
    });

    await page.locator('#email').fill(email);
    await page.locator('#password').fill(password);
    await page.locator('#login-btn').click();

    await loginSucceeded;

    await expect(page.locator('#error-message')).toBeHidden();
  } finally {
    await app.close();
  }
});
