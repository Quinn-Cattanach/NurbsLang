struct Uniforms {
    time: f32,
    fov: f32,
    padding: vec2<f32>,
    viewMatrix: mat4x4<f32>,
    projMatrix: mat4x4<f32>,
    invViewProj: mat4x4<f32>,
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;

struct NURBSPatch {
    degU: f32,
    degV: f32,
    numCtrlU: f32,
    numCtrlV: f32,
    offsetCtrl: f32,
    uMin: f32,
    uMax: f32,
    vMin: f32,
    vMax: f32,
    uSamples: f32,
    vSamples: f32,
    padding: f32,
};

@group(0) @binding(1) var<storage, read> patches: array<NURBSPatch>;
@group(0) @binding(2) var<storage, read> weightedControlPoints: array<vec4<f32>>;
@group(0) @binding(3) var basisUTex: texture_2d<f32>;
@group(0) @binding(4) var basisVTex: texture_2d<f32>;
@group(0) @binding(5) var basisUDerivTex: texture_2d<f32>;
@group(0) @binding(6) var basisVDerivTex: texture_2d<f32>;
@group(0) @binding(7) var basisSampler: sampler;
@group(0) @binding(8) var outTex: texture_storage_2d<rgba8unorm, write>;

const MAX_ITERATIONS: u32 = 8u;
const CONVERGENCE_EPSILON: f32 = 0.0001;

struct Ray {
    origin: vec3<f32>,
    direction: vec3<f32>,
    n1: vec3<f32>,
    d1: f32,
    n2: vec3<f32>,
    d2: f32,
};

struct HitInfo {
    hit: bool,
    t: f32,
    u: f32,
    v: f32,
    position: vec3<f32>,
    normal: vec3<f32>,
};
// Sample basis function from pre-computed texture
fn sampleBasisFunction(param: f32, basisIdx: u32, patchIdx: u32, isU: bool) -> f32 {
    let p = patches[patchIdx];
    
    var paramMin: f32;
    var paramMax: f32;
    var numSamples: f32;
    
    if (isU) {
        paramMin = p.uMin;
        paramMax = p.uMax;
        numSamples = p.uSamples;
    } else {
        paramMin = p.vMin;
        paramMax = p.vMax;
        numSamples = p.vSamples;
    }
    
    // Normalize parameter to [0, 1]
    let t = clamp((param - paramMin) / (paramMax - paramMin), 0.0, 1.0);
    
    // Sample from appropriate texture based on isU
    var dims: vec2<u32>;
    var value: f32;
    
    if (isU) {
        dims = textureDimensions(basisUTex);
        let x = i32(t * f32(dims.x - 1u));
        let y = i32(basisIdx);
        value = textureLoad(basisUTex, vec2<i32>(x, y), 0).r;
    } else {
        dims = textureDimensions(basisVTex);
        let x = i32(t * f32(dims.x - 1u));
        let y = i32(basisIdx);
        value = textureLoad(basisVTex, vec2<i32>(x, y), 0).r;
    }
    
    return value;
}

// Sample basis function derivative
fn sampleBasisDerivative(param: f32, basisIdx: u32, patchIdx: u32, isU: bool) -> f32 {
    let p = patches[patchIdx];
    
    var paramMin: f32;
    var paramMax: f32;
    var numSamples: f32;
    
    if (isU) {
        paramMin = p.uMin;
        paramMax = p.uMax;
        numSamples = p.uSamples;
    } else {
        paramMin = p.vMin;
        paramMax = p.vMax;
        numSamples = p.vSamples;
    }
    
    let t = clamp((param - paramMin) / (paramMax - paramMin), 0.0, 1.0);
    
    var dims: vec2<u32>;
    var value: f32;
    
    if (isU) {
        dims = textureDimensions(basisUDerivTex);
        let x = i32(t * f32(dims.x - 1u));
        let y = i32(basisIdx);
        value = textureLoad(basisUDerivTex, vec2<i32>(x, y), 0).r;
    } else {
        dims = textureDimensions(basisVDerivTex);
        let x = i32(t * f32(dims.x - 1u));
        let y = i32(basisIdx);
        value = textureLoad(basisVDerivTex, vec2<i32>(x, y), 0).r;
    }
    
    return value;
}

// Evaluate NURBS surface at (u,v)
fn evaluateSurface(patchIdx: u32, u: f32, v: f32) -> vec3<f32> {
    let p = patches[patchIdx];
    var sum = vec3<f32>(0.0);
    var denom = 0.0;
    
    let numCtrlU = u32(p.numCtrlU) + 1u;
    let numCtrlV = u32(p.numCtrlV) + 1u;
    
    for (var i = 0u; i < numCtrlU; i++) {
        let Nu = sampleBasisFunction(u, i, patchIdx, true);
        
        for (var j = 0u; j < numCtrlV; j++) {
            let Nv = sampleBasisFunction(v, j, patchIdx, false);
            let idx = u32(p.offsetCtrl) + j * numCtrlU + i;
            let cp = weightedControlPoints[idx];
            
            let w = cp.w * Nu * Nv;
            sum += w * cp.xyz;
            denom += w;
        }
    }
    
    if (abs(denom) < 0.0001) {
        return vec3<f32>(0.0);
    }
    
    return sum / denom;
}

// Evaluate surface with derivatives
fn evaluateSurfaceWithDerivatives(patchIdx: u32, u: f32, v: f32) -> array<vec3<f32>, 3> {
    let p = patches[patchIdx];
    var S = vec3<f32>(0.0);
    var Su = vec3<f32>(0.0);
    var Sv = vec3<f32>(0.0);
    var denom = 0.0;
    var denomU = 0.0;
    var denomV = 0.0;
    
    let numCtrlU = u32(p.numCtrlU) + 1u;
    let numCtrlV = u32(p.numCtrlV) + 1u;
    
    for (var i = 0u; i < numCtrlU; i++) {
        let Nu = sampleBasisFunction(u, i, patchIdx, true);
        let dNu = sampleBasisDerivative(u, i, patchIdx, true);
        
        for (var j = 0u; j < numCtrlV; j++) {
            let Nv = sampleBasisFunction(v, j, patchIdx, false);
            let dNv = sampleBasisDerivative(v, j, patchIdx, false);
            
            let idx = u32(p.offsetCtrl) + j * numCtrlU + i;
            let cp = weightedControlPoints[idx];
            
            let w = cp.w * Nu * Nv;
            let wu = cp.w * dNu * Nv;
            let wv = cp.w * Nu * dNv;
            
            S += w * cp.xyz;
            Su += wu * cp.xyz;
            Sv += wv * cp.xyz;
            denom += w;
            denomU += wu;
            denomV += wv;
        }
    }
    
    var result: array<vec3<f32>, 3>;
    if (abs(denom) > 0.0001) {
        result[0] = S / denom;
        result[1] = (Su * denom - S * denomU) / (denom * denom);
        result[2] = (Sv * denom - S * denomV) / (denom * denom);
    }
    
    return result;
}

// Create ray from pixel coordinates
fn createRay(pixelPos: vec2<f32>, screenSize: vec2<f32>) -> Ray {
    let ndc = vec2<f32>(
        (2.0 * pixelPos.x) / screenSize.x - 1.0,
        1.0 - (2.0 * pixelPos.y) / screenSize.y
    );
    
    let nearPoint = uniforms.invViewProj * vec4<f32>(ndc, -1.0, 1.0);
    let farPoint = uniforms.invViewProj * vec4<f32>(ndc, 1.0, 1.0);
    
    let origin = nearPoint.xyz / nearPoint.w;
    let farPos = farPoint.xyz / farPoint.w;
    let direction = normalize(farPos - origin);
    
    var ray: Ray;
    ray.origin = origin;
    ray.direction = direction;
    
    var up = vec3<f32>(0.0, 1.0, 0.0);
    if (abs(dot(direction, up)) > 0.9) {
        up = vec3<f32>(1.0, 0.0, 0.0);
    }
    
    ray.n1 = normalize(cross(direction, up));
    ray.d1 = -dot(ray.n1, origin);
    
    ray.n2 = normalize(cross(direction, ray.n1));
    ray.d2 = -dot(ray.n2, origin);
    
    return ray;
}

fn rayPatchEquation(ray: Ray, S: vec3<f32>) -> vec2<f32> {
    return vec2<f32>(
        dot(ray.n1, S) + ray.d1,
        dot(ray.n2, S) + ray.d2
    );
}

fn intersectPatch(ray: Ray, patchIdx: u32, initialU: f32, initialV: f32) -> HitInfo {
    var hit: HitInfo;
    hit.hit = false;
    
    let p = patches[patchIdx];
    var u = clamp(initialU, p.uMin, p.uMax);
    var v = clamp(initialV, p.vMin, p.vMax);
    
    for (var iter = 0u; iter < MAX_ITERATIONS; iter++) {
        let eval = evaluateSurfaceWithDerivatives(patchIdx, u, v);
        let S = eval[0];
        let Su = eval[1];
        let Sv = eval[2];
        
        let F = rayPatchEquation(ray, S);
        
        let error = length(F);
        if (error < CONVERGENCE_EPSILON) {
            hit.hit = true;
            hit.u = u;
            hit.v = v;
            hit.position = S;
            hit.t = length(S - ray.origin);
            hit.normal = normalize(cross(Su, Sv));
            return hit;
        }
        
        let J11 = dot(ray.n1, Su);
        let J12 = dot(ray.n1, Sv);
        let J21 = dot(ray.n2, Su);
        let J22 = dot(ray.n2, Sv);
        
        let det = J11 * J22 - J12 * J21;
        if (abs(det) < 0.0001) {
            break;
        }
        
        let invDet = 1.0 / det;
        let du = -(J22 * invDet * F.x - J12 * invDet * F.y);
        let dv = -(-J21 * invDet * F.x + J11 * invDet * F.y);
        
        u = clamp(u + du, p.uMin, p.uMax);
        v = clamp(v + dv, p.vMin, p.vMax);
    }
    
    return hit;
}

@compute @workgroup_size(8, 8)
fn main(@builtin(global_invocation_id) gid: vec3<u32>) {
    let dims = textureDimensions(outTex);
    
    if (gid.x >= dims.x || gid.y >= dims.y) {
        return;
    }
    
    let pos = vec2<f32>(gid.xy);
    let ray = createRay(pos + vec2<f32>(0.5), vec2<f32>(dims));
    
    var color = vec3<f32>(0.1, 0.1, 0.15);
    var closestT = 1e10;
    
    let numPatches = arrayLength(&patches);
    for (var i = 0u; i < numPatches; i++) {
        let p = patches[i];
        let uMid = (p.uMin + p.uMax) * 0.5;
        let vMid = (p.vMin + p.vMax) * 0.5;
        
        let hit = intersectPatch(ray, i, uMid, vMid);
        
        if (hit.hit && hit.t < closestT) {
            closestT = hit.t;
            
            let lightDir = normalize(vec3<f32>(1.0, 1.0, 1.0));
            let diffuse = max(dot(hit.normal, lightDir), 0.0);
            let ambient = 0.3;
            
            color = vec3<f32>(0.7, 0.5, 0.3) * (ambient + 0.7 * diffuse);
        }
    }
    
    textureStore(outTex, vec2<i32>(gid.xy), vec4<f32>(color, 1.0));
}