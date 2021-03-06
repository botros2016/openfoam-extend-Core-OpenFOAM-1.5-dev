/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
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

Description
    Agglomerate one level using the MGridGen algorithm.

\*---------------------------------------------------------------------------*/

#include "MGridGenGAMGAgglomeration.H"
#include "fvMesh.H"
#include "syncTools.H"

//extern "C"
//{
//#   include "mgridgen.h"
//}

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::MGridGenGAMGAgglomeration::
makeCompactCellFaceAddressingAndFaceWeights
(
    const lduAddressing& fineAddressing,
    List<idxtype>& cellCells,
    List<idxtype>& cellCellOffsets,
    const vectorField& Si,
    List<scalar>& faceWeights
)
{
    const label nFineCells = fineAddressing.size();
    const label nFineFaces = fineAddressing.upperAddr().size();

    const unallocLabelList& upperAddr = fineAddressing.upperAddr();
    const unallocLabelList& lowerAddr = fineAddressing.lowerAddr();

    // Number of neighbours for each cell
    labelList nNbrs(nFineCells, 0);

    forAll (upperAddr, facei)
    {
        nNbrs[upperAddr[facei]]++;
    }

    forAll (lowerAddr, facei)
    {
        nNbrs[lowerAddr[facei]]++;
    }

    // Set the sizes of the addressing and faceWeights arrays
    cellCellOffsets.setSize(nFineCells + 1);
    cellCells.setSize(2*nFineFaces);
    faceWeights.setSize(2*nFineFaces);


    cellCellOffsets[0] = 0;
    forAll (nNbrs, celli)
    {
        cellCellOffsets[celli+1] = cellCellOffsets[celli] + nNbrs[celli];
    }

    // reset the whole list to use as counter
    nNbrs = 0;

    forAll (upperAddr, facei)
    {
        label own = upperAddr[facei];
        label nei = lowerAddr[facei];

        label l1 = cellCellOffsets[own] + nNbrs[own]++;
        label l2 = cellCellOffsets[nei] + nNbrs[nei]++;

        cellCells[l1] = nei;
        cellCells[l2] = own;

        faceWeights[l1] = mag(Si[facei]);
        faceWeights[l2] = mag(Si[facei]);
    }
}


Foam::tmp<Foam::labelField> Foam::MGridGenGAMGAgglomeration::agglomerate
(
    label& nCoarseCells,
    const label minSize,
    const label maxSize,
    const lduAddressing& fineAddressing,
    const scalarField& V,
    const vectorField& Sf,
    const scalarField& Sb
)
{
    const label nFineCells = fineAddressing.size();

    // Compact addressing for cellCells
    List<idxtype> cellCells;
    List<idxtype> cellCellOffsets;

    // Face weights = face areas of the internal faces
    List<scalar> faceWeights;

    // Create the compact addressing for cellCells and faceWeights
    makeCompactCellFaceAddressingAndFaceWeights
    (
        fineAddressing,
        cellCells,
        cellCellOffsets,
        Sf,
        faceWeights
    );

    // agglomeration options.
    List<int> options(4, 0);
    options[0] = 4;                   // globular agglom
    options[1] = 6;                   // objective F3 and F2
    options[2] = 128;                 // debugging output level
    options[3] = fvMesh_.nGeometricD(); // Dimensionality of the grid


    // output: cell -> processor addressing
    List<int> finalAgglom(nFineCells);
    int nMoves = -1;
        
    MGridGen
    (
        nFineCells,
        cellCellOffsets.begin(),
        const_cast<scalar*>(V.begin()),
        const_cast<scalar*>(Sb.begin()),
        cellCells.begin(),
        faceWeights.begin(),
        minSize,
        maxSize,
        options.begin(),
        &nMoves,
        &nCoarseCells,
        finalAgglom.begin()
    );

    return tmp<labelField>(new labelField(finalAgglom));
}


// ************************************************************************* //
