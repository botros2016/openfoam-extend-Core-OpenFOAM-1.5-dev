    surfaceScalarField muEff
    (
        "muEff",
        twoPhaseProperties.muf()
      + fvc::interpolate(rho*turbulence->nuSgs())
    );

    fvVectorMatrix UEqn
    (
        fvm::ddt(rho, U)
      + fvm::div(phi, U)
      - fvm::laplacian(muEff, U)
    //- (fvc::grad(U) & fvc::grad(muf))
      - fvc::div(muEff*(fvc::interpolate(dev(fvc::grad(U))) & mesh.Sf()))
    );

    if (momentumPredictor)
    {
        solve(UEqn == -fvc::grad(p));
    }
