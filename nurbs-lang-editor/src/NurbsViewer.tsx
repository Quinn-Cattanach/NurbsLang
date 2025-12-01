import { useRef, useEffect, useContext, useState } from "react";
import type { Nurbs3Ptr } from "./WasmContext";
import { WasmContext } from "./WasmContext";

type NurbsViewerProps = {
    volumePtr: Nurbs3Ptr;
    lods?: [number, number, number];
};

export const NurbsViewer = ({
    volumePtr,
    lods = [20, 20, 20],
}: NurbsViewerProps) => {
    const { module, ready } = useContext(WasmContext);
    const canvasRef = useRef<HTMLCanvasElement | null>(null);
    const animationRef = useRef<number>(0);
    const [zoom, setZoom] = useState(1);

    useEffect(() => {
        if (!module || !ready) return;
        const canvas = canvasRef.current;
        if (!canvas || !navigator.gpu) return;

        const resizeCanvas = () => {
            canvas.width = canvas.clientWidth;
            canvas.height = canvas.clientHeight;
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

            // Uniform buffer for zoom
            const uniformBuffer = device.createBuffer({
                size: 4,
                usage: GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST,
            });

            const shaderCode = `
struct VertexOutput {
  @builtin(position) Position : vec4<f32>,
};

@group(0) @binding(0) var<uniform> zoom : f32;

@vertex
fn vs_main(@location(0) position: vec3<f32>) -> VertexOutput {
  var out: VertexOutput;
  out.Position = vec4<f32>(position * zoom, 1.0);
  return out;
}

@fragment
fn fs_main() -> @location(0) vec4<f32> {
  return vec4<f32>(0.2, 0.7, 1.0, 1.0);
}
`;

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
                            arrayStride: 4 * 4,
                            attributes: [
                                {
                                    shaderLocation: 0,
                                    offset: 0,
                                    format: "float32x3",
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
                primitive: {
                    topology: "point-list",
                    cullMode: "none",
                },
            });

            const bindGroup = device.createBindGroup({
                layout: pipeline.getBindGroupLayout(0),
                entries: [{ binding: 0, resource: { buffer: uniformBuffer } }],
            });

            const frame = () => {
                if (!canvas) return;

                // Update zoom uniform
                device.queue.writeBuffer(
                    uniformBuffer,
                    0,
                    new Float32Array([zoom]),
                );

                const encoder = device.createCommandEncoder();
                const textureView = context.getCurrentTexture().createView();

                const pass = encoder.beginRenderPass({
                    colorAttachments: [
                        {
                            view: textureView,
                            loadOp: "clear",
                            storeOp: "store",
                            clearValue: { r: 0, g: 0, b: 0, a: 1 },
                        },
                    ],
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
    }, [module, ready, volumePtr, lods, zoom]);

    // Mouse wheel zoom
    useEffect(() => {
        const canvas = canvasRef.current;
        if (!canvas) return;

        const onWheel = (e: WheelEvent) => {
            e.preventDefault();
            setZoom((z) => Math.max(0.1, z + e.deltaY * -0.001));
        };

        canvas.addEventListener("wheel", onWheel);
        return () => canvas.removeEventListener("wheel", onWheel);
    }, []);

    return <canvas ref={canvasRef} className="w-full h-full rounded-md" />;
};
