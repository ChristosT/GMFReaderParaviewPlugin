#include "vtkGMFReader.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"

#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkSmartPointer.h"

#include "libmeshb7.h"
#include "ConnectivityReader.h"

vtkStandardNewMacro(vtkGMFReader);

// Construct object with merging set to true.
vtkGMFReader::vtkGMFReader()
{
    this->MeshFile = NULL;
    this->SolutionFile = NULL;

    this->SetNumberOfInputPorts(0);
}


vtkGMFReader::~vtkGMFReader()
{
    //delete [] this->MeshFile;
    //delete [] this->SolutionFile;
}
/*
int vtkGMFReader::FillOutputPortInformation( int port, vtkInformation* info )
{
    if ( port == 0 )
    {
        info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkUnstructuredGrid" );

        return 1;
    }

    return 0;
}
*/

int vtkGMFReader::RequestData( vtkInformation *vtkNotUsed(request), 
                                 vtkInformationVector **vtkNotUsed(inputVector), 
                                 vtkInformationVector *outputVector)
{
    // get the info object
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the ouptut
    vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT())); 
    //vtkUnstructuredGrid *output = vtkUnstructuredGrid::GetData( outputVector,0); 

    if (!this->MeshFile)
    {
        vtkErrorMacro(<<"A File Name for the Mesh must be specified.");
        return 0;
    }

    // open a GMF file for reading
    int64_t InpMsh;
    int version,dim;
    InpMsh = GmfOpenMesh(this->MeshFile, GmfRead, &version, &dim);
    if(InpMsh == 0)
    {
        vtkWarningMacro(<<"Could not open GMF file");
        return 0;
    }
    if( dim != 3)
    {
        vtkErrorMacro(<<"Unsupported mesh dimension " << dim );
        return 0;
    }
    if( (version != 2 ) and (version != 3) and (version != 4) )
    {
        vtkErrorMacro(<<"Unsupported mesh version " << version );
        return 0;
    }


    std::cerr <<"Mesh File Opened" << std::endl;
    
    // Read the number of vertices
    const int64_t NmbVer = GmfStatKwd(InpMsh, GmfVertices);

    // Set up point container
    vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
    pts->SetDataTypeToDouble();
    pts->SetNumberOfPoints(NmbVer);

    double p[3];
    int rt; //reference , not in use in this plugin

    // Read the vertices
    GmfGotoKwd(InpMsh, GmfVertices);
    for(int64_t i=0;i<NmbVer;i++)
    {
        GmfGetLin(  InpMsh, GmfVertices, &p[0], &p[1], &p[2],&rt);
        vtkDebugMacro(<<p[0] << " " <<p[1] << " " << p[2]);
        pts->SetPoint(i,p);
    }
    output->SetPoints(pts);

    // Read Triangles and Tetrahedra
    if(version < 4)
    {
        ReadConnectivity<int32_t>(InpMsh,output);
    }
    else
    {
        ReadConnectivity<int64_t>(InpMsh,output);
    }


    output->Squeeze();

    // Close the mesh
    GmfCloseMesh(InpMsh);

    if(this->SolutionFile == NULL || strlen(this->SolutionFile) == 0)
        return 1;


    InpMsh = GmfOpenMesh( this->SolutionFile, GmfRead, &version, &dim);
    if(InpMsh == 0)
    {
        vtkWarningMacro(<<"Could not open Solution file");
        return 0;
    }
    if( dim != 3)
    {
        vtkErrorMacro(<<"Unsupported mesh dimension " << dim );
        return 0;
    }
    if( (version != 2 ) and (version != 3) and (version != 4) )
    {
        vtkErrorMacro(<<"Unsupported mesh version " << version );
        return 0;
    }
    
    vtkDebugMacro(<<"Solution File Opened");

    int64_t NmbSol;
    int SolSiz, NmbTyp, TypTab[ GmfMaxTyp ];

    NmbSol = GmfStatKwd(InpMsh, GmfSolAtVertices, &NmbTyp, &SolSiz, TypTab);

    if(SolSiz > 6)
    {
        vtkWarningMacro("Unsupported Solution size :" << SolSiz << " Currently up to  size 6 is supported");
        GmfCloseMesh(InpMsh);
        return 1;
    }
    if(NmbSol != NmbVer)
    {
        vtkWarningMacro("Unsupported number of solutions, all vertices should have a solution");
        GmfCloseMesh(InpMsh);
        return 1;
    }

    vtkSmartPointer<vtkDoubleArray> metric_field = vtkSmartPointer<vtkDoubleArray>::New();

    if( SolSiz < 6 )
    {
        metric_field->SetNumberOfComponents(SolSiz);
        metric_field->SetNumberOfTuples(NmbSol);
        metric_field->SetName("field data");

        double* sol = new double[SolSiz];

        GmfGotoKwd(InpMsh, GmfSolAtVertices);
        for(int64_t i=0;i<NmbSol;i++)
        {
            GmfGetLin(  InpMsh, GmfSolAtVertices, sol);
            metric_field->InsertTuple(i,sol);
        }

        GmfCloseMesh(InpMsh);
        output->GetPointData()->AddArray(metric_field);

        delete[] sol;
    }
    else
        // if we are dealing with tensors save them as such to enable more visualization options
    {
        metric_field->SetNumberOfComponents(9);
        metric_field->SetNumberOfTuples(NmbSol);
        metric_field->SetName("metric_field");

        double* sol = new double[SolSiz];

        double tensor[9];

        GmfGotoKwd(InpMsh, GmfSolAtVertices);
        for(int64_t i=0;i<NmbSol;i++)
        {
            GmfGetLin(  InpMsh, GmfSolAtVertices, sol);
            // sol represents the matrix 
            // sol0 sol1 sol3
            // sol1 sol2 sol4
            // sol3 sol4 sol5
            
            // VTK expect column major ordering
            tensor[0] = sol[0];  tensor[1] = sol[1]; tensor[2] = sol[3];
            tensor[3] = sol[1];  tensor[4] = sol[2]; tensor[5] = sol[4];
            tensor[6] = sol[3];  tensor[7] = sol[4]; tensor[8] = sol[5];
            metric_field->InsertTuple(i,tensor);
        }

        GmfCloseMesh(InpMsh);
        output->GetPointData()->AddArray(metric_field);

        delete[] sol;

    }

    return 1;
}

void vtkGMFReader::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    os  << indent << "MeshFile: "
        << (this->MeshFile ? this->MeshFile : "(none)") << "\n"
        << indent << "SolutionFile: "
        << (this->SolutionFile ? this->SolutionFile : "(none)") << "\n";
}
