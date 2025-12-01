import { useRef, useEffect, useContext } from "react";
import type { Nurbs3Ptr } from "./WasmContext";
import { WasmContext } from "./WasmContext";

type NurbsViewerProps = {
    volumePtr: Nurbs3Ptr;
    lods?: [number, number, number];
};

const shaderCode = `
struct Uniforms {
zoom: f32,
rotX: f32,
rotY: f32,
pad: f32,
};

struct VertexOutput {
@builtin(position) Position : vec4<f32>,
@location(0) vViewPos : vec3<f32>, // Pass the rotated position to Fragment Shader
};

@group(0) @binding(0) var<uniform> u : Uniforms;

@vertex
fn vs_main(@location(0) data: vec4<f32>) -> VertexOutput {
var out: VertexOutput;

// 1. Center
var pos = data.xyz - vec3<f32>(0.5, 0.5, 0.5);

// 2. Rotate X
let cX = cos(u.rotX);
let sX = sin(u.rotX);
let ry = pos.y * cX - pos.z * sX;
let rz = pos.y * sX + pos.z * cX;
pos.y = ry;
pos.z = rz;

// 3. Rotate Y
let cY = cos(u.rotY);
let sY = sin(u.rotY);
let rx = pos.x * cY + pos.z * sY;
let rz2 = -pos.x * sY + pos.z * cY;
pos.x = rx;
pos.z = rz2;

// Pass the "View Space" position (rotated but not squashed) to fragment shader
// We need this to calculate accurate normals
out.vViewPos = pos;

// 4. Output Clip Position
out.Position = vec4<f32>(
pos.x * u.zoom,
pos.y * u.zoom,
(pos.z * u.zoom * 0.1) + 0.5, // Squash Z for clip space
1.0
);

return out;
}

@fragment
fn fs_main(@location(0) vViewPos: vec3<f32>) -> @location(0) vec4<f32> {
// --- CALCULATE NORMAL FROM POSITION ---

// 1. Get the derivatives of the position (how much P changes per pixel)
let dp_dx = dpdx(vViewPos);
let dp_dy = dpdy(vViewPos);

// 2. The Cross Product of these two vectors gives the surface Normal
// normalize ensures length is 1.0
let N = normalize(cross(dp_dx, dp_dy));

// 3. Map Normal (-1.0 to 1.0) to Color (0.0 to 1.0)
// Standard Normal Map colors: X=Red, Y=Green, Z=Blue
let color = N * 0.5 + 0.5;

return vec4<f32>(color, 1.0);
}
`;

export const NurbsViewer = ({
    volumePtr,
    lods = [10, 10, 80],
}: NurbsViewerProps) => {
    const { module, ready } = useContext(WasmContext);
    const canvasRef = useRef<HTMLCanvasElement | null>(null);
    const animationRef = useRef<number>(0);

    const cameraState = useRef({
        zoom: 0.2,
        rotX: 0,
        rotY: 0,
    });

    const isDragging = useRef(false);
    const lastMousePos = useRef({ x: 0, y: 0 });

    useEffect(() => {
        if (!module || !ready) return;
        const canvas = canvasRef.current;
        if (!canvas || !navigator.gpu) return;

        const resizeCanvas = () => {
            const devicePixelRatio = window.devicePixelRatio || 1;
            canvas.width = canvas.clientWidth * devicePixelRatio;
            canvas.height = canvas.clientHeight * devicePixelRatio;
        };
        resizeCanvas();
        window.addEventListener("resize", resizeCanvas);

        const init = async () => {
            const adapter = await navigator.gpu.requestAdapter();
            if (!adapter) return;
            const device = await adapter.requestDevice();

            const context = canvas.getContext("webgpu") as GPUCanvasContext;
            const format = navigator.gpu.getPreferredCanvasFormat();
            context.configure({ device, format, alphaMode: "opaque" });

            const vertices = module.toMesh3(volumePtr, lods);
            const vertexCount = vertices.length / 4;

            const vertexBuffer = device.createBuffer({
                size: vertices.byteLength,
                usage: GPUBufferUsage.VERTEX | GPUBufferUsage.COPY_DST,
            });
            device.queue.writeBuffer(vertexBuffer, 0, vertices);

            const uniformBuffer = device.createBuffer({
                size: 16,
                usage: GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST,
            });

            const shaderModule = device.createShaderModule({
                code: shaderCode,
            });

            const pipeline = device.createRenderPipeline({
                layout: "auto",
                vertex: {
                    module: shaderModule,
                    entryPoint: "vs_main",
                    buffers: [
                        {
                            arrayStride: 16,
                            attributes: [
                                {
                                    shaderLocation: 0,
                                    offset: 0,
                                    format: "float32x4",
                                },
                            ],
                        },
                    ],
                },
                fragment: {
                    module: shaderModule,
                    entryPoint: "fs_main",
                    targets: [{ format }],
                },
                primitive: { topology: "triangle-list", cullMode: "none" },
                depthStencil: {
                    depthWriteEnabled: true,
                    depthCompare: "less",
                    format: "depth24plus",
                },
            });

            let depthTexture = device.createTexture({
                size: [canvas.width, canvas.height],
                format: "depth24plus",
                usage: GPUTextureUsage.RENDER_ATTACHMENT,
            });

            const bindGroup = device.createBindGroup({
                layout: pipeline.getBindGroupLayout(0),
                entries: [{ binding: 0, resource: { buffer: uniformBuffer } }],
            });

            const frame = () => {
                if (!canvas) return;

                if (
                    depthTexture.width !== canvas.width ||
                    depthTexture.height !== canvas.height
                ) {
                    depthTexture.destroy();
                    depthTexture = device.createTexture({
                        size: [canvas.width, canvas.height],
                        format: "depth24plus",
                        usage: GPUTextureUsage.RENDER_ATTACHMENT,
                    });
                }

                const { zoom, rotX, rotY } = cameraState.current;
                device.queue.writeBuffer(
                    uniformBuffer,
                    0,
                    new Float32Array([zoom, rotX, rotY, 0.0]),
                );

                const encoder = device.createCommandEncoder();
                const pass = encoder.beginRenderPass({
                    colorAttachments: [
                        {
                            view: context.getCurrentTexture().createView(),
                            loadOp: "clear",
                            storeOp: "store",
                            clearValue: { r: 0.1, g: 0.1, b: 0.1, a: 1 },
                        },
                    ],
                    depthStencilAttachment: {
                        view: depthTexture.createView(),
                        depthClearValue: 1.0,
                        depthLoadOp: "clear",
                        depthStoreOp: "store",
                    },
                });

                pass.setPipeline(pipeline);
                pass.setBindGroup(0, bindGroup);
                pass.setVertexBuffer(0, vertexBuffer);
                pass.draw(vertexCount);
                pass.end();

                device.queue.submit([encoder.finish()]);
                animationRef.current = requestAnimationFrame(frame);
            };

            frame();
        };

        init();

        return () => {
            cancelAnimationFrame(animationRef.current);
            window.removeEventListener("resize", resizeCanvas);
        };
    }, [module, ready, volumePtr, lods]);

    const handleMouseDown = (e: React.MouseEvent) => {
        isDragging.current = true;
        lastMousePos.current = { x: e.clientX, y: e.clientY };
    };

    const handleMouseMove = (e: React.MouseEvent) => {
        if (!isDragging.current) return;
        const deltaX = e.clientX - lastMousePos.current.x;
        const deltaY = e.clientY - lastMousePos.current.y;
        const speed = 0.01;
        cameraState.current.rotY += deltaX * speed;
        cameraState.current.rotX += deltaY * speed;
        lastMousePos.current = { x: e.clientX, y: e.clientY };
    };

    const handleMouseUp = () => (isDragging.current = false);

    const handleWheel = (e: React.WheelEvent) => {
        const zoomSpeed = 0.001;
        const newZoom = cameraState.current.zoom + e.deltaY * -zoomSpeed;
        cameraState.current.zoom = Math.max(0.01, newZoom);
    };

    return (
        <div className="p-4 w-full h-full">
            <canvas
                ref={canvasRef}
                className="w-full h-full rounded-md cursor-move"
                onMouseDown={handleMouseDown}
                onMouseMove={handleMouseMove}
                onMouseUp={handleMouseUp}
                onMouseLeave={handleMouseUp}
                onWheel={handleWheel}
            />
        </div>
    );
};
