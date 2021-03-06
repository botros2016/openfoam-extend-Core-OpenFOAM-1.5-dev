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

Global
    wallFunctions

Description
    Calculate wall dissipation from wall-functions.

\*---------------------------------------------------------------------------*/

{
    labelList cellBoundaryFaceCount(epsilon_.size(), 0);

    scalar Cmu25 = pow(Cmu_.value(), 0.25);
    scalar Cmu75 = pow(Cmu_.value(), 0.75);

    const fvPatchList& patches = mesh_.boundary();

    //- Initialise the near-wall G field to zero
    forAll(patches, patchi)
    {
        const fvPatch& curPatch = patches[patchi];

        if (isType<wallFvPatch>(curPatch))
        {
            forAll(curPatch, facei)
            {
                label faceCelli = curPatch.faceCells()[facei];

                epsilon_[faceCelli] = 0.0;
                G[faceCelli] = 0.0;
            }
        }
    }

    //- Accumulate the wall face contributions to epsilon and G
    //  Increment cellBoundaryFaceCount for each face for averaging
    forAll(patches, patchi)
    {
        const fvPatch& curPatch = patches[patchi];

        if (isType<wallFvPatch>(curPatch))
        {
#           include "checkPatchFieldTypes.H"

            const scalarField& rhow = rho_.boundaryField()[patchi];

            const scalarField& muw = mu().boundaryField()[patchi];
            const scalarField& mutw = mut_.boundaryField()[patchi];

            scalarField magFaceGradU =
                mag(U_.boundaryField()[patchi].snGrad());

            forAll(curPatch, facei)
            {
                label faceCelli = curPatch.faceCells()[facei];

                scalar yPlus =
                    Cmu25*RASModel::y_[patchi][facei]
                   *sqrt(k_[faceCelli])
                   /(muw[facei]/rhow[facei]);

                // For corner cells (with two boundary or more faces),
                // epsilon and G in the near-wall cell are calculated
                // as an average

                cellBoundaryFaceCount[faceCelli]++;

                epsilon_[faceCelli] +=
                    Cmu75*pow(k_[faceCelli], 1.5)
                   /(kappa_.value()*RASModel::y_[patchi][facei]);

                if (yPlus > yPlusLam_)
                {
                    G[faceCelli] +=
                        (mutw[facei] + muw[facei])
                       *magFaceGradU[facei]
                       *Cmu25*sqrt(k_[faceCelli])
                       /(kappa_.value()*RASModel::y_[patchi][facei]);
                }
            }
        }
    }


    // Perform the averaging

    forAll(patches, patchi)
    {
        const fvPatch& curPatch = patches[patchi];

        if (isType<wallFvPatch>(curPatch))
        {
            forAll(curPatch, facei)
            {
                label faceCelli = curPatch.faceCells()[facei];

                epsilon_[faceCelli] /= cellBoundaryFaceCount[faceCelli];
                G[faceCelli] /= cellBoundaryFaceCount[faceCelli];
            }
        }
    }
}


// ************************************************************************* //
