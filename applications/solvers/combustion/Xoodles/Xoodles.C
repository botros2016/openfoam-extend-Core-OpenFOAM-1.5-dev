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

Application
    Xoodles

Description
    Compressible premixed/partially-premixed combustion solver with large-eddy
    simulation (LES) turbulence modelling.

    Combusting LES code using the b-Xi two-equation model.
    Xi may be obtained by either the solution of the Xi transport
    equation or from an algebraic exression.  Both approaches are
    based on Gulder's flame speed correlation which has been shown
    to be appropriate for LES by comparison with the results from the
    spectral model.

    Strain effects are encorporated directly into the Xi equation
    but not in the algebraic approximation.  Further work need to be
    done on this issue, particularly regarding the enhanced removal rate
    caused by flame compression.  Analysis using results of the spectral
    model will be required.

    For cases involving very lean Propane flames or other flames which are
    very strain-sensitive, a transport equation for the laminar flame
    speed is present.  This equation is derived using heuristic arguments
    involving the strain time scale and the strain-rate at extinction.
    the transport velocity is the same as that for the Xi equation.

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "hhuCombustionThermo.H"
#include "compressible/LESModel/LESModel.H"
#include "laminarFlameSpeed.H"
#include "ignition.H"
#include "IFstream.H"
#include "OFstream.H"

#define divDevRhoReff divDevRhoBeff

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "setRootCase.H"

    #include "createTime.H"
    #include "createMeshNoClear.H"
    #include "readEnvironmentalProperties.H"
    #include "createFields.H"
    #include "readPISOControls.H"
    #include "readCombustionProperties.H"
    #include "initContinuityErrs.H"

    Info<< "\nStarting time loop\n" << endl;

    for (runTime++; !runTime.end(); runTime++)
    {
        Info<< "Time = " << runTime.timeName() << nl << endl;

        #include "compressibleCourantNo.H"

        #include "rhoEqn.H"

        turbulence->correct();

        #include "UEqn.H"

        // --- PISO loop
        for (int corr=1; corr<=nCorr; corr++)
        {
            #include "ftEqn.H"
            #include "bEqn.H"
            #include "huEqn.H"
            #include "hEqn.H"

            if (!ign.ignited())
            {
                hu == h;
            }

            #include "pEqn.H"
        }

        runTime.write();

        rho = thermo->rho();

        Info<< "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
            << "  ClockTime = " << runTime.elapsedClockTime() << " s"
            << nl << endl;
    }

    Info<< "End\n" << endl;

    return(0);
}


// ************************************************************************* //
