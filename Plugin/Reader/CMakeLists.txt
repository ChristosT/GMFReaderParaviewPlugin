set(classes
    vtkGMFReader
    )

vtk_module_add_module(GMFReader
  CLASSES ${classes}
  )
vtk_module_link(GMFReader
    PUBLIC
    Meshb.7)
paraview_add_server_manager_xmls(
    XMLS  vtkGMFReader.xml)
