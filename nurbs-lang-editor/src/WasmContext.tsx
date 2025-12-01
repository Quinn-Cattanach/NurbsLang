import {
    createContext,
    useContext,
    useEffect,
    useState,
    type ReactNode,
} from "react";

export type Nurbs1Ptr = number & { readonly __nurbs1: unique symbol };
export type Nurbs2Ptr = number & { readonly __nurbs2: unique symbol };
export type Nurbs3Ptr = number & { readonly __nurbs3: unique symbol };
export type NurbsModuleRaw = {
    destroyNurbs1: (ptr: Nurbs1Ptr) => void;
    destroyNurbs2: (ptr: Nurbs2Ptr) => void;
    destroyNurbs3: (ptr: Nurbs3Ptr) => void;
    Line: (length: number, x: number, y: number, z: number) => Nurbs1Ptr;
    Rectangle: (width: number, height: number) => Nurbs2Ptr;
    Box: (width: number, height: number, depth: number) => Nurbs3Ptr;
    BentLine: (
        length: number,
        bend_origin_u: number,
        radius: number,
        base_x: number,
        base_y: number,
        base_z: number,
        bend_x: number,
        bend_y: number,
        bend_z: number,
    ) => Nurbs1Ptr;
    sweep2: (toolPtr: Nurbs2Ptr, pathPtr: Nurbs1Ptr) => Nurbs3Ptr;
    toMesh3Raw: (
        volumePtr: Nurbs3Ptr,
        lods_ptr: number[],
    ) => {
        data: number;
        length: number;
    };
    HEAPF32: Float32Array;
    HEAP8: Int8Array;
};

export type NurbsModule = NurbsModuleRaw & {
    toMesh3: (
        volumePtr: Nurbs3Ptr,
        lods: [number, number, number],
    ) => Float32Array;
};

export type WasmContextType = {
    module: NurbsModule | null;
    ready: boolean;
};

export const WasmContext = createContext<WasmContextType>({
    module: null,
    ready: false,
});

type ProviderProps = { children: ReactNode };

export const WasmContextProvider = ({ children }: ProviderProps) => {
    const [module, setModule] = useState<NurbsModule | null>(null);
    const [ready, setReady] = useState(false);

    useEffect(() => {
        const script = document.createElement("script");
        script.src = "/external/nurbs.js";
        script.async = true;
        script.onload = () => {
            // window.Module is injected by Emscripten
            const rawModule = (window as any).Module as
                | NurbsModuleRaw
                | undefined;
            if (!rawModule) return;

            // Wait for Emscripten runtime to initialize
            rawModule.onRuntimeInitialized = () => {
                const module: NurbsModule = {
                    ...rawModule,
                    toMesh3: (
                        volumePtr: Nurbs3Ptr,
                        lods: [number, number, number],
                    ) => {
                        const meshRaw = rawModule.toMesh3Raw(volumePtr, lods);
                        const buffer = rawModule.HEAPF32.buffer;

                        const view = new Float32Array(
                            buffer,
                            meshRaw.data,
                            meshRaw.length * 4,
                        );

                        return view;
                    },
                };

                setModule(module);
                setReady(true);

                console.log("Testing NURBS module...");
                const path = module.BentLine(20, 0.5, 4, 1, 0, 0, 1, 1, 0);
                console.log("Created path:", path);

                const face = module.Rectangle(5, 1);
                console.log("Created face:", face);

                const volume = module.sweep2(face, path);
                console.log("Created volume:", volume);

                const tesselated = module.toMesh3(volume, [20, 20, 20]);
                console.log("Tesselated mesh:", tesselated);
                console.log("Mesh length:", tesselated.length);
                console.log("First few vertices:", tesselated.slice(0, 12));
            };
        };

        document.body.appendChild(script);

        return () => {
            if (document.body.contains(script)) {
                document.body.removeChild(script);
            }
        };
    }, []);

    return (
        <WasmContext.Provider value={{ module, ready }}>
            {children}
        </WasmContext.Provider>
    );
};
