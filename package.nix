{
  lib,
  cairo,
  gawk,
  libconfig,
  libx11,
  libxext,
  libxfixes,
  libxi,
  libxinerama,
  libxrandr,
  libxt,
  pango,
  pkg-config,
  stdenv,
  wayland,
  wayland-protocols,
  wayland-scanner,
  xorgproto,
}:
stdenv.mkDerivation {
  name = "activate-linux";
  src = lib.cleanSource ./.;

  makeFlags = [ "PREFIX=$out" ];
  nativeBuildInputs = [
    gawk
    pkg-config
    wayland-scanner
  ];
  buildInputs = [
    cairo
    libconfig
    libx11
    libxext
    libxfixes
    libxi
    libxinerama
    libxrandr
    libxt
    pango
    wayland
    wayland-protocols
    xorgproto
  ];

  installPhase = ''
    runHook preInstall
    install -D {.,$out/bin}/activate-linux
    install -D {.,$out/share/man/man1}/activate-linux.1
    pushd res
    install -D {.,$out/share/icons/hicolor/128x128/apps}/activate-linux.png
    install -D {.,$out/share/applications}/activate-linux.desktop
    popd
    runHook postInstall
  '';

  meta = {
    description = ''The "Activate Windows" watermark ported to Linux'';
    homepage = "https://github.com/MrGlockenspiel/activate-linux";
    license = lib.licenses.gpl3;
    platforms = lib.platforms.linux;
    mainProgram = "activate-linux";
  };
}
