import { defineConfig } from "vitest/config";

export default defineConfig({
  test: {
    globals: true,
    environment: "node",
    include: ["packages/**/*.test.ts", "apps/**/*.test.ts"],
    exclude: ["**/node_modules/**", "**/.next/**"],
    coverage: {
      provider: "v8",
      include: ["packages/*/src/**", "apps/*/app/api/**"],
      exclude: ["**/*.d.ts", "**/node_modules/**"],
    },
    testTimeout: 10000,
  },
});
