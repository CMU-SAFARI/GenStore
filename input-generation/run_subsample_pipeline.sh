#!/bin/bash

INFILE="HG001_GRCh38_GIAB_highconf_CG-IllFB-IllGATKHC-Ion-10X-SOLID_CHROM1-X_v.3.3.2_highconf_PGandRTGphasetransfer.clean.vcf.gz"
OUTBASE="HG001.subsample"
REF="hg38.fa"
NTHREADS_TOTAL=48
NTHREADS=$(( $NTHREADS_TOTAL / 2 ))
NREADS=70000000
PARAMS="--eqx -a --secondary=no"
INDEX="index/hg38.mmi"
MASON_PARAMS="--illumina-prob-mismatch 0.001 --illumina-prob-mismatch-begin 0.001 --illumina-prob-mismatch-end 0.001 --illumina-read-length 150"

function full {
    FRAC=1.0
    python3 subsample_vcf.py <(pv $INFILE | zcat) $FRAC | bgzip > $OUTBASE.$FRAC.vcf.gz
    tabix -p vcf $OUTBASE.$FRAC.vcf.gz

    mason_simulator -ir $REF -n $NREADS -iv $OUTBASE.$FRAC.vcf.gz -o $OUTBASE.$FRAC.fq.gz $MASON_PARAMS --num-threads $NTHREADS
    minimap2 $PARAMS -t $NTHREADS $INDEX $OUTBASE.$FRAC.fq.gz > $OUTBASE.$FRAC.fq.gz.sam
    python3 get_reads.py $OUTBASE.$FRAC.fq.gz.sam >$OUTBASE.$FRAC.fq.gz.sam.picked.fq 2>$OUTBASE.$FRAC.fq.gz.sam.stats
}

function half {
    FRAC=0.5
    python3 subsample_vcf.py <(pv $INFILE | zcat) $FRAC | bgzip > $OUTBASE.$FRAC.vcf.gz
    tabix -p vcf $OUTBASE.$FRAC.vcf.gz

    mason_simulator -ir $REF -n $NREADS -iv $OUTBASE.$FRAC.vcf.gz -o $OUTBASE.$FRAC.fq.gz $MASON_PARAMS --num-threads $NTHREADS
    minimap2 $PARAMS -t $NTHREADS $INDEX $OUTBASE.$FRAC.fq.gz > $OUTBASE.$FRAC.fq.gz.sam
    python3 get_reads.py $OUTBASE.$FRAC.fq.gz.sam >$OUTBASE.$FRAC.fq.gz.sam.picked.fq 2>$OUTBASE.$FRAC.fq.gz.sam.stats
}

function partial {
    FRAC=0.25
    python3 subsample_vcf.py <(pv $INFILE | zcat) $FRAC | bgzip > $OUTBASE.$FRAC.vcf.gz
    tabix -p vcf $OUTBASE.$FRAC.vcf.gz

    mason_simulator -ir $REF -n $NREADS -iv $OUTBASE.$FRAC.vcf.gz -o $OUTBASE.$FRAC.fq.gz $MASON_PARAMS --num-threads $NTHREADS
    minimap2 $PARAMS -t $NTHREADS $INDEX $OUTBASE.$FRAC.fq.gz > $OUTBASE.$FRAC.fq.gz.sam
    python3 get_reads.py $OUTBASE.$FRAC.fq.gz.sam >$OUTBASE.$FRAC.fq.gz.sam.picked.fq 2>$OUTBASE.$FRAC.fq.gz.sam.stats
}


full
half
partial
wait
