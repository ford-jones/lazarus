# Version: 2025.08.10042615

* [d53f08f](https://github.com/ford-jones/lazarus/commit/d53f08f80566696148ee78d4189ff8f7abcbbb84): Update changelog.yml
* [c7c988a](https://github.com/ford-jones/lazarus/commit/c7c988a675f0efd378da68a2a5186a54858d7842): Update: Optimise index-buffer creation. Ensure O(log(n)) lookup time over O(n^2)
* [3d1f867](https://github.com/ford-jones/lazarus/commit/3d1f86708081c7147266ccb9b9b31e6e0052691c): Docs: Update API-reference and comments
* [fb4afac](https://github.com/ford-jones/lazarus/commit/fb4afacbecc05bbc62655a33ce9509c93e8d65f8): Update: Implements maps over vectors for faster indices lookups during mesh-load
* [a6ea435](https://github.com/ford-jones/lazarus/commit/a6ea43566fa528443d0c9f88d5563eac11e1a3ac): Fix: Explicitly declare iterator size on short-range loop for Windows compatibility
* [5644acb](https://github.com/ford-jones/lazarus/commit/5644acbff63255c3f887fd2baa80da18e4c5f0fc): Fix: Removed unstandardised types for MacOS compatibility
* [e15338d](https://github.com/ford-jones/lazarus/commit/e15338db0e1c49f024cfacb99198e07503dddb1f): Fix: Disabled image flipping on-load when loading from memory.
* [a6eb0cc](https://github.com/ford-jones/lazarus/commit/a6eb0ccd0fc734dd357ce58a030c017496205ee7): Clean: Updated comments and naming conventions
* [9f8d8bc](https://github.com/ford-jones/lazarus/commit/9f8d8bcbb3e6d8c0d9cac7d165e8a1ddbd78e4d0): Feat: Dynamically handles bytestride (interleaved) vertex attributes as well as tightly-packed.
* [f5c65b7](https://github.com/ford-jones/lazarus/commit/f5c65b71932cce47e61185f251cac38149250d1f): Fix: Restored per-material indices serialisation
* [7337370](https://github.com/ford-jones/lazarus/commit/7337370dd52d9b7773706f3afb7c5537c54365c6): Docs: Update resources
* [88e77ea](https://github.com/ford-jones/lazarus/commit/88e77ea71f1ce96647b92cb5d647d451acd4c718): Update: Handles both 32 and 16 bit glb indices values
* [3e43b1b](https://github.com/ford-jones/lazarus/commit/3e43b1b21c7b3af5ab5fcfe835a36f575ec0f4f9): Feat: Parses embeded image textures from glb binary chunks
* [4253f14](https://github.com/ford-jones/lazarus/commit/4253f14bf23ff9ff6cbae1942d11fac5d1b8f2ea): Update: Enabled decoding of image data from memory
* [edfd662](https://github.com/ford-jones/lazarus/commit/edfd6623696dc39c4ddacd0166850336f4094c6b): Feat: Debugger and optimisation makefile scripts.
* [9b61654](https://github.com/ford-jones/lazarus/commit/9b61654ae8ccd040cb6a24ce5fd73dcb1453bf21): Fix: Resolved stack overflow issues caused by glb vertex containers
* [50efabb](https://github.com/ford-jones/lazarus/commit/50efabb0ba97bdd4a9c749ce5d28ec74dc8b7c1d): Update: Enabled compiler optimisations
* [9509de6](https://github.com/ford-jones/lazarus/commit/9509de6fa477810948f9f2c02744977ac606d637): Update: Enabled compiler optimisations
* [857824f](https://github.com/ford-jones/lazarus/commit/857824f8628e0b05a14baac31ce59cff038387ec): Feat: Added support for glb mesh assets.
* [83bf763](https://github.com/ford-jones/lazarus/commit/83bf763adc3831379b5aaec54bdbf82b4d0272df): Update: Use file extension checks to dynamically load 3D asset parsers.
* [19c60e1](https://github.com/ford-jones/lazarus/commit/19c60e1aff11e468f106eb3a81b1860a537eb023): Fix: Removed sized-types which aren't included in the standard
* [efa8cb3](https://github.com/ford-jones/lazarus/commit/efa8cb353e87dc82dda2eb6db33c77191b3cfe01): Update: Ensures activation of correct texture units relative to texture load
* [a88c8cf](https://github.com/ford-jones/lazarus/commit/a88c8cffb0001968f28f4bd562f73920b946b4fe): Fix: Resolved conflicts with upstream
* [7e5b5ea](https://github.com/ford-jones/lazarus/commit/7e5b5eae93d0361c01f014e249deb6e8ba18a4db): Update: Codebase refactor - sized types
* [83e8ae9](https://github.com/ford-jones/lazarus/commit/83e8ae9e244db610e66513ac8c25ed08d60c8a57): Update: Code refactor - Sized types
* [e7aae32](https://github.com/ford-jones/lazarus/commit/e7aae3274d607c6261050f01b1c678d5b7a378d1): Fix: Corrected whitespace glyph uv-coordinates
* [dc0a79d](https://github.com/ford-jones/lazarus/commit/dc0a79db14de16f45243ed7a89605fa971283a35): Fix: Dynamically lookup y-axis uv coordinates per-glyph
* [ef8633a](https://github.com/ford-jones/lazarus/commit/ef8633af538fb2a31c67cc95eec32c6e21142020): Fix: Resolved texture atlas layout when working with multiple fonts
* [6d91859](https://github.com/ford-jones/lazarus/commit/6d91859a99ba17ad4d68b10dbd7190e8d2160065): Update: Extended error checking to bitmap texture loader
* [e9fddc2](https://github.com/ford-jones/lazarus/commit/e9fddc29bf26e8d76c152dc1ef63d396c50c052c): Update: Internally tracked font properties
* [fd74f3d](https://github.com/ford-jones/lazarus/commit/fd74f3da7f29cb2131ac15b2d7f93863822a2816): Fix: Reallocates additional texture storage for new font parses
* [294e5e9](https://github.com/ford-jones/lazarus/commit/294e5e955394dc090361b4277af09a6dc96b1cb5): Fix: Updated rotation function parameter names
* [0094704](https://github.com/ford-jones/lazarus/commit/0094704824809388a7a6677e5ee1ef6dd4f31641): Fix: Corrections to release notes linting
