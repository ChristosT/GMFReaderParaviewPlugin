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
    this->FileName = NULL;

    this->SetNumberOfInputPorts(0);
}


vtkGMFReader::~vtkGMFReader()
{
    delete [] this->FileName;
}

int vtkGMFReader::RequestData( vtkInformation *vtkNotUsed(request), 
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

    // open a GMF file for reading
    int64_t InpMsh;
    int version,dim;
    InpMsh = GmfOpenMesh(this->FileName, GmfRead, &version, &dim);
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


    vtkDebugMacro(<<"File Opened");
    
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

    // Try to open a sol/solb file if it exists with the same name
    std::string basefilename(this->FileName);

    const char ch = *basefilename.rbegin(); // .back() requires c++11

    if (ch == 'b')
    {
        // extension is meshb
        basefilename = basefilename.substr(0,basefilename.size() -5); // remove 'meshb'
    }
    else
    {
        basefilename = basefilename.substr(0,basefilename.size() -4); // remove 'mesh'
    }

    //try ascii mode
    InpMsh = GmfOpenMesh((basefilename + "sol").c_str(), GmfRead, &version, &dim);
    if(InpMsh == 0)
    {
        // try binary mode
        InpMsh = GmfOpenMesh((basefilename + "solb").c_str(), GmfRead, &version, &dim);
        if(InpMsh == 0)
        {
            return 1;
        }
    }

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
    
    vtkSmartPointer<vtkDoubleArray> metric = vtkSmartPointer<vtkDoubleArray>::New();

    metric->SetNumberOfComponents(SolSiz);
    metric->SetNumberOfTuples(NmbSol);
    metric->SetName("metric");
    
    double* sol = new double[SolSiz];
    
    GmfGotoKwd(InpMsh, GmfSolAtVertices);
    for(int64_t i=0;i<NmbSol;i++)
    {
        GmfGetLin(  InpMsh, GmfSolAtVertices, sol);
        metric->InsertTuple(i,sol);
    }

    GmfCloseMesh(InpMsh);
    output->GetPointData()->AddArray(metric);

    delete[] sol;

    return 1;
}

void vtkGMFReader::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    os << indent << "File Name: "
        << (this->FileName ? this->FileName : "(none)") << "\n";
}
