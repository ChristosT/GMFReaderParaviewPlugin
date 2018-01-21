#include "vtkMEDITReader.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"

#include "vtkPolyData.h"
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

    int vtkMEDITReader::RequestData(
                                    vtkInformation *vtkNotUsed(request),
                                    vtkInformationVector **vtkNotUsed(inputVector),
                                    vtkInformationVector *outputVector)
{
    // get the info object
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the ouptut
    vtkPolyData *output = vtkPolyData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT())); 

    if (!this->FileName)
    {
        vtkErrorMacro(<<"A File Name must be specified.");
        return 0;
    }

    // open a MEDIT file for reading
    long long InpMsh;
    int ver,dim;
    InpMsh = GmfOpenMesh(this->FileName, GmfRead, &ver, &dim);
    if(InpMsh == 0)
    {
        vtkWarningMacro(<<"Could not open MEDIT file");
        return 0;
    }
    vtkDebugMacro(<<"File Opened");
    // Read the number of vertices
    int NmbVer = GmfStatKwd(InpMsh, GmfVertices);
    // Set up point container
    vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
    pts->SetDataTypeToDouble();
    pts->SetNumberOfPoints(NmbVer);

    // Read the vertices
    vtkDebugMacro(<<"Read Vertices");
    GmfGotoKwd(InpMsh, GmfVertices);
    double p[3];
    int rt; //reference , not in use in this module
    for(int i=0;i<NmbVer;i++)
    {
        GmfGetLin(  InpMsh, GmfVertices, &p[0], &p[1], &p[2],&rt);
        //vtkDebugMacro(<<p[0] << " " <<p[1] << " " << p[2]);
        pts->SetPoint(i,p);
    }
    output->SetPoints(pts);
    vtkDebugMacro(<<"DONE");

    // Get Number of Triangles
    int NmbTri = GmfStatKwd(InpMsh, GmfTriangles);
    vtkDebugMacro(<<NmbTri);
    // Read triangles
    GmfGotoKwd(InpMsh, GmfTriangles);
    vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
    vtkIdType vtkVerts[3]; // connectivity information of a single triangle
    int verts[3];
    for (int i=0; i < NmbTri; i++)
    {
        GmfGetLin(  InpMsh, GmfTriangles, &verts[0], &verts[1],&verts[2],&rt);
        // mesh/meshb are one based
        //vtkDebugMacro(<<verts[0] << " " <<verts[1] << " " << verts[2]);
        vtkVerts[0] = verts[0] -1;
        vtkVerts[1] = verts[1] -1;
        vtkVerts[2] = verts[2] -1;
        //vtkDebugMacro(<<vtkVerts[0] << " " <<vtkVerts[1] << " " << vtkVerts[2]);
        polys->InsertNextCell(3,vtkVerts);
    }
    output->SetPolys(polys);

    vtkDebugMacro( <<"Read: " << NmbVer << " points, " << NmbTri << " triangles"); 

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
