// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

'use strict';

const { defineConfig } = require('@playwright/test');

module.exports = defineConfig({
  testDir: 'test',
  testMatch: '*.playwright.test.js',
  timeout: 30_000,
});
