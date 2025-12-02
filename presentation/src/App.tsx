import { useEffect, useState } from "react";
import reactLogo from "./assets/react.svg";
import { invoke } from "@tauri-apps/api/core";
import "./App.css";
import {
    Slide0,
    Slide1,
    Slide2,
    Slide3,
    Slide4,
    Slide5,
    Slide6,
    Slide7,
    Slide8,
} from "./slides/Slides";
import { LogicalSize, Window } from "@tauri-apps/api/window";

function App() {
    const slides = [
        <Slide0 />,
        <Slide1 />,
        <Slide2 />,
        <Slide3 />,
        <Slide4 />,
        <Slide5 />,
        <Slide6 />,
        <Slide7 />,
        <Slide8 />,
    ];
    const [currentSlide, setCurrentSlide] = useState(0);

    useEffect(() => {
        function handleKeyDown(e: KeyboardEvent) {
            if (e.key === "ArrowLeft") {
                setCurrentSlide((p) => (p - 1 + slides.length) % slides.length);

                console.log((p - 1 + slides.length) % slides.length);
            } else if (e.key === "ArrowRight") {
                setCurrentSlide((p) => (p + 1) % slides.length);
            }
        }

        window.addEventListener("keydown", handleKeyDown);
        return () => window.removeEventListener("keydown", handleKeyDown);
    }, []);

    return (
        <div className="w-lvw h-lvh flex overflow-hidden select-none p-16">
            {slides[currentSlide]}
        </div>
    );
}

export default App;
