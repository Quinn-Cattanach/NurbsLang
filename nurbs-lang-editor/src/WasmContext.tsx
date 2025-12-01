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
        vertices: number;
        parametric_coords: number;
        normals: number;
        length: number;
    };
    HEAPF32: Float32Array;
    HEAP8: Int8Array;
};

export type NurbsModule = NurbsModuleRaw & {
    toMesh3: (
        volumePtr: Nurbs3Ptr,
        lods: [number, number, number],
    ) => {
        vertices: Float32Array;
        parametricCoords: Float32Array;
        normals: Float32Array;
    };
};

export function makeNurbsModuleWrappers(
    module: NurbsModuleRaw,
    showFunction: (ptr: Nurbs3Ptr) => void,
) {
    function make1(ptr: Nurbs1Ptr) {
        return {
            ptr,
        };
    }

    function make2(ptr: Nurbs2Ptr) {
        return {
            ptr,
            sweep(path: Nurbs1Ptr | { ptr: Nurbs1Ptr }) {
                console.log("sweeping", ptr, path);

                const pathPtr =
                    typeof path === "number" ? (path as Nurbs1Ptr) : path.ptr;

                const volPtr = module.sweep2(ptr, pathPtr);
                return make3(volPtr);
            },
        };
    }

    function make3(ptr: Nurbs3Ptr) {
        return {
            ptr,
            show() {
                console.log("Showing", ptr);
                showFunction(ptr);
            },
        };
    }
    return {
        Line({ length, x, y, z }: any) {
            return make1(module.Line(length, x, y, z));
        },

        Rectangle({ u, v }: any) {
            console.log("constructing line u", u, "v", v);
            return make2(module.Rectangle(u, v));
        },

        Box({ u, v, w }: any) {
            return make3(module.Box(u, v, w));
        },

        BentLine({
            length,
            bendOrigin,
            radius,
            startDirection,
            endDirection,
        }: any) {
            console.log(
                `constructing bent line ${length}, ${bendOrigin}, ${radius}, ${startDirection}, ${endDirection}`,
            );
            const ptr = module.BentLine(
                length,
                bendOrigin,
                radius,
                startDirection.x,
                startDirection.y,
                startDirection.z,
                endDirection.x,
                endDirection.y,
                endDirection.z,
            );
            return make1(ptr);
        },
    };
}

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
        script.src = "NurbsLang/external/nurbs.js";
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
                        const mesh_raw = rawModule.toMesh3Raw(volumePtr, lods);

                        const vertices_raw = mesh_raw.vertices;
                        const parametric_coords_raw =
                            mesh_raw.parametric_coords;
                        const normals_raw = mesh_raw.normals;
                        const length = mesh_raw.length;

                        const vertex_buffer = rawModule.HEAPF32.buffer;
                        const parameter_buffer = rawModule.HEAPF32.buffer;
                        const normals_buffer = rawModule.HEAPF32.buffer;

                        const vertices = new Float32Array(
                            vertex_buffer,
                            vertices_raw,
                            length * 4,
                        );

                        const parametricCoords = new Float32Array(
                            parameter_buffer,
                            parametric_coords_raw,
                            length,
                        );

                        const normals = new Float32Array(
                            normals_buffer,
                            normals_raw,
                            length * 4,
                        );

                        return { vertices, parametricCoords, normals };
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
