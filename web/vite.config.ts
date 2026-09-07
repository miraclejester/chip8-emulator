import { defineConfig } from 'vite';

export default defineConfig({
  // Set to '/<repo-name>/' when deploying to GitHub Pages under a subpath.
  base: './',
  server: { port: 5173 },
});
