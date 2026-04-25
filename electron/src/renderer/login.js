// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

'use strict';

const emailInput     = document.getElementById('email');
const passwordInput  = document.getElementById('password');
const loginBtn       = document.getElementById('login-btn');
const eyeToggle      = document.getElementById('eye-toggle');
const eyeIcon        = document.getElementById('eye-icon');
const spinnerWrapper = document.getElementById('spinner-wrapper');
const errorMessage   = document.getElementById('error-message');
const offlineLink    = document.getElementById('offline-link');

function setLoading(isLoading) {
  loginBtn.disabled = isLoading;
  spinnerWrapper.hidden = !isLoading;
}

function showError(message) {
  errorMessage.textContent = message;
  errorMessage.hidden = false;
}

function clearError() {
  errorMessage.textContent = '';
  errorMessage.hidden = true;
}

async function attemptLogin() {
  clearError();

  const email    = emailInput.value.trim();
  const password = passwordInput.value;

  if (!email || !password) {
    showError('Please enter both email and password.');
    return;
  }

  setLoading(true);
  try {
    const result = await window.pointlessAPI.login(email, password);
    if (result.success) {
      console.log('Login successful, userId:', result.data.userId);
    } else {
      showError(result.error || 'Login failed. Please try again.');
    }
  } finally {
    setLoading(false);
  }
}

loginBtn.addEventListener('click', attemptLogin);

emailInput.addEventListener('keydown', (e) => {
  if (e.key === 'Enter') passwordInput.focus();
});

passwordInput.addEventListener('keydown', (e) => {
  if (e.key === 'Enter') attemptLogin();
});

eyeToggle.addEventListener('click', () => {
  const isVisible = passwordInput.type === 'text';
  passwordInput.type = isVisible ? 'password' : 'text';
  eyeIcon.className = isVisible ? 'fa-solid fa-eye-slash' : 'fa-solid fa-eye';
});

offlineLink.addEventListener('click', () => {
  console.log('Offline mode selected');
});
