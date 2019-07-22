# This file defines Nix-expression accessible via `nix-shell` and `nix-build`
# commands. Search NixOS for details.

{ pkgs ?  import <nixpkgs> {} } :

pkgs.stdenv.mkDerivation {
  src = builtins.filterSource (path: type: type != "directory" || baseNameOf path != "build") ./.;
  name = "xkb-switch-env";
  buildInputs = (with pkgs; with xorg; [ cmake libX11 libxkbfile ]);
}
