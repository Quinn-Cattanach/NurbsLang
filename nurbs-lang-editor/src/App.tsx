import { Editor, type Monaco, type OnMount } from "@monaco-editor/react";
import { useContext, useEffect, useRef, useState } from "react";
import { initializeLanguage } from "./language";
import { WasmContext, type Nurbs3Ptr } from "./WasmContext";
import { NurbsViewer } from "./NurbsViewer";
// import Module from "../../nurbs/build-wasm/nurbs.js";
//
const defaultProgram = `
// An L Bracket
component LBracket(radius: [0, 2]) {
    const startDirection = vec(x: 1, y: 0, z: 0);
    const endDirection = vec(x: 1, y: 1, z: 0);

    const path = BentLine(length: 20, bendOrigin: 0.5, startDirection /* same name as param no need for name:name */, endDirection);

    return Rectangle(u: 5, v: 1).sweep(path);
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

    const { module, ready } = useContext(WasmContext);

    const [volume, setVolume] = useState<Nurbs3Ptr | null>(null);

    useEffect(() => {
        console.log("Wasm is ready!!", ready);
        if (ready && module) {
            console.log(module);

            const path = module.BentLine(20, 0.5, 4, 1, 0, 0, 1, 1, 0);
            const face = module.Rectangle(5, 1);

            setVolume(module.sweep2(face, path));
        }
    }, [ready]);

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
            <div className="flex-1">
                {volume ? <NurbsViewer volumePtr={volume} /> : <></>}
            </div>
        </div>
    );
};
