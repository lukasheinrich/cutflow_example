#!/bin/bash
mkdir /input
cd /input
export TEST_FILENAME=test_ttbar_AOD.pool.root
curl -O http://physics.nyu.edu/~lh1132/test_ttbar_AOD.pool.root
cd /analysis
source setup.sh
root -l -b -q $ROOTCOREDIR/scripts/load_packages.C $ROOTCOREBIN/user_scripts/MyAnalysis/submit.C'("submitDir")'
python tests/plot.py
mkdir /cutflowcomp
python tests/extractcutflow.py "submitDir/hist-$TEST_FILENAME.root" /cutflowcomp/cutflow.yaml