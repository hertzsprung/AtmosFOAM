#!/bin/bash -e

# clear out old stuff
rm -rf [0-9]* constant/polyMesh core log

# create mesh
blockMesh

# hydrostatically balanced initial conditions
rm -rf [0-9]* core
mkdir 0
cp -r init_0/* 0
setExnerBalancedH

# change Exner BC from fixedValue to hydroStaticExner
sed -i 's/fixedFluxBuoyantExner/partitionedHydrostaticExner/g' 0/Exner

# Add a warm perturnation
cp 0/theta 0/theta_init
makeHotBubble

# Partition into stable and buoyant fluids
mv 0/theta 0/buoyant.theta
mv 0/theta_init 0/stable.theta
mv 0/Uf 0/stable.Uf
cp 0/stable.Uf 0/buoyant.Uf
rm 0/thetaf

# create initial conditions
setFields
sumFields 0 stable.sigma init_0 stable.sigma 0 buoyant.sigma -scale1 -1

# Plot initial conditions
gmtFoam theta
gv 0/theta.pdf &
gmtFoam sigma
gv 0/sigma.pdf &

# Solve Euler equations
partitionedExnerFoam >& log & sleep 0.01; tail -f log

# Plot one value of theta
time=100
gmtFoam theta -time $time
gv $time/theta.pdf &

# animate the results
gmtFoam theta
animate 0/theta.pdf ???/theta.pdf 1000/theta.pdf

# Differences between non-partitioned run
time=2
sumFields $time ExnerDiff $time Exner ../standard/$time Exner -scale1 -1
sumFields $time thetaDiff $time stable.theta ../standard/$time theta -scale1 -1
sumFields $time UfDiff $time stable.Uf ../standard/$time Uf -scale1 -1
gmtFoam -time $time ExnerDiff
gv $time/ExnerDiff.pdf &
gmtFoam -time $time thetaDiff
gv $time/thetaDiff.pdf &

sumFields $time fluxDiff $time fluxSum ../standard/$time U -scale1 -1
sumFields $time rhoDiff $time stable.sigmaRho ../standard/$time rho -scale1 -1
sumFields $time uDiff $time stable.u ../standard/$time u -scale1 -1

sumFields $time buoyant.sigma $time stable.sigma init_0 stable.sigma -scale0 -1
gmtFoam -time $time sigma
gv $time/sigma.pdf &

