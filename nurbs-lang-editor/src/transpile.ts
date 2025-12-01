function splitTopLevelParams(s: string): string[] {
    const out: string[] = [];
    let buf = "";
    let depth = 0;

    for (let i = 0; i < s.length; i++) {
        const c = s[i];

        if (c === "[") depth++;
        if (c === "]") depth--;

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

export function transpileDSL(source: string): string {
    function removeComments(s: string) {
        // remove /* ... */ first
        s = s.replace(/\/\*[\s\S]*?\*\//g, "");
        // remove // to end of line
        s = s.replace(/\/\/.*$/gm, "");
        return s;
    }

    const code = removeComments(source);

    const compRe =
        /component\s+([A-Za-z_]\w*)\s*\(\s*([^\)]*)\)\s*\{\s*([\s\S]*?)\s*\}\s*$/m;
    const m = code.match(compRe);
    if (!m) throw new Error("No component found");

    const compName = m[1];
    const paramsRaw = m[2].trim();
    const bodyRaw = m[3];

    interface Param {
        name: string;
        low: string;
        high: string;
    }
    const params: Param[] = [];

    if (paramsRaw.length > 0) {
        const parts = splitTopLevelParams(paramsRaw);
        for (const p of parts) {
            const pm = p.match(
                /^([A-Za-z_]\w*)\s*:\s*\[\s*(.*?)\s*,\s*(.*?)\s*\]\s*$/,
            );
            if (!pm) throw new Error("Can't parse param: " + p);
            params.push({ name: pm[1], low: pm[2].trim(), high: pm[3].trim() });
        }
    }

    function parseArgsToObjText(argsText: string) {
        const parts = argsText
            .split(",")
            .map((s) => s.trim())
            .filter(Boolean);
        const kv: string[] = [];
        for (const p of parts) {
            const named = p.match(/^([A-Za-z_]\w*)\s*:\s*(.+)$/s);
            if (named) {
                const key = named[1];
                const val = named[2].trim();
                kv.push(`${key}: ${val}`);
                continue;
            }
            kv.push(`${p}: ${p}`);
        }
        return kv.join(", ");
    }

    function replaceVecs(s: string) {
        return s.replace(/vec\s*\(\s*([^)]+)\)/g, (_all, inside) => {
            const objText = parseArgsToObjText(inside);
            return `{ ${objText} }`;
        });
    }

    const knownFns = ["Line", "Rectangle", "Box", "BentLine"];
    function wrapKnownCalls(s: string) {
        for (const fn of knownFns) {
            const re = new RegExp(fn + "\\s*\\(([^)]*)\\)", "g");
            s = s.replace(re, (_all, inside) => {
                const objText = parseArgsToObjText(inside);
                return `wrappers.${fn}({ ${objText} })`;
            });
        }
        return s;
    }

    function replaceSweepCalls(s: string) {
        return s.replace(
            /(wrappers\.[A-Za-z_]\w*\(\{[^\)]*\}\))\s*\.sweep\s*\(\s*([^)]+)\s*\)/g,
            (_all, toolCall, pathArg) => {
                return `wrappers.sweep2({ tool: ${toolCall}, path: ${pathArg.trim()} })`;
            },
        );
    }

    let transformedBody = bodyRaw;

    transformedBody = replaceVecs(transformedBody);
    transformedBody = wrapKnownCalls(transformedBody);
    transformedBody = replaceSweepCalls(transformedBody);

    const argNames = params.map((p) => p.name).join(", ");
    const fnHeader = `const ${compName} = ({ ${argNames} }) => {`;

    const paramObjs = params
        .map(
            (p) =>
                `  const ${p.name}Variable = { name: "${p.name}", lower: ${p.low}, upper: ${p.high} };`,
        )
        .join("\n");

    const finalJs = [
        `// Transpiled component: ${compName}`,
        fnHeader,
        paramObjs ? paramObjs : "",
        "",
        transformedBody
            .split("\n")
            .map((line) => (line.trim() ? "  " + line.trim() : ""))
            .join("\n"),
        "};",
    ]
        .filter(Boolean)
        .join("\n");

    return finalJs;
}
