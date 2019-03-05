#ifndef GMF_CONNECTIVITY_READER_H
#define GMF_CONNECTIVITY_READER_H

#include "libmeshb7.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellType.h"
#include "vtkCellData.h"
#include <vtkTypeInt64Array.h>

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
    
    IndexType v0,v1,v2,v3,v4,v5,v6,v7,rt;
    int64_t last_index = 0;
    
    GmfGotoKwd(InpMsh, GmfTriangles);
    vtkIdType tria[3];

    for (int64_t i=0; i < NmbTri; i++)
    {
        GmfGetLin(  InpMsh, GmfTriangles, &v0, &v1,&v2,&rt);

        // mesh/meshb are one based
        tria[0] = v0-1;
        tria[1] = v1-1;
        tria[2] = v2-1;
        output->InsertNextCell(VTK_TRIANGLE,3,tria);
        reference->InsertTuple1(last_index + i,rt);
    }
    last_index = NmbTri;

    GmfGotoKwd(InpMsh, GmfQuadrilaterals);
    vtkIdType quad[4];

    for (int64_t i=0; i < NmbQuad; i++)
    {
        GmfGetLin(  InpMsh, GmfQuadrilaterals, &v0, &v1,&v2,&v3,&rt);
        
        // mesh/meshb are one based
        quad[0] = v0-1;
        quad[1] = v1-1;
        quad[2] = v2-1;
        quad[3] = v3-1;
        output->InsertNextCell(VTK_QUAD,4,quad);
        reference->InsertTuple1(last_index + i,rt);
    }
    last_index += NmbQuad;

    GmfGotoKwd(InpMsh, GmfTetrahedra);
    vtkIdType tet[4];

    for (int64_t i=0; i < NmbTet; i++)
    {
        GmfGetLin(  InpMsh, GmfTetrahedra, &v0, &v1,&v2,&v3,&rt);
        
        // mesh/meshb are one based
        tet[0] = v0-1;
        tet[1] = v1-1;
        tet[2] = v2-1;
        tet[3] = v3-1;
        output->InsertNextCell(VTK_TETRA,4,tet);
        reference->InsertTuple1(last_index + i,rt);
    }
    last_index += NmbTet;


    GmfGotoKwd(InpMsh, GmfPrisms);
    vtkIdType prism[6];

    for (int64_t i=0; i < NmbPrism; i++)
    {
        GmfGetLin(  InpMsh, GmfPrisms, &v0, &v1,&v2,&v3,&v4,&v5,&rt);
        
        // mesh/meshb are one based
        prism[0] = v0-1;
        prism[1] = v1-1;
        prism[2] = v2-1;
        prism[3] = v3-1;
        prism[4] = v4-1;
        prism[5] = v5-1;
        output->InsertNextCell(VTK_WEDGE,6,prism);
        reference->InsertTuple1(last_index + i,rt);
    }
    last_index += NmbPrism;

    GmfGotoKwd(InpMsh, GmfPyramids);
    vtkIdType pyramid[5];

    for (int64_t i=0; i < NmbPyramid; i++)
    {
        GmfGetLin(  InpMsh, GmfPyramids, &v0, &v1,&v2,&v3,&v4,&rt);
        
        // mesh/meshb are one based
        pyramid[0] = v0-1;
        pyramid[1] = v1-1;
        pyramid[2] = v2-1;
        pyramid[3] = v3-1;
        pyramid[4] = v4-1;
        output->InsertNextCell(VTK_PYRAMID,5,pyramid);
        reference->InsertTuple1(last_index + i,rt);
    }
    last_index += NmbPyramid;

    GmfGotoKwd(InpMsh, GmfHexahedra);
    vtkIdType hexahedron[8];
    
    for (int64_t i=0; i < NmbHex; i++)
    {
        GmfGetLin(  InpMsh, GmfHexahedra, &v0, &v1,&v2,&v3,&v4,&v5,&v6,&v7,&rt);
        
        // mesh/meshb are one based
        hexahedron[0] = v0-1;
        hexahedron[1] = v1-1;
        hexahedron[2] = v2-1;
        hexahedron[3] = v3-1;
        hexahedron[4] = v4-1;
        hexahedron[5] = v5-1;
        hexahedron[6] = v6-1;
        hexahedron[7] = v7-1;
        output->InsertNextCell(VTK_HEXAHEDRON,8,hexahedron);
        reference->InsertTuple1(last_index + i,rt);
    }
    last_index += NmbHex;


    output->GetCellData()->AddArray(reference);
}
#endif
