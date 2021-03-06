#!/bin/bash
set -e
rm -rf [0-9]*
blockMesh
terrainFollowingMesh
setInitialTracerField
setVelocityField

advectiveFoamF

setAnalyticTracerField -time 5000
setAnalyticTracerField -time 10000
zeroVerticalFaces Tf
zeroVerticalFaces -time 5000 Tf_analytic
zeroVerticalFaces -time 10000 Tf_analytic

sumFields -scale0 1 -scale1 -1 10000 T_diff 10000 T 10000 T_analytic
sumFields -scale0 1 -scale1 -1 5000 Tf_diff 5000 Tf 5000 Tf_analytic
sumFields -scale0 1 -scale1 -1 10000 Tf_diff 10000 Tf 10000 Tf_analytic
gmtFoam -time 5000 Tf_diff
gmtFoam -time 10000 Tf_diff

globalSum -time 10000 Tf_diff
mv globalSumTf_diff.dat 10000
tail -n1 10000/globalSumTf_diff.dat | cut -d' ' -f3 > 10000/l2errorTf_diff.txt

globalSum -time 10000 Tf_analytic
mv globalSumTf_analytic.dat 10000
tail -n1 10000/globalSumTf_analytic.dat | cut -d' ' -f3 > 10000/l2errorTf_analytic.txt

python3 -c "print(`paste -d'/' 10000/l2errorTf_diff.txt 10000/l2errorTf_analytic.txt`)" > 10000/l2errorTf.txt
