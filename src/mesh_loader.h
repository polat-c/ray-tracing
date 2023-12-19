// --> https://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

#include "vec2.h"
#include "material.h"
#include "mesh.h"

class mesh_loader {
    // Loads triangular mesh with given material

    public:
        // Constructor
        mesh_loader(shared_ptr<material> m) : mat(m) {}

        // Functions
        bool load(string filename, vector<shared_ptr<triangle>>& triangles, double scale=1) const {
            // initialize temp objects
            vector<point3> vertices;
            vector<unsigned int> vindices;

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
                    vertices.push_back(vertex*scale);
                } else if ( strcmp( line_header, "f" ) == 0 ) {
                    unsigned int vertex_index[3];
                    fscanf(file, "%d %d %d\n", &vertex_index[0], &vertex_index[1], &vertex_index[2] );
                    vindices.push_back(vertex_index[0]);
                    vindices.push_back(vertex_index[1]);
                    vindices.push_back(vertex_index[2]);
                }
            }
            // process data
            process(vertices, vindices, triangles);
            return true;
        }

        private:
            shared_ptr<material> mat;

            bool process(vector<point3> vertices, vector<unsigned int> vindices, vector<shared_ptr<triangle>>& triangles) const {
                for(int i=0; i<vindices.size(); i++) {
                    point3 Q = vertices[vindices[i]-1];
                    vec3 u = vertices[vindices[i+1]-1] - vertices[vindices[i]-1];
                    vec3 v = vertices[vindices[i+2]-1] - vertices[vindices[i]-1];
                    triangles.push_back(make_shared<triangle>(Q, u, v, mat));
                    i += 2;
                }
                return true;
            }
};

#endif