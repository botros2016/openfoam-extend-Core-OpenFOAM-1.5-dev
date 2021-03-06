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

Class
    Foam::LUscalarMatrix

Description
    Foam::LUscalarMatrix

SourceFiles
    LUscalarMatrix.C

\*---------------------------------------------------------------------------*/

#ifndef LUscalarMatrix_H
#define LUscalarMatrix_H

#include "scalarMatrix.H"
#include "labelList.H"
#include "FieldField.H"
#include "lduInterfaceFieldPtrsList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class lduMatrix;
class procLduMatrix;

/*---------------------------------------------------------------------------*\
                           Class LUscalarMatrix Declaration
\*---------------------------------------------------------------------------*/

class LUscalarMatrix
:
    public scalarMatrix
{
    // Private data

        //- Processor matrix offsets
        labelList procOffsets_;

        //- The pivot indices used in the LU decomposition
        labelList pivotIndices_;

        //- Convert the given lduMatrix into this LUscalarMatrix
        void convert
        (
            const lduMatrix& ldum,
            const FieldField<Field, scalar>& interfaceCoeffs,
            const lduInterfaceFieldPtrsList& interfaces
        );

        //- Convert the given list of procLduMatrix into this LUscalarMatrix
        //  on the master processor
        void convert(const PtrList<procLduMatrix>& lduMatrices);


        //- Print the ratio of the mag-sum of the off-diagonal coefficients 
        //  to the mag-diagonal
        void printDiagonalDominance() const;


public:

    // Constructors

        //- Construct from Matrix<scalar> and perform LU decomposition
        LUscalarMatrix(const Matrix<scalar>&);

        //- Construct from lduMatrix and perform LU decomposition
        LUscalarMatrix
        (
            const lduMatrix&,
            const FieldField<Field, scalar>& interfaceCoeffs,
            const lduInterfaceFieldPtrsList& interfaces
        );


    // Member Functions

        //- Solve the matrix using the LU decomposition with pivoting
        //  returning the solution in the source
        template<class T>
        void solve(Field<T>& source) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "LUscalarMatrixTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
