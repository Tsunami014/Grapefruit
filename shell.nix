{ pkgs ? import <nixpkgs> {} }:

let
  uberApkSigner = pkgs.stdenv.mkDerivation rec {
    pname = "uber-apk-signer";
    version = "1.3.0";

    src = pkgs.fetchurl {
      url = "https://github.com/patrickfav/uber-apk-signer/releases/download/v${version}/uber-apk-signer-${version}.jar";
      sha256 = "sha256-4Smf1vz02lJ91Tc1tWEn6OqSKjIRKBI7nDLWGbuh2DU=";
    };

    dontUnpack = true;
    nativeBuildInputs = [ pkgs.makeWrapper ];
    installPhase = ''
      mkdir -p $out/share/java $out/bin
      cp $src $out/share/java/uber-apk-signer.jar

      makeWrapper ${pkgs.jre}/bin/java $out/bin/uber-apk-signer \
        --add-flags "-jar $out/share/java/uber-apk-signer.jar"
    '';
  };
in
pkgs.mkShell {
  name = "grapefruit-shell";

  nativeBuildInputs = with pkgs; [
    cmake
    ninja
    openjdk17
    python3
    androidenv.androidPkgs.androidsdk
    uberApkSigner
    glib glibc_multi
    libGL.dev libGL
  ];

  shellHook = ''
    export NIX_LD_LIBRARY_PATH="${pkgs.lib.makeLibraryPath [
      pkgs.glib
      pkgs.zlib
      pkgs.zstd
      pkgs.stdenv.cc.cc.lib
      pkgs.libGL
      pkgs.libxkbcommon
      pkgs.fontconfig
      pkgs.freetype
      pkgs.dbus

      pkgs.xorg.libX11
      pkgs.wayland
    ]}"

    export NIX_LD="${pkgs.stdenv.cc.libc}/lib/ld-linux-x86-64.so.2"
    export ANDROID_SDK_ROOT=$(nix-build '<nixpkgs>' -A androidenv.androidPkgs.androidsdk --no-out-link)/libexec/android-sdk

    export LD_LIBRARY_PATH="/run/opengl-driver/lib:$NIX_LD_LIBRARY_PATH:$LD_LIBRARY_PATH"

    [ ! -d ".venv" ] && python3 -m venv .venv
    . ./.venv/bin/activate
    pip install aqtinstall
  '';
}
