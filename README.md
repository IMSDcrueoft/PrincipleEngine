# PrincipleEngine
[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/IMSDcrueoft/PrincipleEngine)

This is my graduation project in college, and it's full of miscellaneous tools.

# introduction

## Project Structure
PrincipleEngine is a comprehensive graphics engine that integrates 3D and 2D rendering capabilities with a robust mathematical foundation. The project is organized into several key modules:

```
PrincipleEngine/  
├── CORE/            # 3D rendering core  
├── CORE2D/          # 2D rendering and UI  
├── ExternalTool/    # Third-party utilities  
├── Math/            # Mathematical library  
└── Shaders/         # Shader programs  
```

## 3D Rendering System (CORE)
The 3D rendering system is built on DirectX 12, providing modern GPU-accelerated graphics capabilities.

### Key Components:
- Rendering Pipeline: Implements a complete DirectX 12 rendering pipeline with swap chains, command queues, and descriptors
- Camera System: Handles view transformations and projections
- Model Loading: Supports loading and rendering 3D models
- Texture Management: Includes DDS texture loading functionality
- Shadow Mapping: Implements dynamic shadow mapping techniques

## 2D Graphics System (CORE2D)
The 2D rendering system is built on Direct2D, providing efficient 2D graphics and UI capabilities.

### Key Components:
- 2D Primitives: Drawing and filling of basic shapes like circles, rectangles, and lines
- UI Framework: Button, panel, label, textbox components for creating user interfaces
- Sprite Management: Sprite sheet handling for 2D game development

## Math Library
The math library provides optimized implementations of essential mathematical operations for 3D and 2D graphics.

### Key Components:
- Vector and Matrix Operations: Comprehensive implementation of 4D vectors and 4x4 matrices
- SIMD Optimization: Uses AVX256 instructions for high-performance calculations
- Transformation Functions: Matrix creation for rotations, scaling, translations
- Utility Functions: Normalization, cross product, dot product, and other vector operations

## Shaders
- BlinnPhong and CookTorranceBRDF with Metalness
- Bloom & ACES

## ModleLoader
- Load .obj(wavefront) and .pcm model file.

## PngProcessingTools
Configurable parameter-configurable image processing methods.

- Scale default: Edge-Adaptive Scaling, Edge Detection & Adaptive Weighting & Threshold Control & RCAS Enhancement
- Scale bicubic
- Sharpening (Laplace and Gauss-Laplace)
- ACES Tone mapping
- Grayscale conversion
- Color inversion
- Vibrance adjustment
- Binary, quaternary, and hexatical
- Surface blur
- Sobel edge enhancement
- Mosaic pixelation
- Image blending
- Pixel to RGB 3x3 conversion
- Interlaced
- XOR based encryption: Creates unique encryption pattern based on image dimensions
- HSL adjustments

# Project status
It's basically stagnant, and if I continue to work on graphics APIs in the future, then I will move to Vulkan.
