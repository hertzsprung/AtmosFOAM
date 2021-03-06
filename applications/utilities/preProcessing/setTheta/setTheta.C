/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 1991-2009 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Application
    setTheta

Description
    Set theta based on an array of Brunt Vailsalla frequencies for different
    layers

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "ExnerTheta.H"
#include "noAdvection.H"
#include "stratifiedThermalField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    Foam::argList::addBoolOption("CP", "Set thetaf and reconstruct theta from thetaf for Charney-Phillips Staggering");
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"
    #include "readEnvironmentalProperties.H"
    #include "readThermoProperties.H"

    const noAdvection velocityField;
    stratifiedThermalField profile
    (
            g,
            T0,
            envProperties.lookup("BruntVaisallaFreq"),
            envProperties.lookup("zN"),
            velocityField
    );
        
    Info<< "Reading theta_init\n" << endl;
    volScalarField theta_init
    (
        IOobject("theta_init", runTime.constant(), mesh, IOobject::MUST_READ),
        mesh
    );

    Info<< "Reading thetaf_init\n" << endl;
    surfaceScalarField thetaf_init
    (
        IOobject("thetaf_init", runTime.constant(), mesh, IOobject::READ_IF_PRESENT),
        linearInterpolate(theta_init)
    );

    // thetaf
    Info << "Creating thetaf\n" << endl;
    surfaceScalarField thetaf
    (
        IOobject("thetaf", runTime.timeName(), mesh, IOobject::NO_READ),
	thetaf_init
    );
    profile.applyTo(thetaf);
    forAll(thetaf.boundaryField(), patchI)
    {
        fvsPatchField<scalar>& thetap = thetaf.boundaryFieldRef()[patchI];
        forAll(thetap, faceI)
        {
            thetap[faceI] = profile.tracerAt
            (
                    mesh.Cf().boundaryField()[patchI][faceI],
                    runTime
            );
        }
    }

    // theta
    Info<< "Creating theta\n" << endl;
    volScalarField theta
    (
        IOobject("theta", runTime.timeName(), mesh, IOobject::NO_READ),
        theta_init
    );

    // theta is either recontructed from thetaf (CP staggering) or
    // calculated from the analytic profile
    if (args.options().found("CP"))
    {
        // Creating bouyancy variables for reconstructing theta
        surfaceScalarField bf("bf", thetaf * gUnitNormal);
        volVectorField b("b", fvc::reconstruct(bf * mesh.magSf()));
        // set theta at boundaries as well as interior
        theta == (b & ghat);
    
        // Correct thetaf and theta based on b and bf
        thetaf = mag(bf)
               + fvc::interpolate(theta) * (1.0 - mag(gUnitNormal));
    }
    else // Lorenz staggering so calculate theta from analytic profile
    {
        profile.applyTo(theta);
    }

    theta.write();
    thetaf.write();

    volScalarField BruntFreq
    (
        IOobject("BruntFreq", runTime.timeName(), mesh),
        Foam::sqrt(-(g & fvc::grad(thetaf))/theta)
    );
    BruntFreq.write();

    surfaceScalarField BruntFreq2f
    (
        IOobject("BruntFreq2f", runTime.timeName(), mesh),
        -(g & mesh.delta())/mag(mesh.delta())*fvc::snGrad(theta)/thetaf
    );
    BruntFreq2f.write();

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //

