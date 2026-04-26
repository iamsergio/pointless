// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

'use strict';

const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('pointlessAPI', {
  login: (email, password) =>
    ipcRenderer.invoke('auth:login', { email, password }),
  getCredentials: () =>
    ipcRenderer.invoke('env:credentials'),
  loadLocalData: () =>
    ipcRenderer.invoke('data:loadLocal'),
});
