import {
    createContext,
    useEffect,
    useRef,
    useState,
    type Dispatch,
    type ReactNode,
    type RefObject,
    type SetStateAction,
} from "react";

export type RenderingContext = {
    // MARK: References
    // (for rendering)
    // im using refs because using state destorys perfomance when using canvas.

    fovRef: RefObject<number>;

    // MARK: State variables
    // (for UI)
    // so react can automatically bind value to inputs.

    fovState: number;
    setFovState: Dispatch<SetStateAction<number>>;
};

export const renderingContext = createContext<RenderingContext>(undefined as any as RenderingContext);

export const RenderingManager = ({ children }: { children: ReactNode }) => {
    const [fovState, setFovState] = useState(0.0);
    const fovRef = useRef<number>(fovState);

    useEffect(() => {
        fovRef.current = fovState;
    }, [fovState]);

    return <renderingContext.Provider value={{ fovRef, fovState, setFovState }}>{children}</renderingContext.Provider>;
};
