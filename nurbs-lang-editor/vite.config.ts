import { defineConfig } from "vite";

export default defineConfig({
    base: "./", // VERY IMPORTANT for GH Pages + WASM
    build: {
        target: "esnext",
    },
});
