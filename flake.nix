{
  description = "My project description";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachSystem ["x86_64-linux" ](system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        stdenv = pkgs.clangStdenv;
      in
      {
        # Development environment
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            cmake
            ninja
            gtest
            clang-tools
            valgrind
          ];

          shellHook = ''
            echo "Entering development environment"
            export GTEST_ROOT="${pkgs.gtest}"
          '';
        };

        # Build package
       packages.default = stdenv.mkDerivation {
          name = "glom";
          version = "0.1.0";
          src = ./.;

          nativeBuildInputs = with pkgs; [
               cmake
               ninja
               clang-tools
          ];

          dontUseCmakeBuild = true;

          cmakeFlags = [
            "-G Ninja"
            "-DCMAKE_BUILD_TYPE=Release"
            "-DBUILD_TESTING=OFF"
            "-DCMAKE_CXX_FLAGS=\"-O3\""
          ];

          configurePhase = ''
            runHook preConfigure
            cmake -B build $cmakeFlags
            runHook postConfigure
          '';

          buildPhase = ''
            runHook preBuild
            cd build
            ninja glom_exe
            runHook postBuild
          '';

          installPhase = ''
            runHook preInstall
            mkdir -p $out/bin
            cp src/glom_exe $out/bin/glom
            runHook postInstall
          '';
        };

        apps.default = {
            type = "app";
            program = "${self.packages.${system}.default}/bin/glom";
            meta = {
              description = "Glom demo app";
              mainProgram = "glom";
              license = pkgs.lib.licenses.mit;
            };
        };

        # Test package
        checks.test = stdenv.mkDerivation {
          name = "glom-tests";

          src = ./.;

         nativeBuildInputs = with pkgs; [
              cmake
              ninja
              clang-tools
              gtest
         ];

         dontUseCmakeBuild = true;

          cmakeFlags = [
            "-G Ninja"
            "-DCMAKE_BUILD_TYPE=Debug"
            "-DBUILD_TESTING=ON"
          ];

         configurePhase = ''
           runHook preConfigure
           export GTEST_ROOT="${pkgs.gtest}"
           cmake -B build $cmakeFlags
           runHook postConfigure
         '';

         buildPhase = ''
           runHook preBuild
           cd build
           ninja build-test
           runHook postBuild
         '';

          doCheck = true;
          checkPhase = ''
            runHook preCheck
            ninja test --verbose
            runHook postCheck
          '';

          installPhase = ''
              runHook preInstall

              mkdir -p $out
              echo "Tests completed successfully" > $out/TESTS_PASSED

              runHook postInstall
          '';
        };
      }
    );
}