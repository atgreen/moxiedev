Modified MiBench source code for MiDataSets (GPL license)


*** Development website ***

http://midatasets.sourceforge.net


*** Release History ***

V1.3 January 25, 2008

     Many thanks to Kenneth Hoste from Ghent University who evaluated 
     MiDataSets and provided a valuable feedback. Several programs 
     (either source code or loop wrapper) have been slightly modified 
     either to remove small bugs or to keep benchmark execution time 
     approximately 10 seconds on AMD Athlon 64 3700+ processors:
 
     * consumer_lame
     * consumer_tiff2bw
     * consumer_tiffdither
     * consumer_tiffmedian
     * office_ghostscript
     * office_rsynth

     Several datasets have also been modified/changed to work properly 
     with the updated programs:
     
     * consumer_tiff_data (all ??.bw.tif have been converted to 8-bit grayscale
                          instead of 1-bit B&W to work properly with consumer_tiffdither)
     * office_data

V1.2 November 19, 2007
     
     Qsort and stringsearch benchmarks are updated since they used 
     standard library calls and were not useful for iterative optimizations -
     I added bodies of the qsort and strsearch functions to these benchmarks
     to have more room for optimizations. 

     Note, that these benchmarks are now in the new directories:
     
     * automotive_qsort1
     * office_stringsearch1

     A few stupid mistakes are fixed in several benchmarks (security_blowfish_d, 
     security_blowfish_e, security_pgp_d, security_pgp_e, dijkstra, patricia) 
     where I used a file tmp-ccc-run-midataset instead of _finfo_dataset 
     for the loop wrapper [thanks to Veerle Desmet from Ghent University].
     I am working on the Collective Compilation Framework and these files
     have been taken from that project by accident ...

     Some numbers for loop wrappers for dijkstra and patricia have been updated.

     A few tmp files have been removed (*.a, *.wav) - I forgot to remove
     them from the sources directories :( - it reduced the size of the tar ball by 40% !..

     Finally, Makefiles for Open64 compiler have been added.
     
     If you have any questions or comments, please don't hesitate
     to contact me (grigori.fursin@inria.fr, http://fursin.net/research)
          

V1.1 September 05, 2007

     We would like to thank you all for your interest
     and valuable feedback, and are pleased to announce 
     a bug-fix release for modified MiBench benchmark sources 
     for MiDataSets V1. Any new benchmarks and datasets'
     contributions are welcome!

     The following benchmarks have been fixed:

     * consumer_lame
     * office_ghostscript
     * office_ispell
     * office_stringsearch
     * security_blowfish_d
     * security_blowfish_e
     * security_pgp_d
     * security_pgp_e

V1.0 March 17, 2007

     First official release.

V0.1 February 01, 2006 

     Preliminary set of several datasets is prepared
     and used internally at INRIA for research.


*** Remarks ***

Most of the source codes have been slightly modified by Grigori Fursin
to simplify and automate iterative optimizations. A loop wrapper has been 
added around the main procedure to make some benchmarks run longer when 
real execution time is used for measurements instead of a simulator 
(we do not yet take into account cache effects - it's a future work). 
Each benchmark with each datasets run approximately 10 seconds 
on INRIA cluster with AMD Athlon 64 3700+ processors.

Each directory has 4 Makefiles for GCC, Intel compilers, Open64 and PathScale compilers. 
Each directory has a "__run" batch file to execute a benchmark. The first
parameter is the dataset number and the second optional parameter is the 
upper bound of the loop wrapper around the main procedure. 
If second parameter is omitted, the loop wrapper upper bound 
is taken from the file _run/_finfo_dataset.<dataset_number>.

Several batch files are included as examples to automate iterative optimizations
 all__create_work_dirs - creates temporal work directories for each benchmark
 all__delete_work_dirs - delete all temporal work directories
 all_compile - compile all benchmarks in the temporal work directories 
 all_run - run all benchmarks with all datasets in the temporal work directories

Though we made an effort to include only copyright free datasets 
from the Internet, mistakes are possible. In such cases, please 
contact Grigori Fursin (grigori.fursin@inria.fr) as soon as possible 
and we will try to resolve the issue.
