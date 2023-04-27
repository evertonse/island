#! usr/bin/python3
import sys
from pathlib import Path
NORMALIZE = True

def check_dimension(v, filename, line_number):
    if len(v) != 3:
        print(
            f"[poly2tri]: Error {v} has dimension != 3 from {filename} on {line_number}\n"
        )
        exit(1)


def convert(filepath) -> str:
    vertices = list()
    faces = list()
    uvs = list()
    normals = list()
    vertice_max = 0 
    with open(filepath) as objfile:
        for line in objfile.readlines():
            line = line.replace("\n", "").strip()
            if line.startswith("#"):
                continue

            if line.startswith("vt"):
                uv = [v for v in line.split()[1:]]
                uvs.append(uv)

            elif line.startswith("vn"):
                vn = [v for v in line.split()[1:]]
                assert len(vn) == 3
                normals.append(vn)

            elif line.startswith("v"):
                vertex = [v for v in line.split()[1:]]
                assert len(vertex) == 3
                vertices.append(vertex)

            elif line.startswith("f"):
                face = [f for f in line.split()[1:]]
                for j in range(len(face) - 2):
                    triangle = [face[0], face[j + 1], face[j + 2]]
                    faces.append(triangle)
            else:
                pass  # ignore it for now,
    if NORMALIZE:
        import numpy as np
        vertices  = np.array(vertices, dtype=np.float32)
        abs_max   = np.abs(vertices.max())
        abs_min   = np.abs(vertices.min())
        max_val   = max(abs_max, abs_min)
        vertices  = vertices/max_val

    def to_string(start, data:list,dim=3):
        if dim == 3:
            strings = [f"{start} {i[0]} {i[1]} {i[2]}\n" for i in data]
        elif dim == 2:
            strings = [f"{start} {i[0]} {i[1]}\n" for i in data]
        return "".join(strings)

    vertices_string = to_string('v',vertices)

    faces_string = to_string('f', faces)

    normals_string = to_string('vn', normals)

    uvs_string = to_string('vt',uvs, dim=2) 

    return f"{vertices_string}{normals_string}{uvs_string}{faces_string}"



def main(files) -> list:
    mapped_files = []
    for file in files:
        print(f"[poly2tri]: converting {file}")

        original = Path(file)
        obj = convert(file)
        objfilepath = Path(f"{Path(original.parent,original.stem)}.tri")
        objfilepath.touch(exist_ok=True)
        objfilepath.write_text(obj)

        print(f"[poly2tri]: file converted succesfully, on {objfilepath}\n")
        mapped_files.append(objfilepath)
    return mapped_files



if __name__ == "__main__":
    main(sys.argv[1:])
