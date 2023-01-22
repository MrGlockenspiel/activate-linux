{
  description = "The \"Activate Windows\" watermark ported to Linux";
  inputs = {
    nixpkgs = {
      url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    };
    flake-utils = {
      url = "github:numtide/flake-utils";
    };
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
    ...
  }:
    flake-utils.lib.eachDefaultSystem (system: let
      pkgs = nixpkgs.legacyPackages.${system};
    in {
      packages.activate-linux = pkgs.stdenv.mkDerivation {
        name = "activate-linux";
        src = pkgs.lib.cleanSource ./.;

        makeFlags = [
          "PREFIX=/"
          "DESTDIR=${placeholder "out"}"
          "CC=${pkgs.stdenv.cc}/bin/cc"
        ];

        buildInputs = with pkgs; [
          gawk
          pkg-config
        ] ++ (import ./deps.nix {inherit pkgs;});

        installPhase = ''
          runHook preInstall

          mkdir -p $out/bin
          mkdir -p $out/share/man/man1

          cp activate-linux $out/bin
          cp activate-linux.1 $out/share/man/man1

          runHook postInstall
        '';
      };

      packages.default = self.packages.${system}.activate-linux;
    });
}
