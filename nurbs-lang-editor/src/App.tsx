import { Editor, type Monaco, type OnMount } from "@monaco-editor/react";
import { useContext, useEffect, useRef, useState } from "react";
import { initializeLanguage } from "./language";
import {
    makeNurbsModuleWrappers,
    WasmContext,
    type Nurbs3Ptr,
} from "./WasmContext";
import { NurbsViewer } from "./NurbsViewer";
import { transpileDSL } from "./transpile";
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

    LBracket(radius: 2).show();

    // Simulated as aluminum fixed to a wall with a load.
    // LBracket.minimize({
    //     objective: MaxStress,
    //     material: Aluminum,
    //     boundaryConditions: [
    //         Directional(direction: -w, condition: Dirichlet(0)),
    //         Directional(direction: u, condition: Neumann(500 * -w)),
    //     ],
    // });


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

    useEffect(() => {
        const handler = function (e) {
            // Check for Ctrl+S (Windows/Linux) or Cmd+S (Mac)
            if ((e.ctrlKey || e.metaKey) && e.key === "s") {
                e.preventDefault();

                if (code && module) {
                    const jscode = transpileDSL(code);
                    console.log(jscode);
                    const wrappers = makeNurbsModuleWrappers(
                        module,
                        (ptr: Nurbs3Ptr) => {
                            console.log("showing", ptr);
                            setVolume(ptr);
                        },
                    );
                    eval(jscode);
                }

                // TODO: convert to js and then need to eval() the js.
                // need to make sure the module is loaded properly so i might wrap in like another module wrapper in the scope of the eval.
            }
        };

        document.addEventListener("keydown", handler);

        return () => {
            document.removeEventListener("keydown", handler);
        };
    }, [ready]);

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
