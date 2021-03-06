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

#include "fvSchemes.H"
#include "Time.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

int fvSchemes::debug(Foam::debug::debugSwitch("fvSchemes", false));

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

fvSchemes::fvSchemes(const objectRegistry& obr)
:
    IOdictionary
    (
        IOobject
        (
            "fvSchemes",
            obr.time().system(),
            obr,
//             IOobject::MUST_READ,
            IOobject::READ_IF_PRESENT,  // Allow default dictionary creation
            IOobject::NO_WRITE
        )
    ),
    ddtSchemes_
    (
        ITstream(objectPath() + "::ddtSchemes",
        tokenList())()
    ),
    defaultDdtScheme_
    (
        ddtSchemes_.name() + "::default",
        tokenList()
    ),
    d2dt2Schemes_
    (
        ITstream(objectPath() + "::d2dt2Schemes",
        tokenList())()
    ),
    defaultD2dt2Scheme_
    (
        d2dt2Schemes_.name() + "::default",
        tokenList()
    ),
    interpolationSchemes_
    (
        ITstream(objectPath() + "::interpolationSchemes",
        tokenList())()
    ),
    defaultInterpolationScheme_
    (
        interpolationSchemes_.name() + "::default",
        tokenList()
    ),
    divSchemes_
    (
        ITstream(objectPath() + "::divSchemes",
        tokenList())()
    ),
    defaultDivScheme_
    (
        divSchemes_.name() + "::default",
        tokenList()
    ),
    gradSchemes_
    (
        ITstream(objectPath() + "::gradSchemes",
        tokenList())()
    ),
    defaultGradScheme_
    (
        gradSchemes_.name() + "::default",
        tokenList()
    ),
    snGradSchemes_
    (
        ITstream(objectPath() + "::snGradSchemes",
        tokenList())()
    ),
    defaultSnGradScheme_
    (
        snGradSchemes_.name() + "::default",
        tokenList()
    ),
    laplacianSchemes_
    (
        ITstream(objectPath() + "::laplacianSchemes",
        tokenList())()
    ),
    defaultLaplacianScheme_
    (
        laplacianSchemes_.name() + "::default",
        tokenList()
    ),
    fluxRequired_
    (
        ITstream(objectPath() + "::fluxRequired",
        tokenList())()
    ),
    defaultFluxRequired_(false)
{
    if (!headerOk())
    {
        if (debug)
        {
            InfoIn
            (
                "fvSchemes::fvSchemes(const objectRegistry& obr)"
            )   << "fvSchemes dictionary not found.  Creating default."
                << endl;
        }

        regIOobject::write();
    }

    read();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool fvSchemes::read()
{
    if (regIOobject::read())
    {
        const dictionary& dict = schemesDict();

        if (dict.found("ddtSchemes"))
        {
            ddtSchemes_ = dict.subDict("ddtSchemes");
        }
        else if (dict.found("timeScheme"))
        {
            // For backward compatibility.
            // The timeScheme will be deprecated with warning or removed

            word timeSchemeName(dict.lookup("timeScheme"));

            if (timeSchemeName == "EulerImplicit")
            {
                timeSchemeName = "Euler";
            }
            else if (timeSchemeName == "BackwardDifferencing")
            {
                timeSchemeName = "backward";
            }
            else if (timeSchemeName == "SteadyState")
            {
                timeSchemeName = "steadyState";
            }
            else
            {
                FatalIOErrorIn("fvSchemes::read()", dict.lookup("timeScheme"))
                    << "\n    Only EulerImplicit, BackwardDifferencing and "
                       "SteadyState\n    are supported by the old timeScheme "
                       "specification.\n    Please use ddtSchemes instead."
                    << exit(FatalIOError);
            }

            if (ddtSchemes_.found("default"))
            {
                ddtSchemes_.remove("default");
            }

            ddtSchemes_.add("default", timeSchemeName);

            ddtSchemes_.lookup("default")[0].lineNumber() =
                dict.lookup("timeScheme").lineNumber();
        }
        else
        {
            ddtSchemes_.add("default", "none");
        }

        if
        (
            ddtSchemes_.found("default")
         && word(ddtSchemes_.lookup("default")) != "none"
        )
        {
            defaultDdtScheme_ = ddtSchemes_.lookup("default");
        }


        if (dict.found("d2dt2Schemes"))
        {
            d2dt2Schemes_ = dict.subDict("d2dt2Schemes");
        }
        else if (dict.found("timeScheme"))
        {
            // For backward compatibility.
            // The timeScheme will be deprecated with warning or removed

            word timeSchemeName(dict.lookup("timeScheme"));

            if (timeSchemeName == "EulerImplicit")
            {
                timeSchemeName = "Euler";
            }
            else if (timeSchemeName == "SteadyState")
            {
                timeSchemeName = "steadyState";
            }

            if (d2dt2Schemes_.found("default"))
            {
                d2dt2Schemes_.remove("default");
            }

            d2dt2Schemes_.add("default", timeSchemeName);

            d2dt2Schemes_.lookup("default")[0].lineNumber() =
                dict.lookup("timeScheme").lineNumber();
        }
        else
        {
            d2dt2Schemes_.add("default", "none");
        }

        if
        (
            d2dt2Schemes_.found("default")
         && word(d2dt2Schemes_.lookup("default")) != "none"
        )
        {
            defaultD2dt2Scheme_ = d2dt2Schemes_.lookup("default");
        }


        if (dict.found("interpolationSchemes"))
        {
            interpolationSchemes_ = dict.subDict("interpolationSchemes");
        }
        else if (!interpolationSchemes_.found("default"))
        {
            interpolationSchemes_.add("default", "linear");
        }

        if
        (
            interpolationSchemes_.found("default")
         && word(interpolationSchemes_.lookup("default")) != "none"
        )
        {
            defaultInterpolationScheme_ =
                interpolationSchemes_.lookup("default");
        }


        divSchemes_ = dict.subDict("divSchemes");

        if
        (
            divSchemes_.found("default")
         && word(divSchemes_.lookup("default")) != "none"
        )
        {
            defaultDivScheme_ = divSchemes_.lookup("default");
        }


        gradSchemes_ = dict.subDict("gradSchemes");

        if
        (
            gradSchemes_.found("default")
         && word(gradSchemes_.lookup("default")) != "none"
        )
        {
            defaultGradScheme_ = gradSchemes_.lookup("default");
        }


        if (dict.found("snGradSchemes"))
        {
            snGradSchemes_ = dict.subDict("snGradSchemes");
        }
        else if (!snGradSchemes_.found("default"))
        {
            snGradSchemes_.add("default", "corrected");
        }

        if
        (
            snGradSchemes_.found("default")
         && word(snGradSchemes_.lookup("default")) != "none"
        )
        {
            defaultSnGradScheme_ = snGradSchemes_.lookup("default");
        }


        laplacianSchemes_ = dict.subDict("laplacianSchemes");

        if
        (
            laplacianSchemes_.found("default")
         && word(laplacianSchemes_.lookup("default")) != "none"
        )
        {
            defaultLaplacianScheme_ = laplacianSchemes_.lookup("default");
        }


        if (dict.found("fluxRequired"))
        {
            fluxRequired_ = dict.subDict("fluxRequired");

            if
            (
                fluxRequired_.found("default")
             && word(fluxRequired_.lookup("default")) != "none"
            )
            {
                defaultFluxRequired_ = Switch(fluxRequired_.lookup("default"));
            }
        }

        return true;
    }
    else
    {
        return false;
    }
}


const dictionary& fvSchemes::schemesDict() const
{
    if (found("select"))
    {
        return subDict(word(lookup("select")));
    }
    else
    {
        return *this;
    }
}


ITstream& fvSchemes::ddtScheme(const word& name) const
{
    if (debug)
    {
        Info<< "Lookup ddtScheme for " << name << endl;
    }

    if
    (
        ddtSchemes_.found(name)
     || !defaultDdtScheme_.size()
    )
    {
        return ddtSchemes_.lookup(name);
    }
    else
    {
        const_cast<ITstream&>(defaultDdtScheme_).rewind();
        return const_cast<ITstream&>(defaultDdtScheme_);
    }
}


ITstream& fvSchemes::d2dt2Scheme(const word& name) const
{
    if (debug)
    {
        Info<< "Lookup d2dt2Scheme for " << name << endl;
    }

    if
    (
        d2dt2Schemes_.found(name)
     || !defaultD2dt2Scheme_.size()
    )
    {
        return d2dt2Schemes_.lookup(name);
    }
    else
    {
        const_cast<ITstream&>(defaultD2dt2Scheme_).rewind();
        return const_cast<ITstream&>(defaultD2dt2Scheme_);
    }
}


ITstream& fvSchemes::interpolationScheme(const word& name) const
{
    if (debug)
    {
        Info<< "Lookup interpolationScheme for " << name << endl;
    }

    if
    (
        interpolationSchemes_.found(name)
     || !defaultInterpolationScheme_.size()
    )
    {
        return interpolationSchemes_.lookup(name);
    }
    else
    {
        const_cast<ITstream&>(defaultInterpolationScheme_).rewind();
        return const_cast<ITstream&>(defaultInterpolationScheme_);
    }
}


ITstream& fvSchemes::divScheme(const word& name) const
{
    if (debug)
    {
        Info<< "Lookup divScheme for " << name << endl;
    }

    if (divSchemes_.found(name) || !defaultDivScheme_.size())
    {
        return divSchemes_.lookup(name);
    }
    else
    {
        const_cast<ITstream&>(defaultDivScheme_).rewind();
        return const_cast<ITstream&>(defaultDivScheme_);
    }
}


ITstream& fvSchemes::gradScheme(const word& name) const
{
    if (debug)
    {
        Info<< "Lookup gradScheme for " << name << endl;
    }

    if (gradSchemes_.found(name) || !defaultGradScheme_.size())
    {
        return gradSchemes_.lookup(name);
    }
    else
    {
        const_cast<ITstream&>(defaultGradScheme_).rewind();
        return const_cast<ITstream&>(defaultGradScheme_);
    }
}


ITstream& fvSchemes::snGradScheme(const word& name) const
{
    if (debug)
    {
        Info<< "Lookup snGradScheme for " << name << endl;
    }

    if (snGradSchemes_.found(name) || !defaultSnGradScheme_.size())
    {
        return snGradSchemes_.lookup(name);
    }
    else
    {
        const_cast<ITstream&>(defaultSnGradScheme_).rewind();
        return const_cast<ITstream&>(defaultSnGradScheme_);
    }
}


ITstream& fvSchemes::laplacianScheme(const word& name) const
{
    if (debug)
    {
        Info<< "Lookup laplacianScheme for " << name << endl;
    }

    if (laplacianSchemes_.found(name) || !defaultLaplacianScheme_.size())
    {
        return laplacianSchemes_.lookup(name);
    }
    else
    {
        const_cast<ITstream&>(defaultLaplacianScheme_).rewind();
        return const_cast<ITstream&>(defaultLaplacianScheme_);
    }
}


bool fvSchemes::fluxRequired(const word& name) const
{
    if (debug)
    {
        Info<< "Lookup fluxRequired for " << name << endl;
    }

    if (fluxRequired_.found(name))
    {
        return true;
    }
    else
    {
        return defaultFluxRequired_;
    }
}


bool fvSchemes::writeData(Ostream& os) const
{
    // Write dictionaries
    os << nl << "ddtSchemes";
    ddtSchemes_.write(os, true);

    os << nl << "d2dt2Schemes";
    d2dt2Schemes_.write(os, true);

    os << nl << "interpolationSchemes";
    interpolationSchemes_.write(os, true);

    os << nl << "divSchemes";
    divSchemes_.write(os, true);

    os << nl << "gradSchemes";
    gradSchemes_.write(os, true);

    os << nl << "snGradSchemes";
    snGradSchemes_.write(os, true);

    os << nl << "laplacianSchemes";
    laplacianSchemes_.write(os, true);

    os << nl << "fluxRequired";
    fluxRequired_.write(os, true);

    return true;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
