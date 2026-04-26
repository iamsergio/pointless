<!-- SPDX-FileCopyrightText: 2025 Sergio Martins -->
<!-- SPDX-License-Identifier: MIT -->

<script>
  import MainScreen from './MainScreen.svelte';

  let currentScreen = $state('login');

  let email = $state('');
  let password = $state('');

  $effect(() => {
    window.pointlessAPI.getCredentials().then(creds => {
      if (creds.email) email = creds.email;
      if (creds.password) password = creds.password;
    });
  });
  let passwordVisible = $state(false);
  let loading = $state(false);
  let errorMessage = $state('');
  let passwordInput = $state(null);

  function togglePassword() {
    passwordVisible = !passwordVisible;
  }

  async function attemptLogin() {
    errorMessage = '';

    if (!email.trim() || !password) {
      errorMessage = 'Please enter both email and password';
      return;
    }

    loading = true;
    try {
      const result = await window.pointlessAPI.login(email.trim(), password);
      if (result.success) {
        console.log('Login successful, userId:', result.data.userId);
        currentScreen = 'main';
      } else {
        errorMessage = result.error || 'Login failed. Please try again.';
      }
    } finally {
      loading = false;
    }
  }
</script>

{#if currentScreen === 'login'}
  <div class="login-wrapper">
    <div class="login-panel">
      <h1 class="app-title">Pointless</h1>

      <div class="fields-section">
        <div class="field-group">
          <label class="field-label" for="email">Email</label>
          <div class="input-wrapper">
            <i class="fa-solid fa-envelope input-icon"></i>
            <input
              id="email"
              type="email"
              placeholder="Enter your email"
              autocomplete="email"
              bind:value={email}
              onkeydown={(e) => { if (e.key === 'Enter') passwordInput.focus(); }}
            >
          </div>
        </div>

        <div class="field-group">
          <div class="label-row">
            <label class="field-label" for="password">Password</label>
            <a class="forgot-link" href="#">Forgot Password?</a>
          </div>
          <div class="input-wrapper">
            <i class="fa-solid fa-lock input-icon"></i>
            <input
              id="password"
              type={passwordVisible ? 'text' : 'password'}
              placeholder="Enter your password"
              autocomplete="current-password"
              bind:value={password}
              bind:this={passwordInput}
              onkeydown={(e) => { if (e.key === 'Enter') attemptLogin(); }}
            >
            <button class="eye-toggle" type="button" aria-label="Toggle password visibility" onclick={togglePassword}>
              <i class="fa-solid {passwordVisible ? 'fa-eye' : 'fa-eye-slash'}"></i>
            </button>
          </div>
        </div>
      </div>

      <div class="button-row">
        <button id="login-btn" class="login-btn" type="button" disabled={loading} onclick={attemptLogin}>Login</button>
        <button class="key-btn" type="button" aria-label="Use stored credentials">
          <i class="fa-solid fa-key"></i>
        </button>
      </div>

      {#if loading}
        <div class="spinner-wrapper">
          <i class="fa-solid fa-rotate-right spinner"></i>
        </div>
      {/if}

      {#if errorMessage}
        <p id="error-message" class="error-message">{errorMessage}</p>
      {/if}

      <p class="signup-row">
        Don't have an account? <a href="#">Sign Up</a>
      </p>

      <p class="offline-link" onclick={() => console.log('Offline mode selected')}>Offline mode</p>
    </div>
  </div>
{:else}
  <MainScreen />
{/if}

<style>
  .login-wrapper {
    flex: 1;
    display: flex;
    align-items: center;
    justify-content: center;
    padding: 0 24px;
    overflow-y: auto;
  }

  .login-panel {
    width: 100%;
    max-width: 1040px;
    display: flex;
    flex-direction: column;
    gap: 36px;
  }

  .app-title {
    font-size: 44px;
    font-weight: bold;
    text-align: center;
    color: var(--text-white);
  }

  .fields-section {
    display: flex;
    flex-direction: column;
    gap: 18px;
  }

  .field-group {
    display: flex;
    flex-direction: column;
    gap: 10px;
  }

  .field-label {
    font-size: 21px;
    color: var(--text-white);
  }

  .label-row {
    display: flex;
    justify-content: space-between;
    align-items: center;
  }

  .forgot-link {
    font-size: 17px;
    color: var(--blue);
    text-decoration: none;
  }

  .forgot-link:hover {
    text-decoration: underline;
  }

  .input-wrapper {
    display: flex;
    align-items: center;
    background: var(--bg-input);
    border-radius: 8px;
    height: 62px;
    padding: 0 16px;
    gap: 12px;
  }

  .input-icon {
    color: var(--text-muted);
    font-size: 21px;
    flex-shrink: 0;
    width: 21px;
    text-align: center;
  }

  .input-wrapper input {
    flex: 1;
    min-width: 0;
    background: none;
    border: none;
    outline: none;
    color: var(--text-white);
    font-size: 21px;
  }

  .input-wrapper input::placeholder {
    color: var(--text-muted);
  }

  .eye-toggle {
    background: none;
    border: none;
    cursor: pointer;
    color: var(--text-muted);
    font-size: 21px;
    padding: 0;
    display: flex;
    align-items: center;
    justify-content: center;
    flex: 0 0 32px;
    width: 32px;
    height: 32px;
  }

  .eye-toggle:hover {
    color: var(--text-white);
  }

  .button-row {
    display: flex;
    gap: 14px;
  }

  .login-btn {
    flex: 1;
    height: 72px;
    background: var(--blue);
    border: none;
    border-radius: 12px;
    color: var(--text-white);
    font-size: 24px;
    font-weight: bold;
    cursor: pointer;
    transition: background 0.15s;
  }

  .login-btn:hover {
    background: #1a6fd4;
  }

  .login-btn:disabled {
    opacity: 0.6;
    cursor: not-allowed;
  }

  .key-btn {
    width: 72px;
    height: 72px;
    background: var(--bg-input);
    border: none;
    border-radius: 8px;
    color: var(--text-muted);
    font-size: 26px;
    cursor: pointer;
    display: flex;
    align-items: center;
    justify-content: center;
    transition: color 0.15s;
  }

  .key-btn:hover {
    color: var(--text-white);
  }

  .spinner-wrapper {
    display: flex;
    justify-content: center;
  }

  .spinner {
    font-size: 36px;
    color: var(--blue);
    animation: spin 1s linear infinite;
  }

  @keyframes spin {
    from { transform: rotate(0deg); }
    to   { transform: rotate(360deg); }
  }

  .error-message {
    color: var(--text-error);
    font-size: 21px;
    text-align: center;
    word-wrap: break-word;
  }

  .signup-row {
    color: var(--text-muted);
    font-size: 19px;
    text-align: center;
    white-space: nowrap;
  }

  .signup-row a {
    color: var(--blue);
    text-decoration: none;
  }

  .signup-row a:hover {
    text-decoration: underline;
  }

  .offline-link {
    color: var(--text-muted);
    font-size: 19px;
    cursor: pointer;
  }

  .offline-link:hover {
    color: var(--text-white);
  }
</style>
