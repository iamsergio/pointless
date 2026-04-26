// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import { mount } from 'svelte';
import App from './App.svelte';
import './global.css';

mount(App, { target: document.getElementById('app') });
