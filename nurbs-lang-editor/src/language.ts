import type { Monaco } from "@monaco-editor/react";
import light from "./assets/light.json";

export const initializeLanguage = (monaco: Monaco) => {
    const theme = {
        base: "vs",
        inherit: true,
        rules: light.tokenColors.map((tc) => ({
            token: tc.scope,
            foreground: tc.settings.foreground?.replace("#", "") || undefined,
            fontStyle: tc.settings.fontStyle,
        })),
        colors: light.colors,
    };
    monaco.editor.defineTheme("light", theme);
    monaco.languages.register({ id: "nurbsscript" });

    const keywords = ["component", "const", "return"];
    const directions = ["\\+x", "\\-x", "\\+y", "\\-y", "\\+z", "\\-z"];
    const constructors = [
        "Box",
        "Bend",
        "Directional",
        "Dirichlet",
        "Neumann",
        "MaxStress",
    ];
    const materials = ["Aluminum"];
    const functions = ["bend", "minimize"];

    monaco.languages.setMonarchTokensProvider("nurbsscript", {
        tokenizer: {
            root: [
                [new RegExp(`\\b(${keywords.join("|")})\\b`), "keyword"],
                [new RegExp(`(${directions.join("|")})`), "keyword"],

                [new RegExp(`\\b(${constructors.join("|")})\\b`), "type"],
                [new RegExp(`\\b(${materials.join("|")})\\b`), "constant"],
                [new RegExp(`\\b(${functions.join("|")})\\b`), "variable"],

                [
                    /\[([0-9]+|vec)(\.([0-9]+)?)?(f)?\s*,\s*([0-9]+|vec)(\.([0-9]+)?)?(f)?\]/,
                    "string",
                ],
                [/([0-9]+|vec)(\.([0-9]+)?)?(f)?/, "number"],
                [/[{}()\[\]]/, "@brackets"],
                [/[a-zA-Z_]\w*/, "identifier"],
                [/^\s*\/\/.*/, "comment"],
            ],
        },
    });

    monaco.languages.setLanguageConfiguration("nurbsscript", {
        comments: {
            lineComment: "//",
            blockComment: ["/*", "*/"],
        },
        brackets: [
            ["{", "}"],
            ["[", "]"],
            ["(", ")"],
        ],
        autoClosingPairs: [
            { open: "{", close: "}" },
            { open: "[", close: "]" },
            { open: "(", close: ")" },
            { open: '"', close: '"' },
        ],
    });
};
