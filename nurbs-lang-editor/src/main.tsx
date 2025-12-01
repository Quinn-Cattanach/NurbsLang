import { StrictMode } from "react";
import { createRoot } from "react-dom/client";
import "./index.css";
import { App } from "./App";
import "./language";
import { WasmContextProvider } from "./WasmContext";
import { RenderingManager } from "./RenderingContext";

createRoot(document.getElementById("root")!).render(
    <StrictMode>
        <WasmContextProvider>
            <RenderingManager>
                <App />
            </RenderingManager>
        </WasmContextProvider>
    </StrictMode>,
);
