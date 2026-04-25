// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

'use strict';

const { test } = require('node:test');
const assert   = require('node:assert/strict');
const { login } = require('../src/auth');

test('login with valid credentials returns tokens', async () => {
  const email       = process.env.POINTLESS_DEBUG_USERNAME;
  const password    = process.env.POINTLESS_DEBUG_PASSWORD;
  const supabaseUrl = process.env.POINTLESS_SUPABASE_URL;
  const anonKey     = process.env.POINTLESS_SUPABASE_ANON_KEY;

  assert.ok(email,       'POINTLESS_DEBUG_USERNAME env var must be set');
  assert.ok(password,    'POINTLESS_DEBUG_PASSWORD env var must be set');
  assert.ok(supabaseUrl, 'POINTLESS_SUPABASE_URL env var must be set');
  assert.ok(anonKey,     'POINTLESS_SUPABASE_ANON_KEY env var must be set');

  const result = await login({ email, password, supabaseUrl, anonKey });

  assert.ok(result.accessToken,   'accessToken should be present');
  assert.ok(result.userId,        'userId should be present');
  assert.ok(result.refreshToken,  'refreshToken should be present');
  assert.ok(result.expiresIn > 0, 'expiresIn should be positive');
});

test('login with wrong credentials throws', async () => {
  const supabaseUrl = process.env.POINTLESS_SUPABASE_URL;
  const anonKey     = process.env.POINTLESS_SUPABASE_ANON_KEY;

  assert.ok(supabaseUrl, 'POINTLESS_SUPABASE_URL env var must be set');
  assert.ok(anonKey,     'POINTLESS_SUPABASE_ANON_KEY env var must be set');

  await assert.rejects(
    () => login({ email: 'nobody@nowhere.invalid', password: 'wrong', supabaseUrl, anonKey }),
    /HTTP 4/
  );
});
