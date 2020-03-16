paraview_add_plugin(GMFReaderParaview
  VERSION "2.0"
  MODULES GMFReader
  REQUIRED_ON_SERVER
  MODULE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Reader/vtk.module")

