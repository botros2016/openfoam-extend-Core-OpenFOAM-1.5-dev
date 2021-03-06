// The FOAM Project // File: layerSmooth.H
/*  
-------------------------------------------------------------------------------
 =========         | Class Interface
 \\      /         |
  \\    /          | Name:   lateralValves
   \\  /           | Family: engine
    \\/            |
    F ield         | FOAM version: 2.3
    O peration     |
    A and          | Copyright (C) 1991-2004 Nabla Ltd.
    M anipulation  |          All Rights Reserved.
-------------------------------------------------------------------------------
CLASS
    layerSmooth

DESCRIPTION
    Engine mesh class for canted valves engine. Layers are added/removed on the
    piston, while mesh is deformed close to the valves. Attach/detach boundary
    is used. There are no sliding interfaces.

C++
    listOfSourceFiles
        addLayerSmoothMeshModifiers.C
        layerSmooth.C
        layerSmoothCalculate.C
        layerSmoothInitialize.C
        layerSmoothMove.C
    endListOfSourceFiles

AUTHOR
	Tommaso Lucchini

*/
// ------------------------------------------------------------------------- //

#ifndef layerSmooth_H
#define layerSmooth_H

#include "engineTopoChangerMesh.H"
#include "simpleEnginePiston.H"
#include "motionSolver.H"
#include "polyPatchID.H"
#include "verticalValveBank.H"
#include "twoDPointCorrector.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Class forward declarations

/*---------------------------------------------------------------------------*\
                           Class lateralValves Declaration
\*---------------------------------------------------------------------------*/
class twoDPointCorrector;

class layerSmooth
:
    public engineTopoChangerMesh
{
    // Private data

        //- Piston patch
        simpleEnginePiston piston_;

        //- Lateral valves
        verticalValveBank valves_;

        //- Layering-to-deformation switch in crank-angle degrees
        scalar deformSwitch_;

        //- Tolerance used when the piston faceZone is created
        scalar delta_;

        //- Distance from the piston patch
        scalar offSet_;

        //- Piston Position
        scalar pistonPosition_;

        //- Virtual piston position (pistonPosition + offSet)
        scalar virtualPistonPosition_;

        //- deckHeight
        scalar deckHeight_;

        //- Motion solver
        autoPtr<motionSolver> msPtr_;
               
        //- tolerance for attach/detach in the valve plane
        scalar valvePlaneTol_;
        
        
    // Private Member Functions

        //- Disallow default bitwise copy construct
        layerSmooth(const layerSmooth&);

        //- Disallow default bitwise assignment
        void operator=(const layerSmooth&);

        //- Make layering modifiers live
        void makeLayersLive();
        
        //- Prepare valve attach/detach
        void valveDetach();

        //- Prepare valve attach/detach
        void valveAttach();

        //- Prepare valve attach/detach
        void prepareValveDetach();

        //- Check if all patches exist, then calculate virtualPistonPosition, 
        //- pistonPosition and deckHeight for the first time
        void checkAndCalculate();
        
        //- Calculate the virtualPistonPosition, 
        void setVirtualPositions();
                
        bool realDeformation() const;
                        
        bool inCantedValve(const point& p, const label& i) const;

        bool isACylinderHeadFace(const labelList& cylHeadFaces, const label face);
                        
public:

    //- Runtime type information
    TypeName("layerSmooth");


    // Constructors

        //- Construct from database
        explicit layerSmooth(const IOobject& io);


    // Destructor - default


    // Member Functions

        inline const simpleEnginePiston& piston() const;
        inline const verticalValveBank& valves() const;
        inline const scalar& deformSwitch() const;
        inline const scalar& delta() const;
        inline const scalar& offSet() const;
        inline const scalar& pistonPosition() const;
        inline scalar& pistonPosition();
        inline const scalar& virtualPistonPosition() const;
        inline scalar& virtualPistonPosition();
        inline const scalar& deckHeight() const;
        inline scalar& deckHeight();


        //- Return true for mesh deformation mode
        bool deformation() const
        {
            return
                engTime().thetaRevolution() > -deformSwitch_
             && engTime().thetaRevolution() < deformSwitch_;
        }

        //- Return number of valves
        label nValves() const
        {
            return valves_.size();
        }

        //- Add valve and piston zones and modifiers
        void addZonesAndModifiers();

        //- Move and morph
        virtual bool update();

        //- Set boundary velocities
        void setBoundaryVelocity(volVectorField& U);

        //- Generate the mesh from a series of square blocks
        void generateMesh(){};
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
#include "layerSmoothI.H"

#endif

// ************************************************************************* //
