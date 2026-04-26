import { defineConfig } from 'vite';
import { svelte } from '@sveltejs/vite-plugin-svelte';

export default defineConfig({
    plugins: [svelte()],
    base: './', // Crucial for Electron to find assets
    build: {
        outDir: 'dist',
    }
});
