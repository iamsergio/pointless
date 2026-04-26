// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

import { mount } from 'svelte';
import App from './App.svelte';
import './global.css';
import { library, dom } from '@fortawesome/fontawesome-svg-core';
import {
    faEnvelope, faLock, faEye, faEyeSlash, faKey, faRotateRight,
    faFlagCheckered, faChevronLeft, faChevronRight, faPlus, faBars, faArrowsRotate
} from '@fortawesome/free-solid-svg-icons';

library.add(
    faEnvelope, faLock, faEye, faEyeSlash, faKey, faRotateRight,
    faFlagCheckered, faChevronLeft, faChevronRight, faPlus, faBars, faArrowsRotate
);
dom.watch();

mount(App, { target: document.getElementById('app') });
