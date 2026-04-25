// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

'use strict';

const { app, BrowserWindow, ipcMain } = require('electron');
const path = require('node:path');
const { login } = require('./src/auth');

function createWindow() {
  const win = new BrowserWindow({
    width: 440,
    height: 620,
    resizable: false,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      nodeIntegration: false,
      sandbox: true,
    },
  });

  win.loadFile(path.join(__dirname, 'src', 'renderer', 'login.html'));
}

ipcMain.handle('auth:login', async (_event, { email, password }) => {
  const supabaseUrl = process.env.POINTLESS_SUPABASE_URL;
  const anonKey = process.env.POINTLESS_SUPABASE_ANON_KEY;

  if (!supabaseUrl || !anonKey) {
    return { success: false, error: 'Missing POINTLESS_SUPABASE_URL or POINTLESS_SUPABASE_ANON_KEY' };
  }

  try {
    const data = await login({ email, password, supabaseUrl, anonKey });
    return { success: true, data };
  } catch (err) {
    return { success: false, error: err.message };
  }
});

app.whenReady().then(createWindow);

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit();
});

app.on('activate', () => {
  if (BrowserWindow.getAllWindows().length === 0) createWindow();
});
