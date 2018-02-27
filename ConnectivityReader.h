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
    int64_t NmbTet   = GmfStatKwd(InpMsh, GmfTetrahedra);
    output->Allocate(NmbTri + NmbTet);

    vtkSmartPointer<vtkTypeInt64Array> reference = vtkSmartPointer<vtkTypeInt64Array>::New();
    reference->SetNumberOfComponents(1);
    reference->SetNumberOfTuples(NmbTri + NmbTet);
    reference->SetName("ref");
    
    IndexType v0,v1,v2,v3,rt;
    
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
        reference->InsertTuple1(i,rt);
    }
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
        reference->InsertTuple1(NmbTri + i,rt);
    }
    output->GetCellData()->AddArray(reference);
}
#endif
