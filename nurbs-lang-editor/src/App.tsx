import { Editor, type Monaco, type OnMount } from "@monaco-editor/react";
import { useEffect, useRef, useState } from "react";
import { initializeLanguage } from "./language";
// import Module from "../../nurbs/build-wasm/nurbs.js";

const defaultProgram = `
// An L Bracket
component LBracket(radius: [0, 2]) {
    const direction = +x;
    const origin = vec(0, 0, 0);

    Box(u: 10, v: 3, w: 1).bend(direction, origin);
}

// Simulated as aluminum fixed to a wall with a load.
LBracket.minimize({
    objective: MaxStress,
    material: Aluminum,
    boundaryConditions: [
        Directional(direction: -w, condition: Dirichlet(0)),
        Directional(direction: u, condition: Neumann(500 * -w)),
    ],
});

`;

export const App = () => {
    const [code, setCode] = useState<string | undefined>(defaultProgram);

    const handleEditorWillMount = (monaco: Monaco) => {
        initializeLanguage(monaco);
    };

    return (
        <div className="w-lvw h-lvh flex">
            <div className="flex-1 h-full">
                <Editor
                    defaultLanguage="nurbsscript"
                    value={code}
                    onChange={(value) => setCode(value)}
                    theme="light"
                    beforeMount={handleEditorWillMount}
                    options={{
                        scrollBeyondLastLine: false,
                        automaticLayout: true,
                        minimap: { enabled: false },
                    }}
                />
            </div>
            <div className="flex-1"></div>
        </div>
    );
};
