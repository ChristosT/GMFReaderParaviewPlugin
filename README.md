# Gamma Mesh Format (GMF) reader plugin for Paraview


Visualize meshes in the [meshb format](https://github.com/LoicMarechal/libMeshb) in [Paraview](https://www.paraview.org).
Optionally, one can also load a mathing `.sol/.solb` file for visualiazion solution attributes.

Tested using Paraview 5.6.0 & 5.8.0 on Linux (Ubuntu 16.04).

## Instructions


### Paraview 5.8.0
1. Clone [the official docker image for building paraview plugins](https://gitlab.kitware.com/paraview/paraview-plugin-builder)
2. Build paraview 5.8.0 by running `./run_build_paraview.sh  v5.8.0`
3. Build the plugin by running `./run_build_build.sh -d <local copy of this repository> v5.8.0`
4. Copy the generated .so files in a directory 
5. Load from the directory above the `GMFReaderParaview.so`  throught the `Tools->Manage Plug-ins` menu.
6. The `File->Open` menu should now show `.mesh` and `.meshb` files in the available formats.

### Paraview 5.6.0
1. Checkout the 5.6.0 tag of this repository 
2. The installation can be done using the [paraview superbuild system](https://github.com/lhofmann/paraview-superbuild-docker).
Using the provided Dockerfile build the superbuild system or download it directly from [dockerhub](https://hub.docker.com/r/lhofmann/paraview-superbuild).
3. Start the image using the provided `run.sh` script.
4. Start the container and navigate to `/mnt/shared` which is mounted under the current directory.
5. Run `mkdir build  && cd build && cmake ../src/ && make ` to build the plugin. This should produce a `.so` library which you can load from paraview throught the
`Tools->Manage Plug-ins` menu.
6. The `File->Open` menu should now show `meshb` files in the available formats.

## Note
A `.mesh/.meshb` file can hold a wide variaty of attributes. The reader loads :
* Coordinates of points
* Element Connectivity information (high order elements are not supported)
* `VerticesOnGeometricTriangles`,`VerticesOnGeometricEdges`,`VerticesOnGeometricTriangles` info saved into the `cad_info` array using the following convention: Each point gets a `cad_info` tuple. The first component is the dimension of the geometrical entity (0-> vertex,1->edge,2->triangle,-1 -> volume point). The second component is the id of the geometric entity. 
Only ascii ugrid and lb8.ugrid, b8.ugrid files are supported.
