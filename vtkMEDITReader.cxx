#include "vtkMEDITReader.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"

#include "vtkUnstructuredGrid.h"
#include "vtkCellType.h"
#include "vtkCellArray.h"
#include "vtkSmartPointer.h"

#include "libmeshb7.h"



vtkStandardNewMacro(vtkMEDITReader);

// Construct object with merging set to true.
vtkMEDITReader::vtkMEDITReader()
{
    this->FileName = NULL;

    this->SetNumberOfInputPorts(0);
}


vtkMEDITReader::~vtkMEDITReader()
{
    delete [] this->FileName;
}

int vtkMEDITReader::RequestData( vtkInformation *vtkNotUsed(request), 
                                 vtkInformationVector **vtkNotUsed(inputVector), 
                                 vtkInformationVector *outputVector)
{
    // get the info object
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the ouptut
    vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT())); 

    if (!this->FileName)
    {
        vtkErrorMacro(<<"A File Name must be specified.");
        return 0;
    }

    // open a MEDIT file for reading
    int64_t InpMsh;
    int version,dim;
    InpMsh = GmfOpenMesh(this->FileName, GmfRead, &version, &dim);
    if(InpMsh == 0)
    {
        vtkWarningMacro(<<"Could not open MEDIT file");
        return 0;
    }
    if( dim != 3)
    {
        vtkErrorMacro(<<"Unsupported mesh dimension " << dim );
        return 0;
    }
    if( (version !=2 ) and (version != 3) and (version !=4) )
    {
        vtkErrorMacro(<<"Unsupported mesh version " << version );
        return 0;
    }
    

    vtkDebugMacro(<<"File Opened");
    // Read the number of vertices
    int64_t NmbVer = GmfStatKwd(InpMsh, GmfVertices);

    // Set up point container
    vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
    pts->SetDataTypeToDouble();
    pts->SetNumberOfPoints(NmbVer);

    double p[3];
    int rt; //reference , not in use in this module
    
    // Read the vertices
    GmfGotoKwd(InpMsh, GmfVertices);
    for(int64_t i=0;i<NmbVer;i++)
    {
        GmfGetLin(  InpMsh, GmfVertices, &p[0], &p[1], &p[2],&rt);
        vtkDebugMacro(<<p[0] << " " <<p[1] << " " << p[2]);
        pts->SetPoint(i,p);
    }
    output->SetPoints(pts);

    // Get Number of Triangles and Tetrahedra
    int64_t NmbTri = GmfStatKwd(InpMsh, GmfTriangles);
    int64_t NmbTet = GmfStatKwd(InpMsh, GmfTetrahedra);

    output->Allocate(NmbTri + NmbTet);
    
    vtkSmartPointer<vtkIdList> pointIds = vtkSmartPointer<vtkIdList>::New();
    int64_t v0,v1,v2,v3;

    // Read triangles
    GmfGotoKwd(InpMsh, GmfTriangles);
    pointIds->SetNumberOfIds(3);
    for (int64_t i=0; i < NmbTri; i++)
    {
        GmfGetLin(  InpMsh, GmfTriangles, &v0, &v1,&v2,&rt);
        vtkDebugMacro(<<v0 << " " <<v1 << " " << v2);

        // mesh/meshb are one based
        pointIds->SetId(0,v0-1);
        pointIds->SetId(1,v1-1);
        pointIds->SetId(2,v2-1);
        output->InsertNextCell(VTK_TRIANGLE,pointIds);
    }
    
    GmfGotoKwd(InpMsh, GmfTetrahedra);
    pointIds->SetNumberOfIds(4);
    for (int64_t i=0; i < NmbTet; i++)
    {
        GmfGetLin(  InpMsh, GmfTetrahedra, &v0, &v1,&v2,&v3,&rt);
        vtkDebugMacro(<<v0 << " " <<v1 << " " << v2 << " " << v3);
        // mesh/meshb are one based
        pointIds->SetId(0,v0-1);
        pointIds->SetId(1,v1-1);
        pointIds->SetId(2,v2-1);
        pointIds->SetId(3,v3-1);
        output->InsertNextCell(VTK_TETRA,pointIds);
    }

    // Close the mesh
    GmfCloseMesh(InpMsh);

    return 1;
}

void vtkMEDITReader::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    os << indent << "File Name: "
        << (this->FileName ? this->FileName : "(none)") << "\n";
}
