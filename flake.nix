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

          cairo
          xorg.libXi
          xorg.libX11
          xorg.xorgproto
          xorg.libXt
          xorg.libXfixes
          xorg.libXinerama
          xorg.libXrandr
          wayland
          wayland-protocols
        ];
      };

      packages.default = self.packages.${system}.activate-linux;
    });
}
