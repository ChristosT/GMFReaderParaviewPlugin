#ifndef GMF_CONNECTIVITY_READER_H
#define GMF_CONNECTIVITY_READER_H

#include "libmeshb7.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellType.h"
#include "vtkCellData.h"
#include <vtkTypeInt64Array.h>


// Like in refine we assume that the orientation of the elements
// follows the fules of UGRID
// http://www.simcenter.msstate.edu/software/downloads/doc/ug_io/3d_input_output_grids.html
// we reorder the nodes to match the VTK conventions
// https://www.vtk.org/wp-content/uploads/2015/04/file-formats.pdf
template< typename IndexType>
void tri_from_ugrid(IndexType* in, vtkIdType* out)
{
    // just copy
    out[0] = in[0]; 
    out[1] = in[1]; 
    out[2] = in[2]; 
}
template< typename IndexType>
 void quad_from_ugrid(IndexType* in, vtkIdType* out)
{
    // just copy
    out[0] = in[0]; 
    out[1] = in[1]; 
    out[2] = in[2]; 
    out[3] = in[3]; 
}
template< typename IndexType>
 void tet_from_ugrid(IndexType* in, vtkIdType* out)
{
    // just copy
    out[0] = in[0]; 
    out[1] = in[1]; 
    out[2] = in[2]; 
    out[3] = in[3]; 
}

template< typename IndexType>
  void pyramid_from_ugrid(IndexType* in, vtkIdType* out)
{
    out[0] = in[3];
    out[1] = in[4];
    out[2] = in[1];
    out[3] = in[0];
    out[4] = in[2];
}
template< typename IndexType>
  void prism_from_ugrid(IndexType* in, vtkIdType* out)
{

    out[0] = in[1]; 
    out[1] = in[0]; 
    out[2] = in[2]; 

    out[3] = in[4]; 
    out[4] = in[3]; 
    out[5] = in[5]; 
}
template< typename IndexType>
 void hex_from_ugrid(IndexType* in, vtkIdType* out)
{
    // just copy
    out[0] = in[0]; 
    out[1] = in[1]; 
    out[2] = in[2]; 
    out[3] = in[3]; 
                  
    out[4] = in[4]; 
    out[5] = in[5]; 
    out[6] = in[6]; 
    out[7] = in[7]; 
}

template<typename IndexType>
void ReadConnectivity(int64_t& InpMsh,vtkUnstructuredGrid* output)
{
    //Allocate enough Memory
    int64_t NmbTri   = GmfStatKwd(InpMsh, GmfTriangles);
    int64_t NmbQuad  = GmfStatKwd(InpMsh, GmfQuadrilaterals);
    int64_t NmbTet   = GmfStatKwd(InpMsh, GmfTetrahedra);
    int64_t NmbPrism   = GmfStatKwd(InpMsh, GmfPrisms);
    int64_t NmbPyramid   = GmfStatKwd(InpMsh, GmfPyramids);
    int64_t NmbHex       = GmfStatKwd(InpMsh, GmfHexahedra);
    output->Allocate(NmbTri + NmbQuad + NmbTet + NmbPrism + NmbPyramid + NmbHex);

    vtkSmartPointer<vtkTypeInt64Array> reference = vtkSmartPointer<vtkTypeInt64Array>::New();
    reference->SetNumberOfComponents(1);
    reference->SetNumberOfTuples(NmbTri + NmbQuad + NmbTet + NmbPrism + NmbPyramid +NmbHex);
    reference->SetName("ref");
    
    IndexType v[8],rt;
    int64_t last_index = 0;
    
    GmfGotoKwd(InpMsh, GmfTriangles);
    vtkIdType tria[3];

    for (int64_t i=0; i < NmbTri; i++)
    {
        GmfGetLin(  InpMsh, GmfTriangles, &v[0], &v[1],&v[2],&rt);

        tri_from_ugrid(v,tria);
        // mesh/meshb are one based
        for(int j = 0 ; j < 3 ; j++) tria[j]--;
        output->InsertNextCell(VTK_TRIANGLE,3,tria);
        reference->InsertTuple1(last_index + i,rt);
    }
    last_index = NmbTri;

    GmfGotoKwd(InpMsh, GmfQuadrilaterals);
    vtkIdType quad[4];

    for (int64_t i=0; i < NmbQuad; i++)
    {
        GmfGetLin(  InpMsh, GmfQuadrilaterals, &v[0], &v[1],&v[2],&v[3],&rt);
        
	    quad_from_ugrid(v,tria);
        // mesh/meshb are one based
        for(int j = 0 ; j < 4 ; j++) quad[j]--;
        output->InsertNextCell(VTK_QUAD,4,quad);
        reference->InsertTuple1(last_index + i,rt);
    }
    last_index += NmbQuad;

    GmfGotoKwd(InpMsh, GmfTetrahedra);
    vtkIdType tet[4];

    for (int64_t i=0; i < NmbTet; i++)
    {
        GmfGetLin(  InpMsh, GmfTetrahedra, &v[0], &v[1],&v[2],&v[3],&rt);
        
        tet_from_ugrid(v,tet);
        // mesh/meshb are one based
        for(int j = 0 ; j < 4 ; j++) tet[j]--;
        output->InsertNextCell(VTK_TETRA,4,tet);
        reference->InsertTuple1(last_index + i,rt);
    }
    last_index += NmbTet;


    GmfGotoKwd(InpMsh, GmfPrisms);
    vtkIdType prism[6];

    for (int64_t i=0; i < NmbPrism; i++)
    {
        GmfGetLin(  InpMsh, GmfPrisms, &v[0], &v[1],&v[2],&v[3],&v[4],&v[5],&rt);
        
        prism_from_ugrid(v,prism);
        // mesh/meshb are one based
        for(int j = 0 ; j < 6 ; j++) prism[j]--;
        output->InsertNextCell(VTK_WEDGE,6,prism);
        reference->InsertTuple1(last_index + i,rt);
    }
    last_index += NmbPrism;

    GmfGotoKwd(InpMsh, GmfPyramids);
    vtkIdType pyramid[5];

    for (int64_t i=0; i < NmbPyramid; i++)
    {
        GmfGetLin(  InpMsh, GmfPyramids, &v[0], &v[1],&v[2],&v[3],&v[4],&rt);
        
        pyramid_from_ugrid(v,pyramid);
        // mesh/meshb are one based
        for(int j = 0 ; j < 5 ; j++) pyramid[j]--;
        output->InsertNextCell(VTK_PYRAMID,5,pyramid);
        reference->InsertTuple1(last_index + i,rt);
    }
    last_index += NmbPyramid;

    GmfGotoKwd(InpMsh, GmfHexahedra);
    vtkIdType hexahedron[8];
    
    for (int64_t i=0; i < NmbHex; i++)
    {
        GmfGetLin(  InpMsh, GmfHexahedra, &v[0], &v[1],&v[2],&v[3],&v[4],&v[5],&v[6],&v[7],&rt);
        
        hex_from_ugrid(v,hexahedron);
        // mesh/meshb are one based
        for(int j = 0 ; j < 8 ; j++) hexahedron[j]--;
        output->InsertNextCell(VTK_HEXAHEDRON,8,hexahedron);
        reference->InsertTuple1(last_index + i,rt);
    }
    last_index += NmbHex;


    output->GetCellData()->AddArray(reference);
}
#endif
