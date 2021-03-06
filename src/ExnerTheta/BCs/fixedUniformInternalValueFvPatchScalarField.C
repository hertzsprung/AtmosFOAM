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

\*---------------------------------------------------------------------------*/

#include "fixedUniformInternalValueFvPatchScalarField.H"
#include "fvPatchFieldMapper.H"
#include "fvMatrix.H"
#include "volFields.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

fixedUniformInternalValueFvPatchScalarField::
fixedUniformInternalValueFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchField<scalar>(p, iF)
{}


fixedUniformInternalValueFvPatchScalarField::
fixedUniformInternalValueFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchField<scalar>(p, iF, dict)
{}


fixedUniformInternalValueFvPatchScalarField::
fixedUniformInternalValueFvPatchScalarField
(
    const fixedUniformInternalValueFvPatchScalarField& ptf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedValueFvPatchField<scalar>(ptf, p, iF, mapper)
{}


fixedUniformInternalValueFvPatchScalarField::
fixedUniformInternalValueFvPatchScalarField
(
    const fixedUniformInternalValueFvPatchScalarField& wbppsf
)
:
    fixedValueFvPatchField<scalar>(wbppsf)
{}


fixedUniformInternalValueFvPatchScalarField::
fixedUniformInternalValueFvPatchScalarField
(
    const fixedUniformInternalValueFvPatchScalarField& wbppsf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchField<scalar>(wbppsf, iF)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void fixedUniformInternalValueFvPatchScalarField::updateCoeffs()
{
    if (updated())
    {
        return;
    }
    
    scalarField& vf = const_cast<DimensionedField<scalar, volMesh>&>(internalField());
    forAll(*this, facei)
    {
        label celli = patch().faceCells()[facei];
        vf[celli] = this->operator[](facei);
    }
    
    fvPatchField<scalar>::updateCoeffs();
}


void Foam::fixedUniformInternalValueFvPatchScalarField::updateWeightedCoeffs
(
    const scalarField& weights
)
{
    updateCoeffs();
}


void Foam::fixedUniformInternalValueFvPatchScalarField::manipulateMatrix
(
    fvMatrix<scalar>& matrix
)
{
    // Apply the patch internal field as a constraint in the matrix
    matrix.setValues(this->patch().faceCells(), this->patchInternalField());
}


void Foam::fixedUniformInternalValueFvPatchScalarField::manipulateMatrix
(
    fvMatrix<scalar>& matrix,
    const Field<scalar>& weights
)
{
    manipulateMatrix(matrix);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

makePatchTypeField
(
    fvPatchScalarField,
    fixedUniformInternalValueFvPatchScalarField
);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
