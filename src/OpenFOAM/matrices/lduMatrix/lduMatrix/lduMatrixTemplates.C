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
    lduMatrix member H operations.

\*---------------------------------------------------------------------------*/

#include "lduMatrix.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class Type>
Foam::tmp<Foam::Field<Type> > Foam::lduMatrix::H(const Field<Type>& psi) const
{
    tmp<Field<Type> > tHpsi
    (
        new Field<Type>(lduAddr().size(), pTraits<Type>::zero)
    );

    if (lowerPtr_ || upperPtr_)
    {
        Field<Type> & Hpsi = tHpsi();

        Type* __restrict__ HpsiPtr = Hpsi.begin();

        const Type* __restrict__ psiPtr = psi.begin();

        const label* __restrict__ uPtr = lduAddr().upperAddr().begin();
        const label* __restrict__ lPtr = lduAddr().lowerAddr().begin();

        const scalar* __restrict__ lowerPtr = lower().begin();
        const scalar* __restrict__ upperPtr = upper().begin();

        register const label nFaces = upper().size();

        for (register label face=0; face<nFaces; face++)
        {
            #ifdef ICC_IA64_PREFETCH
            __builtin_prefetch (&uPtr[face+32],0,0);
            __builtin_prefetch (&lPtr[face+32],0,0);
            __builtin_prefetch (&lowerPtr[face+32],0,1);
            __builtin_prefetch (&psiPtr[lPtr[face+32]],0,1);
            __builtin_prefetch (&HpsiPtr[uPtr[face+32]],0,1);
            #endif

            HpsiPtr[uPtr[face]] -= lowerPtr[face]*psiPtr[lPtr[face]];
        
            #ifdef ICC_IA64_PREFETCH
            __builtin_prefetch (&upperPtr[face+32],0,1);
            __builtin_prefetch (&psiPtr[uPtr[face+32]],0,1);
            __builtin_prefetch (&HpsiPtr[lPtr[face+32]],0,1);
            #endif

            HpsiPtr[lPtr[face]] -= upperPtr[face]*psiPtr[uPtr[face]];
        }
    }

    return tHpsi;
}

template<class Type>
Foam::tmp<Foam::Field<Type> >
Foam::lduMatrix::H(const tmp<Field<Type> >& tpsi) const
{
    tmp<Field<Type> > tHpsi(H(tpsi()));
    tpsi.clear();
    return tHpsi;
}


template<class Type>
Foam::tmp<Foam::Field<Type> >
Foam::lduMatrix::faceH(const Field<Type>& psi) const
{
    tmp<Field<Type> > tfaceHpsi(new Field<Type> (lduAddr().lowerAddr().size()));
    Field<Type>& faceHpsi = tfaceHpsi();

    if (lowerPtr_ || upperPtr_)
    {
        const scalarField& Lower = const_cast<const lduMatrix&>(*this).lower();
        const scalarField& Upper = const_cast<const lduMatrix&>(*this).upper();

        // Take refereces to addressing
        const unallocLabelList& l = lduAddr().lowerAddr();
        const unallocLabelList& u = lduAddr().upperAddr();

        for (register label face=0; face<l.size(); face++)
        {
            faceHpsi[face] = Upper[face]*psi[u[face]]
                - Lower[face]*psi[l[face]];
        }
    }
    else
    {
        // No off-diagonal.  Bug fix for conjugate matrices.  HJ, 27/Nov/2008
        faceHpsi = pTraits<Type>::zero;
    }

    return tfaceHpsi;
}


template<class Type>
Foam::tmp<Foam::Field<Type> >
Foam::lduMatrix::faceH(const tmp<Field<Type> >& tpsi) const
{
    tmp<Field<Type> > tfaceHpsi(faceH(tpsi()));
    tpsi.clear();
    return tfaceHpsi;
}


// ************************************************************************* //
