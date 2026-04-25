// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

'use strict';

/**
 * @param {{ email: string, password: string, supabaseUrl: string, anonKey: string }} params
 * @returns {Promise<{ accessToken: string, userId: string, refreshToken: string, expiresIn: number }>}
 * @throws {Error} on network failure, non-200 status, or missing fields in response
 */
async function login({ email, password, supabaseUrl, anonKey }) {
  const url = `https://${supabaseUrl}/auth/v1/token?grant_type=password`;

  const response = await fetch(url, {
    method: 'POST',
    headers: {
      'apikey': anonKey,
      'Content-Type': 'application/json',
    },
    body: JSON.stringify({ email, password }),
  });

  if (response.status !== 200) {
    const text = await response.text();
    throw new Error(`Login failed: HTTP ${response.status} - ${text}`);
  }

  const json = await response.json();

  if (!json.access_token) {
    throw new Error('Login response missing access_token');
  }
  if (!json.user?.id) {
    throw new Error('Login response missing user.id');
  }

  return {
    accessToken: json.access_token,
    userId: json.user.id,
    refreshToken: json.refresh_token ?? '',
    expiresIn: json.expires_in ?? 0,
  };
}

module.exports = { login };
