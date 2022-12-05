# SOLO Tools
SOLO A single-user operating system developed by Per Brinch Hansen and his team (1974-1975)

Tools to manipulate SOLO tape images

|Directory |Description                        |
|----------|------------------------------------|
|TapExtract|Extract files from a SOLO tape image|

Tools to manipulate SOLO disk images

|Directory |Description                                                                |
|----------|---------------------------------------------------------------------------|
|DskCatalog|Print the catalog of a dsk SOLO disk image                                 |
|DskExtract|Extract segments and files from a dsk SOLO disk image                      |
|DskMatch  |Match the pages of a dsk SOLO disk image and any other SOLO disk image     | 
|DskMap    |Create C header file with page map vector for dsk and rk05 SOLO disk images|  
|RK05toDsk |Create a dsk SOLO disk image from a rk05 SOLO disk image                   |

Tools to manipulate SOLO files

|Directory      |Description                                                                  |
|---------------|-----------------------------------------------------------------------------|
|Decoder        |Concurrent Pascal virtual machine decoder                                    |
|KernelMerge34  |Merges in a single file the PDP-11/34 kernel source files                    |
|KernelMerge45  |Merges in a single file the PDP-11/45 kernel source files                    |
|KernelExtract34|Extracts the high level encoding from the PDP-11/34 merged kernel source file|
|KernelExtract45|Extracts the high level encoding from the PDP-11/45 merged kernel source file|
