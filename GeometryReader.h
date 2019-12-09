#ifndef GMF_GEOMETRY_READER_H
#define GMF_GEOMETRY_READER_H

#include "libmeshb7.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include <vtkTypeInt32Array.h>


// If any geometry tags are saved read them and save them as point data
template<typename IndexType, typename FloatType>
void ReadGeometry(int64_t& InpMsh,vtkUnstructuredGrid* output)
{
    //Allocate enough Memory
    int64_t NmbOnVertex = GmfStatKwd(InpMsh, GmfVerticesOnGeometricVertices);
    int64_t NmbOnEdge   = GmfStatKwd(InpMsh, GmfVerticesOnGeometricEdges);
    int64_t NmbOnSurf   = GmfStatKwd(InpMsh, GmfVerticesOnGeometricTriangles);

    int64_t NmbTotal = NmbOnVertex +  NmbOnEdge +  NmbOnSurf;
    if(NmbTotal == 0)
        return;
    
    vtkSmartPointer<vtkTypeInt32Array> cad_info = vtkSmartPointer<vtkTypeInt32Array>::New();
    cad_info->SetNumberOfComponents(2);
    cad_info->SetNumberOfTuples( output->GetNumberOfPoints());
    cad_info->SetName("cad_info");

    struct CAD_Info
    {
         IndexType dim;
         IndexType id;
         CAD_Info()
         {  
             dim = id = -1;
         }
         
         CAD_Info(int dim_, int id_)
         {  
             dim = dim_;
             id = id_;
         }
    };
    
    IndexType v[2];
    FloatType params[3];
    int64_t last_index = 0;

    for(int64_t i = 0 ; i < cad_info->GetNumberOfTuples() ; i++)
        cad_info->SetTuple2(i,-1,-1);
    
    GmfGotoKwd(InpMsh,GmfVerticesOnGeometricVertices);
    for (int64_t i=0; i < NmbOnVertex; i++)
    {
        GmfGetLin(  InpMsh, GmfVerticesOnGeometricVertices,&v[0],&v[1]);
        cad_info->SetTuple2(v[0] - 1 , 0, v[1]);
    }
    GmfGotoKwd(InpMsh,GmfVerticesOnGeometricEdges);
    for (int64_t i=0; i < NmbOnEdge; i++)
    {
        GmfGetLin(  InpMsh, GmfVerticesOnGeometricEdges,&v[0],&v[1],&params[0],&params[1]);
        cad_info->SetTuple2(v[0] - 1 , 1, v[1]);
    }
    GmfGotoKwd(InpMsh,GmfVerticesOnGeometricTriangles);
    for (int64_t i=0; i < NmbOnSurf; i++)
    {
        GmfGetLin(  InpMsh, GmfVerticesOnGeometricTriangles,&v[0],&v[1],&params[0],&params[1],&params[2]);
        cad_info->SetTuple2(v[0] - 1 , 2, v[1]);
    }

    output->GetPointData()->AddArray(cad_info);
}
#endif
