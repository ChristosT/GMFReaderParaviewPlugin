#ifndef vtkGMFReader_h
#define vtkGMFReader_h

#include "vtkUnstructuredGridAlgorithm.h"

//https://www.paraview.org/Wiki/Writing_ParaView_Readers

class vtkGMFReader : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkGMFReader* New();
  vtkTypeMacro(vtkGMFReader,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;//  VTK_OVERRIDE;

  vtkSetStringMacro(MeshFile);
  vtkSetStringMacro(SolutionFile);

  vtkGetStringMacro(MeshFile);
  vtkGetStringMacro(SolutionFile);

protected:
  vtkGMFReader();
  ~vtkGMFReader() override;// VTK_OVERRIDE;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override; //VTK_OVERRIDE;
  //int FillOutputPortInformation( int port, vtkInformation* info );
private:
  char* MeshFile;
  char* SolutionFile;
  vtkGMFReader(const vtkGMFReader&) = delete; //  VTK_DELETE_FUNCTION;
  void operator=(const vtkGMFReader&) = delete; // VTK_DELETE_FUNCTION;
};

#endif
