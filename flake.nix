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
      };

      packages.default = self.packages.${system}.activate-linux;
    });
}
