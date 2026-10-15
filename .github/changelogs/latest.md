# Version: 2026.10.163055558

* [3a493ee](https://github.com/ford-jones/lazarus/commit/3a493eec09ecb45cee6342027bf24611f692ed82): Fix: Smoothen the visual transition of up-vector-flips during camera rotation
* [d79b9c1](https://github.com/ford-jones/lazarus/commit/d79b9c1760a9f86a7fae740784da45e6e60a9e9b): Docs: Adds particle system workflows to documentation
* [06c27d4](https://github.com/ford-jones/lazarus/commit/06c27d44201ca11d91a4ae9ac7cd69af075a7c61): Clean: Removed immutable internal particle data from userspace
* [e58f789](https://github.com/ford-jones/lazarus/commit/e58f789b42a99bae381152c11eb475a039ec3029): Fix: Ensures consistent particle travel distances regardless of particle size
* [b4cd930](https://github.com/ford-jones/lazarus/commit/b4cd930ab2007a47085e0cc5520ea7628551a857): Update: Billboards sprite based particles so that they always face the viewport
* [5ef951b](https://github.com/ford-jones/lazarus/commit/5ef951bec2cd213ffb17dce2afda5f8f283065f2): Docs: Cleaned up old documentation
* [d83e582](https://github.com/ford-jones/lazarus/commit/d83e582a936b40c14c44e210d2376bdb8a330d61): Update: Improved validation of WorldFX inputs
* [2bd7375](https://github.com/ford-jones/lazarus/commit/2bd7375f2d7c0147e7947e87645087a008ea1278): Feat: Particles use a variable rate-of-decay set from userspace
* [3d4d537](https://github.com/ford-jones/lazarus/commit/3d4d5377f54dd00fdfc8988c3bef360ad8322522): Feat: Updated the default shader program to include alpha-transparency-out instead of discard-only behaviour
* [0af0453](https://github.com/ford-jones/lazarus/commit/0af0453f262fc730d44aafafddf10c2028d78e3a): Clean: Removed unnecessary instance data
* [4e88ec5](https://github.com/ford-jones/lazarus/commit/4e88ec50af1ce06d20ae19321ab0b67b5b2c03b5): Feat: Uses glm to assist with spherical scattering during particle initialisation
* [8463820](https://github.com/ford-jones/lazarus/commit/846382099593ac48c90d159acb6a327e82cf9b15): Feat: Introduces new particle management structures
* [d2f4664](https://github.com/ford-jones/lazarus/commit/d2f46644fc4b4a1eca8c964b8e9846504b4d6628): Fix: Enabled texture transparency in sponza sample program
* [4dac8be](https://github.com/ford-jones/lazarus/commit/4dac8bebd2a9f35c35c1b89c111fc8efa92d803b): Fix: Ensures that wayland graphical sessions dont have their windows repositioned at any point during window creation
* [4bf6c0f](https://github.com/ford-jones/lazarus/commit/4bf6c0f4336de8d0f1ec706a4e6bee0bc61b2c87): Clean: Makes use of inherited protected TextureLoader members instead of redeclaring them on the mesh level
* [088b2a1](https://github.com/ford-jones/lazarus/commit/088b2a17d34dbe8312d76800bfb1eebe15d87368): Feat: Adds a helper function for checking file extensions