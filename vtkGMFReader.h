#ifndef vtkGMFeader_h
#define vtkGMFReader_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkObjectFactory.h"

class vtkGMFReader : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkGMFReader* New();
  vtkTypeMacro(vtkGMFReader,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  vtkSetStringMacro(MeshFile);
  vtkSetStringMacro(SolutionFile);

  vtkGetStringMacro(MeshFile);
  vtkGetStringMacro(SolutionFile);

protected:
  vtkGMFReader();
  ~vtkGMFReader() VTK_OVERRIDE;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;
private:
  char* MeshFile;
  char* SolutionFile;
  vtkGMFReader(const vtkGMFReader&) VTK_DELETE_FUNCTION;
  void operator=(const vtkGMFReader&) VTK_DELETE_FUNCTION;
};

#endif
