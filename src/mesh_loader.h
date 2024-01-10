// --> https://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

#include "material.h"
#include "mesh.h"

class mesh_loader {

    public:
        // Functions
        bool load(string filename, mesh& obj_mesh) const {

            FILE * file = fopen(filename.c_str(), "r");
            if( file == NULL ){
                clog << "Impossible to open the file !\n";
                return false;
            }
            // read file
            while(true) {
                char line_header[128]; // assume first word in a line is 128 chars at most (we do not to parse more)
                // read the first word of the line
                int res = fscanf(file, "%s", line_header); // parse word by word
                if (res == EOF)
                    break; // EOF = End Of File. Quit the loop.

                if ( strcmp( line_header, "v" ) == 0 ){
                    point3 vertex;
                    float v0, v1, v2;
                    //fscanf(file, "%d %d %d\n", &vertex.e[0], &vertex.e[1], &vertex.e[2] );
                    fscanf(file, "%f %f %f\n", &v0, &v1, &v2 );
                    vertex = vec3(v0,v1,v2);
                    obj_mesh.vertices.push_back(vertex);
                } else if ( strcmp( line_header, "f" ) == 0 ) {
                    unsigned int vertex_index[3];
                    fscanf(file, "%d %d %d\n", &vertex_index[0], &vertex_index[1], &vertex_index[2] );
                    obj_mesh.vindices.push_back(vertex_index[0]);
                    obj_mesh.vindices.push_back(vertex_index[1]);
                    obj_mesh.vindices.push_back(vertex_index[2]);
                }
            }
            return true;
        }
};

#endif