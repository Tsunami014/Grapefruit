{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  name = "grapefruit-shell";

  nativeBuildInputs = with pkgs; [
    cmake
    ninja
    openjdk17
    python3
    androidenv.androidPkgs.androidsdk
    glib glibc_multi
    # For desktop building
    qt6.qtbase qt6.qtbase.dev
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
    ]}"

    export NIX_LD="${pkgs.stdenv.cc.libc}/lib/ld-linux-x86-64.so.2"
    export ANDROID_SDK_ROOT=$(nix-build '<nixpkgs>' -A androidenv.androidPkgs.androidsdk --no-out-link)/libexec/android-sdk

    export LD_LIBRARY_PATH="/run/opengl-driver/lib:${pkgs.libGL}/lib:$LD_LIBRARY_PATH"

    python3 -m venv .venv
    . ./.venv/bin/activate
    pip install aqtinstall
  '';
}
