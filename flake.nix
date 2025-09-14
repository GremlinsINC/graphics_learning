{
  description = "Vulkan playground on NixOS (C++ + GLFW)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs { inherit system; };
  in {
    devShells.${system}.default = pkgs.mkShell {
      buildInputs = [
        pkgs.vulkan-loader
        pkgs.vulkan-headers
        pkgs.vulkan-validation-layers
        pkgs.vulkan-tools
        pkgs.glfw-wayland
        pkgs.gcc
        pkgs.cmake
        pkgs.gdb
      ];
      shellHook = ''
        echo "🚀 Vulkan dev environment loaded!"
        echo "Try running: vulkaninfo | less"
      '';
    };
  };
}

