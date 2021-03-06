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

\*---------------------------------------------------------------------------*/

namespace Foam
{

// * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * * //

template<class Type>
tmp<faPatchField<Type> > faPatchField<Type>::New
(
    const word& patchFieldType,
    const faPatch& p,
    const DimensionedField<Type, areaMesh>& iF
)
{
    if (debug)
    {
        Info<< "faPatchField<Type>::New(const word&, const faPatch&, "
               "const DimensionedField<Type, areaMesh>&) : "
               "constructing faPatchField<Type>"
            << endl;
    }

    typename patchConstructorTable::iterator cstrIter =
        patchConstructorTablePtr_->find(patchFieldType);

    if (cstrIter == patchConstructorTablePtr_->end())
    {
        FatalErrorIn
        (
            "faPatchField<Type>::New(const word&, const faPatch&, "
            "const DimensionedField<Type, areaMesh>&)"
        )   << "Unknown patchTypefield type " << patchFieldType
            << endl << endl
            << "Valid patchField types are :" << endl
            << patchConstructorTablePtr_->toc()
            << exit(FatalError);
    }

    typename patchConstructorTable::iterator patchTypeCstrIter =
        patchConstructorTablePtr_->find(p.type());

    if (patchTypeCstrIter != patchConstructorTablePtr_->end())
    {
        return patchTypeCstrIter()(p, iF);
    }
    else
    {
        return cstrIter()(p, iF);
    }
}


template<class Type>
tmp<faPatchField<Type> > faPatchField<Type>::New
(
    const faPatch& p,
    const DimensionedField<Type, areaMesh>& iF,
    const dictionary& dict
)
{
    if (debug)
    {
        Info<< "faPatchField<Type>::New(const faPatch&, "
               "const DimensionedField<Type, areaMesh>&, const dictionary&) : "
               "constructing faPatchField<Type>"
            << endl;
    }

    word patchFieldType(dict.lookup("type"));

    typename dictionaryConstructorTable::iterator cstrIter
        = dictionaryConstructorTablePtr_->find(patchFieldType);

    if (cstrIter == dictionaryConstructorTablePtr_->end())
    {
        if (!disallowDefaultFaPatchField)
        {
            cstrIter = dictionaryConstructorTablePtr_->find("default");
        }

        if (cstrIter == dictionaryConstructorTablePtr_->end())
        {
            FatalIOErrorIn
            (
                "faPatchField<Type>::New(const faPatch&, "
                "const DimensionedField<Type, areaMesh>&, const dictionary&)",
                dict
            )   << "Unknown patchField type " << patchFieldType
                << " for patch type " << p.type() << endl << endl
                << "Valid patchField types are :" << endl
                << dictionaryConstructorTablePtr_->toc()
                << exit(FatalIOError);
        }
    }

    typename dictionaryConstructorTable::iterator patchTypeCstrIter
        = dictionaryConstructorTablePtr_->find(p.type());

    if
    (
        patchTypeCstrIter != dictionaryConstructorTablePtr_->end()
     && *patchTypeCstrIter != *cstrIter
    )
    {
        FatalIOErrorIn
        (
            "faPatchField<Type>const faPatch&, "
            "const DimensionedField<Type, areaMesh>&, const dictionary&)",
            dict
        ) << "inconsistent patch and patchField types for \n"
             "    patch type " << p.type()
            << " and patchField type " << patchFieldType
            << exit(FatalIOError);
    }

    return cstrIter()(p, iF, dict);
}


template<class Type>
tmp<faPatchField<Type> > faPatchField<Type>::New
(
    const faPatchField<Type>& ptf,
    const faPatch& p,
    const DimensionedField<Type, areaMesh>& iF,
    const faPatchFieldMapper& pfMapper
)
{
    if (debug)
    {
        Info<< "faPatchField<Type>::New(const faPatchField<Type>&,"
               " const faPatch&, const DimensionedField<Type, areaMesh>&, "
               "const faPatchFieldMapper&) : "
               "constructing faPatchField<Type>"
            << endl;
    }

    typename patchMapperConstructorTable::iterator cstrIter =
        patchMapperConstructorTablePtr_->find(ptf.type());

    if (cstrIter == patchMapperConstructorTablePtr_->end())
    {
        FatalErrorIn
        (
            "faPatchField<Type>::New(const faPatchField<Type>&, "
            "const faPatch&, const DimensionedField<Type, areaMesh>&, "
            "const faPatchFieldMapper&)"
        )   << "unknown patchTypefield type " << ptf.type() << endl << endl
            << "Valid patchField types are :" << endl
            << patchMapperConstructorTablePtr_->toc()
            << exit(FatalError);
    }

    typename patchMapperConstructorTable::iterator
        patchTypeCstrIter = patchMapperConstructorTablePtr_->find(p.type());

    if (patchTypeCstrIter != patchMapperConstructorTablePtr_->end())
    {
        return patchTypeCstrIter()(ptf, p, iF, pfMapper);
    }
    else
    {
        return cstrIter()(ptf, p, iF, pfMapper);
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
