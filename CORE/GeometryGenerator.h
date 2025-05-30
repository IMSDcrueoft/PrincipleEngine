#pragma once
#ifndef GEOMETRYGENERATOR_H
#define GEOMETRYGENERATOR_H

#include <cstdint>
#include <DirectXMath.h>
#include <vector>
#include "ModelData.h"

class GeometryGenerator:public ModelData
{
public:
	///<summary>
	/// Creates a box centered at the origin with the given dimensions, where each
    /// face has m rows and n columns of vertices.
	///</summary>
    MeshData CreateBox(float width, float height, float depth, uint32_t numSubdivisions);

	///<summary>
	/// Creates a sphere centered at the origin with the given radius.  The
	/// slices and stacks parameters control the degree of tessellation.
	///</summary>
    MeshData CreateSphere(float radius, uint32_t sliceCount, uint32_t stackCount);

	///<summary>
	/// Creates a geosphere centered at the origin with the given radius.  The
	/// depth controls the level of tessellation.
	///</summary>
    MeshData CreateGeosphere(float radius, uint32_t numSubdivisions);

	///<summary>
	/// Creates a cylinder parallel to the y-axis, and centered about the origin.  
	/// The bottom and top radius can vary to form various cone shapes rather than true
	// cylinders.  The slices and stacks parameters control the degree of tessellation.
	///</summary>
    MeshData CreateCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount);

	///<summary>
	/// Creates an mxn grid in the xz-plane with m rows and n columns, centered
	/// at the origin with the specified width and depth.
	///</summary>
    MeshData CreateGrid(float width, float depth, uint32_t m, uint32_t n);

	///<summary>
	/// Creates a quad aligned with the screen.  This is useful for postprocessing and screen effects.
	///</summary>
    MeshData CreateQuad(float x, float y, float w, float h, float depth);

private:
	void Subdivide(MeshData& meshData);
    Vertex MidPoint(const Vertex& v0, const Vertex& v1);
    void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, MeshData& meshData);
    void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, MeshData& meshData);
};

#endif // !GEOMETRYGENERATOR