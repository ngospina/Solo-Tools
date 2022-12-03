# SOLO Tools
SOLO A single-user operating system developed by Per Brinch Hansen and his team (1974-1975)

|Directory|Description                                                    |
|---------|---------------------------------------------------------------|
|SOLO     |PDP-11/34 and PDP-11/45 SOLO images and SOLO files and segments|

Tools to manipulate SOLO tape images

|Tool      |Description                        |
|----------|-----------------------------------|
|TapExtract|Extract files from SOLO tape images|

Tools to manipulate SOLO disk images

|Tool      |Description                                                           |
|----------|----------------------------------------------------------------------|
|DskCatalog|Print the catalog of a dsk SOLO disk image                            |
|DskExtract|Extract segments and files from SOLO disk images                      |
|DskMatch  |Match the pages of a dsk SOLO disk image and any other SOLO disk image| 
|DskMap    |Create C header file with page map vector for dsk and rk05 SOLO images|  
|RK05toDsk |Create a dsk SOLO file image from a rk05 SOLO file image              |

Tools to manipulate SOLO files

|Tool           |Description                                                                  |
|---------------|-----------------------------------------------------------------------------|
|Decoder        |Concurrent Pascal Virtual Machine Decoder                                    |
|KernelMerge34  |Merges in a single file the PDP-11/34 kernel source files                    |
|KernelMerge45  |Merges in a single file the PDP-11/45 kernel source files                    |
|KernelExtract34|Extracts the high level encoding from the PDP-11/34 merged kernel source file|
|KernelExtract45|Extracts the high level encoding from the PDP-11/45 merged kernel source file|
