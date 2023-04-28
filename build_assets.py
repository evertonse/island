
import os
from pathlib import Path
from poly2tri import main as poly2tri
objs = [Path(f) for f in [
    'assets/models/wolf/wolf.obj', 
    'assets/models/horse/horse.obj', 
    'assets/models/plants/Bamboo/bamboo.obj',
    'assets/models/tiger/tiger.obj',
    'assets/models/goblin/goblin.obj',
    'assets/models/cube/cube.obj',
    'assets/models/plants/decorative/vase_plant.obj',
]]

mapped_objs = poly2tri(objs)
mapped_objs = [ (p,Path("src/assets/",p.stem+"_obj.h")) for p in mapped_objs]

exe =  Path('bin/obj2c')
if not exe.is_file() and not Path(str(exe) + ".exe").is_file():
    output = Path("bin/obj2c")
    input = Path("vendor/obj2c/obj2c.cpp")
    os.system(f"c++ -std=c++11 -Wall -Wextra -Wshadow -pedantic {input} -o {output}")


mapped_objs.extend([
    # Extend the models right here with tuple (in_filepath, out_filepath)  
    # For example:
    # ('assets/models/cube/cube.obj','src/assets/cube_obj.h'),
])

for input_file,output_file in mapped_objs:
    input_file = Path(input_file)
    output_file = Path(output_file)
    print(f"[obj2c]: compiling {input_file}")

    if str(input_file).count("goblin") > 0 :
        options = [
            "--write_counts", 
            "--vb_friendly",
        ]
    else:
        options = [
            "--write_counts", 
            "--vb_friendly",
            "--smooth_normals", # << requires --vb_friendly
        ]

    code = os.system(f"{exe} {input_file} {output_file} {' '.join(options)}")
    if code == 0:
        print(f"[obj2c]: success on {input_file}. obj.h file added to {output_file}")
        continue
    else:
        print(f"[obj2c]: ERROR occurred during build of asset {input_file} !!")
        exit(1)