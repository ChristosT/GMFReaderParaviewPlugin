#ifndef vtkMEDITeader_h
#define vtkMEDITReader_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkObjectFactory.h"

class vtkMEDITReader : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkMEDITReader* New();
  vtkTypeMacro(vtkMEDITReader,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);


protected:
  vtkMEDITReader();
  ~vtkMEDITReader() VTK_OVERRIDE;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;
private:
  char* FileName;
  vtkMEDITReader(const vtkMEDITReader&) VTK_DELETE_FUNCTION;
  void operator=(const vtkMEDITReader&) VTK_DELETE_FUNCTION;
};

#endif
