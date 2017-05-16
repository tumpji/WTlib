# WTlib
This project is under GNU Lesser General Public License 3.0 saved in file "LICENSE" an exception is folder "thirdparty".
This folder has subfolders with their own licences.

Program and library require CMake, Make, C++ compiler with multithreading support.
Program additionally requires Boost C++, CImg library (contents of folder "thirdparty"), libpng library.

## Program
To display a summary of all posible commands and its uses, use -h or --help. 
Further explanation of some more complicated commands can be found in thesis attachment.
Program inputs and outputs are PNG files and proposed format.


## Library
The entire interface can be found in the file "include/wt.h".
There are three main objects: WTencoder (encoding), WTdecoder (decoding), WTcommon (settings for both WTencoder and WTdecoder).
As an example, a program was created (folder "demo").


### Encoding
There are many configurations for encoding data.
It is possible to change this setting by modifying object WTcommon.

List of all posible variables:
barevny_prostor (color transform used), 
vlnka (wavelet), 
dekompozice (image decomposition used in wavelet transform -- tensor product or red-black wavelet transform), 
dekompozice_max (maximum number of levels used in wavelet transform aka. maximum multiresolution level), 
dekompozice_min_rozmer (minimum width and height to continue wavelet transform), 
dekompozice_side_level (maximum WT levels used on H and LH(-1) bands), 
prediktor_pred (predictor before WT), 
prediktor_pasmo (predictor for specific band before entropy coder), 
koder (entropy coder used), 
vyjimka_koder (entropy coder exception for specific band)

The most important method is WTencoder::vlozit_pasmo.
This method loads data into the WTencoder object.
Finaly WTencoder::start( std::string path ) starts compression and creates file in proposed format.

### Decoding
Decoding is mutch simpler because WTcommon is filled automaticly based on encoded file.
Use method std::vector<WTdata> WTdecoder::start( std::string path ) for it.








