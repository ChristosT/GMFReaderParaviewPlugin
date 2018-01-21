#ifndef vtkMEDITeader_h
#define vtkMEDITReader_h

#include "vtkAbstractPolyDataReader.h"

class vtkMEDITReader : public vtkAbstractPolyDataReader
{
public:
  vtkTypeMacro(vtkMEDITReader,vtkAbstractPolyDataReader);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  static vtkMEDITReader *New();

protected:
  vtkMEDITReader();
  ~vtkMEDITReader() VTK_OVERRIDE;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;
private:
  vtkMEDITReader(const vtkMEDITReader&) VTK_DELETE_FUNCTION;
  void operator=(const vtkMEDITReader&) VTK_DELETE_FUNCTION;
};

#endif
