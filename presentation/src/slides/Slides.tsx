import { ReactNode, useState } from "react";
import { InlineMath, BlockMath } from "react-katex";
import { Editor, type Monaco, type OnMount } from "@monaco-editor/react";
import { initializeLanguage } from "./language";
import { Button } from "@/components/ui/button";

// function App() {
//   return (
//     <div>
//       <h1>LaTeX Equations</h1>

//       {/* Inline equation */}
//       <p>
//         The formula is <InlineMath math="E = mc^2" /> for energy.
//       </p>

//       {/* Block equation */}
//       <BlockMath math="\int_{0}^{\infty} x^2 dx" />

//       {/* Complex equation */}
//       <BlockMath math="\frac{-b \pm \sqrt{b^2 - 4ac}}{2a}" />
//     </div>
//   );
// }

export const SuperTitle = ({ children }: { children: ReactNode }) => (
    <p className="text-5xl text-center" style={{ fontFamily: "LatinModernSC" }}>
        {children}
    </p>
);

export const Title = ({ children }: { children: ReactNode }) => (
    <p
        className="text-7xl text-center"
        style={{ fontFamily: "LatinModernDemi" }}
    >
        {children}
    </p>
);

export const Heading = ({ children }: { children: ReactNode }) => (
    <p className="text-7xl text-left" style={{ fontFamily: "LatinModernDemi" }}>
        {children}
    </p>
);

export const Subtitle = ({ children }: { children: ReactNode }) => (
    <p className="text-3xl text-center" style={{ fontFamily: "LatinModern" }}>
        {children}
    </p>
);

export const Text = ({ children }: { children: ReactNode }) => (
    <p className="text-3xl" style={{ fontFamily: "LatinModern" }}>
        {children}
    </p>
);

export const BlockCode = ({ children }: { children: ReactNode }) => (
    <div className="p-4 rounded-md bg-neutral-100">
        <pre>
            <code
                className="text-3xl"
                style={{ fontFamily: "LatinModernMono" }}
            >
                {children}
            </code>
        </pre>
    </div>
);

export const InlineCode = ({ children }: { children: ReactNode }) => (
    <code
        className="px-4 rounded-md bg-neutral-100 text-3xl"
        style={{ fontFamily: "LatinModernMono" }}
    >
        {children}
    </code>
);

export const ItalicText = ({ children }: { children: ReactNode }) => (
    <p className="text-3xl italic" style={{ fontFamily: "LatinModern" }}>
        {children}
    </p>
);

export const Slide0 = () => {
    return (
        <div className="m-auto flex flex-col items-center gap-2">
            <SuperTitle>NurbsLang</SuperTitle>
            <Title>A Declarative Language for Automated Iterative Design</Title>
            <br />
            <Subtitle>Panya and Quinn</Subtitle>
        </div>
    );
};

export const Slide1 = () => {
    return (
        <div className="m-16 w-full h-full flex flex-col gap-8">
            <Heading>NurbsLang Overview</Heading>
            <Text>
                Traditionally, designers create CAD models, tesselate and define
                boundary conditions, then perform finite element analysis (FEA).
                After the results are inspected, the designers manually update
                the model if the results of the simulation identified problem
                areas.
                <br></br>
                <br></br>
                NurbsLang unifies the iterative design process into one language
                where designers can construct component templates, apply
                materials and boundary conditions, and tune the template's
                parameters to the model's objective.
            </Text>
        </div>
    );
};

export const Slide2 = () => {
    return (
        <div className="m-16 w-full h-full flex flex-col gap-8">
            <Heading>NURBS</Heading>
            <Text>
                A Nonuniform Rational B-Spline is a parametric curve on{" "}
                <InlineMath math="u \in [0, 1]" /> defined by{" "}
                <BlockMath math="C(u) = \frac{\sum_{i} N_{i}^{p}(u) w_{i} P_{i}}{\sum_{i} N_{i}^{p}(u) w_{i}}" />
                where <InlineMath math="N_i^p" /> is the <i>basis</i> function
                for each of the <InlineMath math="n" /> weighted (
                <InlineMath math="w\in \R^n" />) control point{" "}
                <InlineMath math="P\in \R^{3\times n}" /> of degree{" "}
                <InlineMath math="p" />. The basis functions{" "}
                <InlineMath math="N_i^p" /> are defined with the recurrence:
                <BlockMath
                    math="N_i^{p}(u) =
\begin{cases}
0 & p = 0 \text{ and } u \notin [u_i,\,u_{i+1}) \\[6pt]
1 & p = 0 \text{ and } u_i \le u < u_{i+1} \\[6pt]
\frac{u - u_i}{u_{i+p} - u_i}\, N_i^{p-1}(u)
+
\frac{u_{i+p+1} - u}{u_{i+p+1} - u_{i+1}}\, N_{i+1}^{p-1}(u)
& p > 0
\end{cases}
"
                />
                where{" "}
                <InlineMath math="u\in \{u \in [0, 1]^{p + n + 1} \mid i < j \implies u_i \le u_j\}" />{" "}
                is the <i>knot vector</i>. The knot vector controls how much of
                the <InlineMath math="p" /> control points within the{" "}
                <i>knot span</i> at each parameterized point{" "}
                <InlineMath math="u" />.
            </Text>
        </div>
    );
};

export const Slide3 = () => {
    return (
        <div className="m-16 w-full h-full flex flex-col gap-8">
            <Heading>NURBS Tensor Product Patches</Heading>
            <Text>
                We can also define <i>surfaces</i>, <i>volumes</i>, and further
                order objects by creating <i>tensor product patches</i>. A NURBS
                volume, for example, is defined on a{" "}
                <InlineMath math="(u, v, w)\in[0, 1]^3" /> parametric domain:{" "}
                <BlockMath math="V(u, v, w) = \frac{\sum_{ijk} N_{ijk}^{pqr}(u, v, w) w_{ijk} P_{ijk}}{\sum_{ijk} N_{ijk}^{pqr}(u, v, w) w_{ijk}}" />
                <BlockMath math="N_{ijk}^{pqr} = N_i^p \otimes N_j^q \otimes N_k^r" />
                For patches that do not self-intersect, volumes can be defined
                on their boundaries, that is,{" "}
                <BlockMath math="(u, v, 0), (u, v, 1), (u, 0, w), (u, 1, w), (0, v, w), (1, v, w)." />
            </Text>
        </div>
    );
};

export const Slide4 = () => {
    return (
        <div className="m-16 w-full h-full flex flex-col gap-8">
            <Heading>Motivating Example</Heading>
            <Text>
                To motivate the language design, we designed an L-Bracket with a
                corner radius and minimizing the maximal stress of the object by
                varying along the radius.
            </Text>
        </div>
    );
};

export const Slide5 = () => {
    return (
        <div className="m-16 w-full h-full flex flex-col gap-8">
            <Heading>Patch Construction Techniques</Heading>
            <Text>
                For our DSL, we tried to keep construction within the parametric
                domain. In order to create or motivating example, we did this by
                defining a set of <i>primitives</i>, like{" "}
                <InlineCode>Box</InlineCode>, <InlineCode>Rectangle</InlineCode>
                , and <InlineCode>Line</InlineCode> and a small set of
                operations: <i>sweep</i> and <i>bends</i>.
            </Text>
        </div>
    );
};

export const Slide6 = () => {
    return (
        <div className="m-16 w-full h-full flex flex-col gap-8">
            <Heading>Boundary Conditions</Heading>
            <Text>
                In order to perform isogeometric analysis, we defined boundary
                conditions on parametric{" "}
                <InlineMath math="(u, v, w)\subseteq [0, 1]^3" /> regions of the
                curve. For our L Bracket minimum viable product, we provided
                ways to create <i>Dirichlet</i> and <i>Neumann</i> boundary
                conditions.
            </Text>
        </div>
    );
};

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

const EditorApp = () => {
    return (
        <iframe
            src="https://quinn-cattanach.github.io/NurbsLang/"
            style={{ width: "100%", height: "100%", border: "none" }}
        />
    );
};

export const Slide7 = () => {
    const [code, setCode] = useState<string | undefined>(defaultProgram);

    const handleEditorWillMount = (monaco: Monaco) => {
        initializeLanguage(monaco);
    };

    return (
        <div className="flex flex-col h-screen w-full">
            {/* Top Section */}
            <div className="m-16 w-full flex-col gap-8">
                <Heading>Demo</Heading>
            </div>

            <div className="w-full flex-1 mb-20">
                <EditorApp />
            </div>
        </div>
    );
};
