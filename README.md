## What is GenStore?

> GenStore: A High-Performance and Energy-Efficient In-Storage Computing System for Genome Sequence Analysis

GenStore, the first in-storage processing system de- signed for genome sequence analysis that greatly reduces both data movement and computational overheads of genome sequence analysis by exploiting low-cost and accurate in-storage filters. GenStore leverages hardware/software co-design to address the challenges of in-storage processing, supporting reads with 1) different properties such as read lengths and error rates, which highly depend on the sequencing technology, and 2) different degrees of genetic variation compared to the reference genome, which highly depends on the genomes that are being compared. 



## Prerequisites

The infrastructure has been tested with the following system configuration:
  * G++ v11.1.0
  * Python v3.6
  * Perl v5.24.1




## Preparing Input Data

### Real Genomics Read Sets

The read sets used in the paper can be obtained by searching the read set eccession IDs provided in the paper in the [European Bioinformatics Institute ftp](ftp://ftp.sra.ebi.ac.uk/vol1/fastq/). 

### Synthetic Read Sets

We use mason_simulator (part of the SeqAn package) to simulate short reads of varying degree of genetic distance from the reference genome. 

0. cd input-generation
1. Download all files specified in files_to_download.txt to this directory
2. Create a directory called "index" and generate an index of the reference genome using the command
```
minimap2 -d index/hg38.mmi hg38.fa
```
3. Run `run_subsample_pipeline.sh`





## Baseline Software Exact Match Filter

We implement a baseline exact match filter using SIMD operations integrated in [minimap2]().

0. For installation, run `make`
1. General usage
```
minimap2 -d ref.mmi ref.fa                     # indexing
minimap2 -a ref.mmi reads.fq > alignment.sam   # alignment
```

### Code Walkthrough

* We implement the exact match filer in `exact2_match_sse.c`
* The filter in used in `map.c` by calling function `exact_match_sse`




## Software GenStore




## Hardware GenStore

We evaluate hardware configurations using two state-of-the-art simulators to analyze the performance of GenStore. We model DRAM timing with the DDR4 interface in [Ramulator](), a widely-used, cycle-accurate DRAM simulator. We model SSD performance using [MQSim](), a widely-used simulator for modern SSDs. We model the end-to-end throughput of GenStore based on the throughput of each GenStore pipeline stage: accessing NAND flash chips, accessing internal DRAM, accelerator computation, and transferring unfiltered data to the host. 


### HDL Implementation
We implement GenStore's accelerator units in Verilog to faithfully measure the throughput of the accelerators, and their area and power cost. We use Design Compiler version N-2017.09. The implementation can be found in genstore-hdl folder. 

0. In `key-script-command.tcl` , `path_to_verilog_files` is the path to genstore verilog source files, `<verilog_module>.v` is the file name containing the verilog module to synthesize, and `<verilog_module_name>` is the name of the module defined in this verilog file
1. Open up Synopsys command line
2. Run `key-script-command.tcl`


We will soon release the scripts used for Ramulator to model DRAM timing and the scripts used for MQSim to model SSD timing.


### End-to-end Throughput

We will soon release the script used for modelling the end-to-end throughput of GenStore based on the throughput of each GenStore pipeline stage.



## Citation
If you use this repo, please cite the following paper:
```
@inproceedings{mansouri2022genstore,
  title={GenStore: a high-performance in-storage processing system for genome sequence analysis},
  author={Mansouri Ghiasi, Nika and Park, Jisung and Mustafa, Harun and Kim, Jeremie and Olgun, Ataberk and Gollwitzer, Arvid and Senol Cali, Damla and Firtina, Can and Mao, Haiyu and Almadhoun Alserr, Nour and others},
  booktitle={Proceedings of the 27th ACM International Conference on Architectural Support for Programming Languages and Operating Systems},
  year={2022}
}
```

## Contact

Nika Mansouri Ghiasi - n.mansorighiasi@gmail.com
