# Engine structure

## What is currently missing?
- Way to specify what to draw (mesh, screen quad, lines?)
- Materials
- Order of object drawing / in what subpass -> material should know this
- Different buffers (e.g uniform buffers for lights / one per scene, uniform buffers for materials / colors, vertex & index buffers)

## Correct order
1. Renderpass with state: frame buffer, blending, depth/stencil
2. Shader with state: shaders, tesselation
3. Material with state: textures, uniforms
4. Object/Geometry with state: vertex/index buffer, matrices/uniforms


```
// Go through all objects to draw
-> Create vertex and index buffers
.Update(rCommandBuffer)

// Get material -> put in correct substep

// Go through all materials that should be used:
-> Create textures, descriptor pools and descriptor sets
.Update(rCommandBuffer)

// Go through substeps (correct order) and draw all pipelines / objects / part-objects they contain

```
