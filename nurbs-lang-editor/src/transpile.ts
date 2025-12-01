// THank you chat G P T for this code <3

export function splitTopLevelParams(s: string): string[] {
    const out: string[] = [];
    let buf = "";
    let depth = 0;
    for (let i = 0; i < s.length; i++) {
        const c = s[i];
        if (c === "[" || c === "(" || c === "{") depth++;
        if (c === "]" || c === ")" || c === "}") depth--;
        if (c === "," && depth === 0) {
            out.push(buf.trim());
            buf = "";
            continue;
        }
        buf += c;
    }
    if (buf.trim()) out.push(buf.trim());
    return out;
}

function isObjectLiteral(text: string) {
    return text.trim().startsWith("{") && text.trim().endsWith("}");
}

function isAlreadyWrapped(text: string) {
    return text.startsWith("wrappers.");
}

function removeComments(s: string): string {
    s = s.replace(/\/\*[\s\S]*?\*\//g, "");
    s = s.replace(/\/\/.*$/gm, "");
    return s;
}

function findMatchingClose(s: string, openIndex: number): number {
    const opens = { "(": ")", "[": "]", "{": "}" } as Record<string, string>;
    const openChar = s[openIndex];
    const closeChar = opens[openChar];
    if (!closeChar) return -1;
    let depth = 0;
    for (let i = openIndex; i < s.length; i++) {
        const c = s[i];
        if (c === openChar) depth++;
        else if (c === closeChar) {
            depth--;
            if (depth === 0) return i;
        }
    }
    return -1;
}

function replaceDirections(s: string): string {
    s = s.replace(/\+x/g, "vec(x: 1, y: 0, z: 0)");
    s = s.replace(/\-x/g, "vec(x: -1, y: 0, z: 0)");
    s = s.replace(/\+y/g, "vec(x: 0, y: 1, z: 0)");
    s = s.replace(/\-y/g, "vec(x: 0, y: -1, z: 0)");
    s = s.replace(/\+z/g, "vec(x: 0, y: 0, z: 1)");
    s = s.replace(/\-z/g, "vec(x: 0, y: 0, z: -1)");
    return s;
}

function replaceTopLevelNamedCalls(
    s: string,
    names: string[],
    replacer: (name: string, argsText: string) => string,
): string {
    const nameSet = new Set(names);
    let out = "";
    let i = 0;
    while (i < s.length) {
        const rest = s.slice(i);
        const nameMatch = rest.match(/^[A-Za-z_][A-Za-z0-9_]*/);
        if (nameMatch) {
            const name = nameMatch[0];
            const nameLen = name.length;
            const afterNameIdx = i + nameLen;

            const next = s[afterNameIdx];
            if (
                nameSet.has(name) &&
                next &&
                s.slice(afterNameIdx).trimStart().startsWith("(")
            ) {
                let p = afterNameIdx;
                while (p < s.length && /\s/.test(s[p])) p++;
                if (s[p] === "(") {
                    const close = findMatchingClose(s, p);
                    if (close !== -1) {
                        const argsText = s.slice(p + 1, close);
                        const replacement = replacer(name, argsText);
                        out += replacement;
                        i = close + 1;
                        continue;
                    }
                }
            }
        }
        out += s[i];
        i++;
    }
    return out;
}

function parseArgsToObjText(argsText: string): string {
    const parts = splitTopLevelParams(argsText);
    const kv: string[] = [];
    for (const p of parts) {
        if (!p) continue;

        const named = p.match(/^([A-Za-z_]\w*)\s*:\s*([\s\S]+)$/s);
        if (named) {
            const key = named[1];
            const val = named[2].trim();
            kv.push(`${key}: ${val}`);
        } else {
            const ident = p.match(/^[A-Za-z_]\w*$/);
            if (ident) kv.push(`${p}: ${p}`);
            else kv.push(p);
        }
    }
    return kv.join(", ");
}

// ===============================================================
//  transformFragment (updated)
//  - REMOVE sweep/show rewrite
//  - Keep primitive wrapper insertion
// ===============================================================

function transformFragment(
    frag: string,
    knownFns: string[],
    componentNames: string[],
): string {
    let s = frag;

    // vec(...) → { x:..., y:..., z:... }
    s = s.replace(/vec\s*\(/g, "__VEC_REPLACE_START__(");
    while (true) {
        const pos = s.indexOf("__VEC_REPLACE_START__(");
        if (pos === -1) break;
        const open = pos + "__VEC_REPLACE_START__".length;
        const close = findMatchingClose(s, open);
        if (close === -1) break;
        const inside = s.slice(open + 1, close);
        const obj = parseArgsToObjText(inside);
        s = s.slice(0, pos) + `{${obj}}` + s.slice(close + 1);
    }

    // Wrap primitive constructors
    if (knownFns.length > 0) {
        s = replaceTopLevelNamedCalls(s, knownFns, (name, argsText) => {
            if (isAlreadyWrapped(name)) return `${name}(${argsText})`;

            const trimmed = argsText.trim();
            if (isObjectLiteral(trimmed)) {
                return `wrappers.${name}(${trimmed})`;
            }

            return `wrappers.${name}({ ${parseArgsToObjText(trimmed)} })`;
        });
    }

    // Component calls (no wrappers prefix)
    if (componentNames.length > 0) {
        s = replaceTopLevelNamedCalls(s, componentNames, (name, argsText) => {
            return `${name}({ ${parseArgsToObjText(argsText)} })`;
        });
    }

    // **NO sweep or show rewriting anymore**
    return s;
}

// ===============================================================
//  Component extraction, output construction — unchanged
// ===============================================================

function extractComponents(source: string) {
    const components: Array<{
        name: string;
        paramsText: string;
        bodyText: string;
        start: number;
        end: number;
    }> = [];

    let i = 0;
    while (i < source.length) {
        const compMatch = source
            .slice(i)
            .match(/\bcomponent\s+([A-Za-z_]\w*)\s*\(/);
        if (!compMatch) break;
        const name = compMatch[1];
        const matchStart = i + (compMatch.index ?? 0);
        const parenPos = matchStart + compMatch[0].indexOf("(");
        const paramsClose = findMatchingClose(source, parenPos);
        if (paramsClose === -1) break;
        const paramsText = source.slice(parenPos + 1, paramsClose);

        let bracePos = paramsClose + 1;
        while (bracePos < source.length && /\s/.test(source[bracePos]))
            bracePos++;
        if (source[bracePos] !== "{") {
            i = paramsClose + 1;
            continue;
        }

        const bodyClose = findMatchingClose(source, bracePos);
        if (bodyClose === -1) break;

        const bodyText = source.slice(bracePos + 1, bodyClose);
        components.push({
            name,
            paramsText,
            bodyText,
            start: matchStart,
            end: bodyClose + 1,
        });
        i = bodyClose + 1;
    }

    let tail = "";
    let last = 0;
    for (const comp of components) {
        tail += source.slice(last, comp.start);
        last = comp.end;
    }
    tail += source.slice(last);

    return { components, tail };
}

export function transpileDSL(source: string): string {
    const codeNoComments = removeComments(source);
    const codeTransformDirections = replaceDirections(codeNoComments);
    const { components, tail } = extractComponents(codeTransformDirections);

    if (components.length === 0) throw new Error("No component found");

    interface Param {
        name: string;
        low: string;
        high: string;
    }

    const compInfos = components.map((c) => {
        const parts = splitTopLevelParams(c.paramsText.trim());
        const params: Param[] = parts.map((p) => {
            const m = p.match(
                /^([A-Za-z_]\w*)\s*:\s*\[\s*(.*?)\s*,\s*(.*?)\s*\]$/s,
            );
            if (!m) throw new Error("Can't parse param: " + p);
            return { name: m[1], low: m[2].trim(), high: m[3].trim() };
        });

        return { name: c.name, params, bodyText: c.bodyText };
    });

    const knownFns = ["Line", "Rectangle", "Box", "BentLine"];
    const componentNames = compInfos.map((ci) => ci.name);

    const transformedComponents = compInfos.map((ci) => {
        const transformedBody = transformFragment(
            ci.bodyText,
            knownFns,
            componentNames,
        );

        const argNames = ci.params.map((p) => p.name).join(", ");
        const fnHeader = `const ${ci.name} = ({ ${argNames} }) => {`;

        const paramObjs = ci.params
            .map(
                (p) =>
                    `  const ${p.name}Variable = { name: "${p.name}", lower: ${p.low}, upper: ${p.high} };`,
            )
            .join("\n");

        const bodyIndented = transformedBody
            .split("\n")
            .map((l) => (l.trim() ? "  " + l.trim() : ""))
            .join("\n");

        return [
            `// Transpiled component: ${ci.name}`,
            fnHeader,
            paramObjs ? paramObjs : "",
            "",
            bodyIndented,
            "};",
        ]
            .filter(Boolean)
            .join("\n");
    });

    const transformedTail = transformFragment(tail, knownFns, componentNames);

    return [
        "// Transpiled output (expects 'wrappers' in scope, e.g. const wrappers = makeNurbsModuleWrappers(module);)",
        transformedComponents.join("\n\n"),
        "",
        "// Top-level code",
        transformedTail.trim(),
    ]
        .filter(Boolean)
        .join("\n\n");
}
